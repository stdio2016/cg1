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
	glutCreateWindow("Assignment 1-2");
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
		sc->camera.eye = Vec3(eye[0] * a - eye[2] * b, eye[1], eye[0] * b + eye[2] * a) + sc->camera.vat;
		break;
	case 'd': case 'D':
		sc->camera.eye = Vec3(eye[0] * a + eye[2] * b, eye[1], -eye[0] * b + eye[2] * a) + sc->camera.vat;
		break;
	}
	if (key >= '1' && key <= '9') {
		size_t idx = key - '1';
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
	printf("mouse drag to %d,%d\n", x, y);
	if (selection >= sc->displayObjs.size()) return ;
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
	ratio *= 2.0f * tanf(sc->camera.fovy * (3.14159265f / 360.0f));
	// calculate delta
	p = sc->displayObjs[selection].transform;
	p = p + right * ((x - lastX) * ratio) + up * ((lastY - y) * ratio);
	sc->displayObjs[selection].transform = p;
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
