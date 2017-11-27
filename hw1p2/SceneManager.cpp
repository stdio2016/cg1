#include <GL/glew.h>
#include "SceneManager.h"
#include <fstream>
#include "include/glut.h"
// without this the program cannot link
#pragma comment (lib, "corona.lib")
#include "include/corona.h"


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

static corona::Image *getTexture(std::string filename) {
	filename = SceneManager::DefaultModelFolder + filename;
	corona::File *f = corona::OpenFile(filename.c_str(), false);
	if (f == NULL) {
		printf("Unable to open file %s\n", filename.c_str());
		return NULL;
	}
	corona::Image *img = corona::OpenImage(f, corona::FileFormat::FF_AUTODETECT, corona::PixelFormat::PF_R8G8B8A8);
	if (img == NULL) {
		printf("Unable to load texture %s\n", filename.c_str());
		return NULL;
	}
	printf("loaded texture %s\n", filename.c_str());
	return img;
}

int SceneManager::LoadTexture(std::string filename, GLuint texId) {
	corona::Image *img = getTexture(filename);
	if (img == NULL) return -1;
	int iWidth = img->getWidth();
	int iHeight = img->getHeight();
	
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, img->getPixels());
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL,0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,10);
	delete img;
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
			DisplayObject obj;
			size_t meshId = meshMap[name];
			if (meshId == 0) {
				name = DefaultModelFolder + name;
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);

	cameraSetup();
	lightSetup();
	for (size_t i = 0; i < displayObjs.size(); i++) {
		drawObject(displayObjs[i]);
	}
	glutSwapBuffers();
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
