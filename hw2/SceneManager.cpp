#include "include/GL/glew.h"
#include "SceneManager.h"
#include <fstream>
#include <cstring>
#include "include/glut.h"
// without this the program cannot link
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "corona.lib")
#include "include/corona.h"

// mirror index
#define MIRROR1  10
#define MIRROR2  11

SceneManager::SceneManager(): camera()
{
	Init();
}

void SceneManager::Init() {
	camera = Camera();
	light = LightSystem();
}

SceneManager::SceneManager(std::string filename) {
	LoadScene(filename);
}

struct MyImg {
	int width, height;
	unsigned char *pixels;
};

static MyImg getTexture(std::string filename) {
	MyImg m;
	m.pixels = NULL;
	filename = SceneManager::DefaultModelFolder + filename;
	corona::File *f = corona::OpenFile(filename.c_str(), false);
	if (f == NULL) {
		printf("Unable to open file %s\n", filename.c_str());
		return m;
	}
	corona::Image *img = corona::OpenImage(f, corona::FileFormat::FF_AUTODETECT, corona::PixelFormat::PF_R8G8B8A8);
	if (img == NULL) {
		printf("Unable to load texture %s\n", filename.c_str());
		return m;
	}
	printf("loaded texture %s\n", filename.c_str());
	m.width = img->getWidth();
	m.height = img->getHeight();
	m.pixels = new unsigned char[m.width * m.height * 4];
	// flip image around y axis
	const void *pix = img->getPixels();
	const unsigned char *p = (const unsigned char *) pix;
	size_t w = m.width * 4;
	for (int i = 0; i < m.height; i++) {
		memcpy(m.pixels + i * w, p + (m.height - i - 1) * w, w);
	}
	delete img;
	return m;
}

int SceneManager::LoadTexture(std::string filename, GLuint texId) {
	MyImg img = getTexture(filename);
	if (img.pixels == NULL) return -1;
	
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, img.pixels);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL,0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,10);
	delete img.pixels;
	return 0;
}

int SceneManager::LoadCubemapTexture(std::string files[6], GLuint texId) {
	const GLenum sides[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
	for (int i = 0; i < 6; i++) {
		MyImg img = getTexture(files[i]);
		if (img.pixels == NULL) return -1;

		glTexImage2D(sides[i], 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.pixels);
		delete img.pixels;
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL,0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL,10);
	return 0;
}

void SceneManager::LoadScene(std::string filename) {
	// camera need to be loaded before gl init
	// but scene need gl context
	//camera = Camera(SceneManager::DefaultCameraFileName);
	// load lights file
	light = LightSystem(SceneManager::DefaultLightFileName);
	// load scene file
	std::ifstream f;
	f.open(filename);
	if (!f.is_open()) {
		printf("Cannot open file \"%s\"\n", filename.c_str());
		return;
	}
	for (size_t i = 0; i < textureMappings.size(); i++) {
		delete textureMappings[i];
	}
	textureMappings.clear();
	TextureMapping *def = new TextureMapping;
	textureMappings.push_back(def);
	size_t texIndex = 0;
	while (!f.eof()) {
		std::string cmd;
		f >> cmd;
		if (cmd.compare("model") == 0) {
			std::string name;
			f >> name;
			name = DefaultModelFolder + name;
			DisplayObject obj;
			size_t meshId = meshMap[name];
			if (meshId == 0) {
				Mesh *m = new Mesh(name.c_str());
				meshes.push_back(m);
				meshId = meshes.size();
				meshMap[name] = meshId;
			}
			obj.meshIndex = meshId;
			obj.texIndex = texIndex;
			f >> obj.scale[0] >> obj.scale[1] >> obj.scale[2];
			f >> obj.angle;
			f >> obj.rotationAxis[0] >> obj.rotationAxis[1] >> obj.rotationAxis[2];
			f >> obj.transform[0] >> obj.transform[1] >> obj.transform[2];
			displayObjs.push_back(obj);
		}
		else if (cmd.compare("no-texture") == 0) {
			textureMappings.push_back(new NoTextureMapping);
			texIndex++;
		}
		else if (cmd.compare("single-texture") == 0) {
			std::string name;
			GLuint texId;
			f >> name;
			glGenTextures(1, &texId);
			printf("error %d\n", glGetError());
			LoadTexture(name, texId);
			textureMappings.push_back(new SingleTextureMapping(texId));
			texIndex++;
		}
		else if (cmd.compare("multi-texture") == 0) {
			std::string name1, name2;
			GLuint texIds[2];
			f >> name1 >> name2;
			glGenTextures(2, texIds);
			printf("error %d\n", glGetError());
			LoadTexture(name1, texIds[0]);
			LoadTexture(name2, texIds[1]);
			textureMappings.push_back(new MultiTextureMapping(texIds[0], texIds[1]));
			texIndex++;
		}
		else if (cmd.compare("cube-map") == 0) {
			std::string names[6];
			GLuint texId;
			for (int i = 0; i < 6; i++) {
				f >> names[i];
			}
			glGenTextures(1, &texId);
			printf("error %d\n", glGetError());
			LoadCubemapTexture(names, texId);
			textureMappings.push_back(new CubeMapMapping(texId));
			texIndex++;
		}
	}
	f.close();
}

SceneManager::~SceneManager()
{
	for (size_t i = 0; i < meshes.size(); i++) {
		delete meshes[i];
		meshes[i] = NULL;
	}
}

void SceneManager::display() {
	// clear everything
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);

	Camera a = camera;
	Vec3 front = camera.vat - camera.eye;
	front = front * (1 / front.magnitude());
	Vec3 up = camera.vup;
	Vec3 right = front.cross(up);
	up = right.cross(front);
	up = up * (1 / up.magnitude());
	right = right * (1 / right.magnitude());
	float jitter[9][2] = {
		{0.0f, 0.0f}, {0.0f, 0.2f}, {0.0f, -0.2f}, {0.2f, 0.0f}, {-0.2f, 0.0f},
		{0.2f, 0.2f}, {0.2f, -0.2f}, {-0.2f, 0.2f}, {-0.2f, -0.2f}
	};

	float r1 = (camera.focus - camera.vat).dot(front);
	float r2 = (camera.focus - camera.eye).dot(front);
	float r = r1 / r2;

	for (int i = 0; i < 8; i++) {
		camera.eye = a.eye + up * jitter[i][0] + right * jitter[i][1];
		camera.vat = a.vat + up * (r * jitter[i][0]) + right * (r * jitter[i][1]);
		cameraSetup();
		drawMirrored();
		camera = a;
		if (i == 0)
			glAccum(GL_LOAD, 1.0 / 8);
		else
			glAccum(GL_ACCUM, 1.0 / 8);
	}
	glAccum(GL_RETURN, 1);
	glutSwapBuffers();
}

void SceneManager::drawMirrored() {
	glClearDepth(1.0);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CCW);
	// draw scene
	drawScene();
	glFrontFace(GL_CW);
	drawObject(displayObjs[MIRROR1]);
	drawObject(displayObjs[MIRROR2]);
	glFrontFace(GL_CCW);

	// draw mirror
	float m1x = displayObjs[MIRROR1].transform.x() - 40;
	float m2x = displayObjs[MIRROR2].transform.x() + 40;
	glMatrixMode(GL_MODELVIEW);
	drawSceneInMirror(MIRROR1, m1x, MIRROR2, m2x);
	drawSceneInMirror(MIRROR2, m2x, MIRROR1, m1x);
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void SceneManager::drawSceneInMirror(int m1, float m1x, int m2, float m2x) {
	glClearStencil(0);
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glPushMatrix();
	drawMirrorInStencil(0, m1);
	drawSceneInStencil(0);
	glFrontFace(GL_CW);
	drawObject(displayObjs[m2]);
	glFrontFace(GL_CCW);
	float eyex = camera.eye.x();
	for (int i = 1; i <= 4; i++) {
		mirrorCamera(((i&1) ? m1 : m2), i, ((i&1) ? m1x : m2x), &eyex);
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glFrontFace((i&1) ? GL_CW : GL_CCW);
		drawMirrorInStencil(i, (i & 1 ? m2 : m1));
		drawSceneInStencil(i);
		drawScene(i);
	}
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();
}

void SceneManager::mirrorCamera(int mirror, int level, float mirrorX, float *eyex) {
	glMatrixMode(GL_MODELVIEW);
	GLdouble equa[4] = { mirror == MIRROR1 ? 1.0 : -1.0, 0.0, 0.0, 0.0 };
	glTranslatef(mirrorX * 2, 0, 0);
	glScalef(-1, 1, 1);
	*eyex = mirrorX * 2 - *eyex;
	glTranslatef(mirrorX, 0, 0);
	glClipPlane(GL_CLIP_PLANE0, equa);
	glTranslatef(-mirrorX, 0, 0);
	glEnable(GL_CLIP_PLANE0);
}

void SceneManager::drawMirrorInStencil(int level, int mirror) {
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilFunc(GL_EQUAL, level, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glFrontFace(level % 2 == 0 ? GL_CCW : GL_CW);
	drawObject(displayObjs[mirror]);
}

void SceneManager::drawSceneInStencil(int level) {
	glStencilFunc(GL_EQUAL, level + 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	drawScene();
}

void SceneManager::drawScene(int level) {
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(GL_EQUAL, level, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	drawScene();
}

void SceneManager::drawScene() {
	lightSetup();
	for (size_t i = 0; i < displayObjs.size(); i++) {
		if (i != MIRROR1 && i != MIRROR2)
			drawObject(displayObjs[i]);
	}
}

void SceneManager::cameraSetup() {
	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera.fovy, (GLfloat)camera.viewWidth / (GLfloat)camera.viewHeight, camera.dnear, camera.dfar);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera.eye[0], camera.eye[1], camera.eye[2],// eye
		camera.vat[0], camera.vat[1], camera.vat[2],     // center
		camera.vup[0], camera.vup[1], camera.vup[2]);    // up
}

void SceneManager::lightSetup() {
	int glconsts[8] = {
		GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3,
		GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7
	};
	glShadeModel(GL_SMOOTH);

	// enable lighting
	glEnable(GL_LIGHTING);

	// set light property
	for (size_t i = 0; i < light.lights.size(); i++) {
		if (i >= 8) { // too many lights, glut doesn't support that
			break;
		}
		int GlLightN = glconsts[i];
		glEnable(GlLightN);
		glLightfv(GlLightN, GL_POSITION, light.lights[i].position.p);
		glLightfv(GlLightN, GL_DIFFUSE, light.lights[i].Id);
		glLightfv(GlLightN, GL_SPECULAR, light.lights[i].Is);
		glLightfv(GlLightN, GL_AMBIENT, light.lights[i].Ia);
	}
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light.ambient);
}

void SceneManager::drawObject(DisplayObject obj) {
	glPushMatrix();
	glTranslatef(obj.transform[0], obj.transform[1], obj.transform[2]);
	glRotatef((GLfloat)obj.angle, obj.rotationAxis[0], obj.rotationAxis[1], obj.rotationAxis[2]);
	glScalef(obj.scale[0], obj.scale[1], obj.scale[2]);

	textureMappings[obj.texIndex]->switchTexture();
	drawObject(meshes[obj.meshIndex-1], textureMappings[obj.texIndex]);
	textureMappings[obj.texIndex]->rollback();
	glPopMatrix();
}

void SceneManager::drawObject(Mesh *obj, const TextureMapping *tm) {
	int lastMaterial = -1;
	glBegin(GL_TRIANGLES);
	for (size_t i = 0; i < obj->fTotal; i++) {
		if (lastMaterial != obj->faceList[i].m) {
			glEnd();
			lastMaterial = obj->faceList[i].m;
			setMaterial(obj->mList[lastMaterial]);
			glBegin(GL_TRIANGLES);
		}
		tm->handleMeshTexture(obj, i);
		/*for (int j = 0; j < 3; j++) {
			glTexCoord2fv(obj->tList[obj->faceList[i].v[j].t].p);
			glNormal3fv(obj->nList[obj->faceList[i].v[j].n].p);
			glVertex3fv(obj->vList[obj->faceList[i].v[j].v].p);
		}*/
	}
	glEnd();
}

void SceneManager::setMaterial(const Material &mat) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat.Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat.Ks);
	glMaterialfv(GL_FRONT, GL_SHININESS, &mat.Ns);
}
