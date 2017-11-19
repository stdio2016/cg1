#include "SceneManager.h"
#include <fstream>
#include "include/glut.h"

// constants
const std::string SceneManager::DefaultCameraFileName = "view.view";
const std::string SceneManager::DefaultLightFileName = "light.light";
const std::string SceneManager::DefaultSceneFileName = "scene.scene";

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

void SceneManager::LoadScene(std::string filename) {
	// load camera file
	camera = Camera(SceneManager::DefaultCameraFileName);
	// load lights file
	light = LightSystem(SceneManager::DefaultLightFileName);
	// load scene file
	std::ifstream f;
	f.open(filename);
	if (!f.is_open()) {
		printf("Cannot open file \"%s\"\n", filename.c_str());
		return;
	}
	while (!f.eof()) {
		std::string cmd;
		f >> cmd;
		if (cmd.compare("model") == 0) {
			std::string name;
			f >> name;
			DisplayObject obj;
			size_t meshId = meshMap[name];
			if (meshId == 0) {
				Mesh *m = new Mesh(name.c_str());
				meshes.push_back(m);
				meshId = meshes.size();
				meshMap[name] = meshId;
			}
			obj.meshIndex = meshId;
			f >> obj.scale[0] >> obj.scale[1] >> obj.scale[2];
			f >> obj.angle;
			f >> obj.rotationAxis[0] >> obj.rotationAxis[1] >> obj.rotationAxis[2];
			f >> obj.transform[0] >> obj.transform[1] >> obj.transform[2];
			displayObjs.push_back(obj);
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
	glScalef(obj.scale[0], obj.scale[1], obj.scale[2]);
	glRotatef((GLfloat)obj.angle, obj.rotationAxis[0], obj.rotationAxis[1], obj.rotationAxis[2]);
	glTranslatef(obj.transform[0], obj.transform[1], obj.transform[2]);

	drawObject(meshes[obj.meshIndex-1]);

	glPopMatrix();
}

void SceneManager::drawObject(Mesh *obj) {
	int lastMaterial = -1;
	glBegin(GL_TRIANGLES);
	for (size_t i = 0; i < obj->fTotal; i++) {
		if (lastMaterial != obj->faceList[i].m) {
			glEnd();
			lastMaterial = obj->faceList[i].m;
			setMaterial(obj->mList[lastMaterial]);
			glBegin(GL_TRIANGLES);
		}
		for (int j = 0; j < 3; j++) {
			glNormal3fv(obj->nList[obj->faceList[i].v[j].n].p);
			glVertex3fv(obj->vList[obj->faceList[i].v[j].v].p);
		}
	}
	glEnd();
}

void SceneManager::setMaterial(const Material &mat) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat.Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat.Ks);
	glMaterialfv(GL_FRONT, GL_SHININESS, &mat.Ns);
}
