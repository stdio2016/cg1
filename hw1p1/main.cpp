#include <cstdio>
#include <cmath>
#include "include/glut.h"
#include "SceneManager.h"

void keyboardInput(unsigned char key, int x, int y);
int lastX, lastY;
int selection = 0;
void mouseClick(int button, int state, int x, int y);
void mouseDrag(int x, int y);
void display(void);
void reshape(int width, int height);
int screenWidth, screenHeight;

SceneManager *sc;

int main(int argc, char *argv[]) {
	sc = new SceneManager(SceneManager::DefaultSceneFileName);
	glutInit(&argc, argv);
	glutInitWindowSize(sc->camera.viewWidth, sc->camera.viewHeight);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Assignment 1-1");
	glutKeyboardFunc(keyboardInput);
	glutMouseFunc(mouseClick);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
}

void keyboardInput(unsigned char key, int x, int y) {
	Vec3 eye = sc->camera.eye - sc->camera.vat;
	const float b = 0.1f, a = sqrtf(1 - b * b);
	const float ratio = 1.05f;
	switch (key) {
	case 'w': case 'W':
		if (sc->camera.eye.magnitude() > sc->camera.dnear) {
			sc->camera.eye = Vec3(eye[0] / ratio, eye[1] / ratio, eye[2] / ratio) + sc->camera.vat;
		}
		break;
	case 's': case 'S':
		if (sc->camera.eye.magnitude() < sc->camera.dfar) {
			sc->camera.eye = Vec3(eye[0] * ratio, eye[1] * ratio, eye[2] * ratio) + sc->camera.vat;
		}
		break;
	case 'a': case 'A':
		sc->camera.eye = Vec3(eye[0] * a + eye[2] * b, eye[1], -eye[0] * b + eye[2] * a) + sc->camera.vat;
		break;
	case 'd': case 'D':
		sc->camera.eye = Vec3(eye[0] * a - eye[2] * b, eye[1], eye[0] * b + eye[2] * a) + sc->camera.vat;
		break;
	}
	if (key >= '1' && key <= '9') {
		int idx = key - '1';
		if (idx < sc->displayObjs.size()) {
			selection = idx;
		}
	}
	printf("key %d %d %d\n", key, x, y);
	glutPostRedisplay();
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
	const float ratio = (sc->camera.vat - sc->camera.eye).magnitude() / sc->camera.viewWidth;
	printf("mouse drag to %d,%d\n", x, y);
	sc->displayObjs[selection].transform[0] += (x - lastX) * ratio;
	sc->displayObjs[selection].transform[1] += (lastY - y) * ratio;
	lastX = x;
	lastY = y;
	glutPostRedisplay();
}

void display() {
	sc->display();
}

void reshape(int width, int height) {
	printf("%dx%d\n", width, height);
	screenHeight = height;
	screenWidth = width;
}