#include <cstdio>
#include <cmath>
#include "include/GL/glew.h"
#include "include/glut.h"
#include "SceneManager.h"
//#define DEBUG

void keyboardInput(unsigned char key, int x, int y);
int lastX, lastY;
size_t selection = 0;
void mouseClick(int button, int state, int x, int y);
void mouseDrag(int x, int y);
void display(void);
void reshape(int width, int height);
int screenWidth, screenHeight;

int frameCount = 0;
SceneManager *sc;

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	sc = new SceneManager();
	// load viewport settings
	sc->camera.LoadSettings(SceneManager::DefaultCameraFileName);
	glutInitWindowSize(sc->camera.viewWidth, sc->camera.viewHeight);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL | GLUT_ACCUM);
	glutCreateWindow("Assignment 2");

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		printf("%s\n", glewGetErrorString(err));
	}
	// after gl initialize
	sc->LoadScene(SceneManager::DefaultSceneFileName);
	glutKeyboardFunc(keyboardInput);
	glutMouseFunc(mouseClick);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
}

void needRedraw() {
#ifdef DEBUG
	sc->slowDraw = false;
#endif
	glutPostRedisplay();
}

void keyboardInput(unsigned char key, int x, int y) {
	Vec3 eye = sc->camera.eye - sc->camera.vat;
	Vec3 up = sc->camera.vup;
	up = up * (1 / up.magnitude());
	Vec3 right = eye.cross(up);
	Vec3 front = up.cross(right);
	up = up * (eye.dot(up));
	const float b = 0.05f, a = sqrtf(1 - b * b);
	const float ratio = 1.05f;
	switch (key) {
	case 'w': case 'W':
		if (eye.magnitude() > sc->camera.dnear) {
			sc->camera.eye = eye * (1/ratio) + sc->camera.vat;
		}
		break;
	case 's': case 'S':
		if (eye.magnitude() < sc->camera.dfar) {
			sc->camera.eye = eye * ratio + sc->camera.vat;
		}
		break;
	case 'a': case 'A':
		sc->camera.eye = right * b + front * a + up + sc->camera.vat;
		break;
	case 'd': case 'D':
		sc->camera.eye = right * -b + front * a + up + sc->camera.vat;
		break;
	case '1':
		sc->camera.vat = Vec3(-10,15,0);
		break;
	case '2':
		sc->camera.vat = Vec3(-50,15,0); // first reflecction at -70,12,0
		break;
	case '3':
		sc->camera.vat = Vec3(-400,15,0); // second reflection at -170,12,0
		break;
	case '+':
		selection += 10;
		if (selection >= sc->displayObjs.size()) {
			selection = sc->displayObjs.size() - 1;
		}
		break;
	case '-':
		selection -= 10;
		if (selection >= sc->displayObjs.size()) {
			selection = 0;
		}
		break;
	}
	if (key >= '0' && key <= '9') {
		size_t idx = key == '0' ? 9 : key - '1';
		if (idx < sc->displayObjs.size()) {
			selection = idx;
		}
	}
	printf("key %d %d %d\n", key, x, y);
	needRedraw();
}

void mouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			lastX = x;
			lastY = y;
			glutMotionFunc(mouseDrag);
		}
		if (state == GLUT_UP) {
			glutMotionFunc(NULL);
		}
	}
}

void mouseDrag(int x, int y) {
	printf("mouse drag to %d,%d\n", x, y);
	if ((size_t) selection >= sc->displayObjs.size()) return ;
	// calculate eye coordinate axes
	Vec3 front = sc->camera.vat - sc->camera.eye;
	front = front * (1 / front.magnitude());
	Vec3 up = sc->camera.vup;
	Vec3 right = front.cross(up);
	up = right.cross(front);
	up = up * (1 / up.magnitude());
	right = right * (1 / right.magnitude());
	// calculate ratio
	Vec3 p = sc->displayObjs[selection].transform - sc->camera.eye;
	float ratio = p.dot(front) / sc->camera.viewHeight;
	ratio *= 2.0f * tanf((float)sc->camera.fovy * (3.14159265f / 360.0f));
	// calculate delta
	p = sc->displayObjs[selection].transform;
	p = p + right * ((x - lastX) * ratio) + up * ((lastY - y) * ratio);
	sc->displayObjs[selection].transform = p;
	lastX = x;
	lastY = y;
	needRedraw();
}

void slowDrawCallback(int t) {
	if (t == frameCount) {
		sc->slowDraw = true;
		glutPostRedisplay();
	}
}

void display() {
	sc->display();
	frameCount++;
	if (!sc->slowDraw) {
		glutTimerFunc(1000, slowDrawCallback, frameCount);
	}
}

void reshape(int width, int height) {
	printf("%dx%d\n", width, height);
	screenHeight = height;
	screenWidth = width;
}
