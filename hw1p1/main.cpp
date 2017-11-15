#include <cstdio>
#include <cmath>
#include "include/glut.h"
#include "Mesh.h"

void keyboardInput(unsigned char key, int x, int y);
void display(void);
void reshape(int width, int height);
int screenWidth, screenHeight;
GLfloat theta = 0.57, distance = 400;

Mesh *obj;

int main(int argc, char *argv[]) {
	obj = new Mesh("box.obj");
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Assignment 1-1");
	glutKeyboardFunc(keyboardInput);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
}

void keyboardInput(unsigned char key, int x, int y) {
	switch (key) {
	case 'w': case 'W':
		distance /= 1.1f; if (distance < 1.0) distance = 1;
		break;
	case 's': case 'S':
		distance *= 1.1f; if (distance > 1000.0) distance = 1000;
		break;
	case 'a': case 'A':
		theta -= 0.1f;
		break;
	case 'd': case 'D':
		theta += 0.1f;
		break;
	}
	printf("key %d %d %d\n", key, x, y);
}

void display() {
	// clear everything
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewport transformation
	glViewport(0, 0, screenWidth, screenHeight);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)screenWidth / (GLfloat)screenHeight, 1.0, 1000.0);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(distance * cos(theta), distance * 0.75, distance * sin(theta),// eye
		0.0, 0.0, 0.0,     // center
		0.0, 1.0, 0.0);    // up

	glBegin(GL_TRIANGLES);
	// right
	GLfloat u[3][3] = {100, 0, 0, 50, -50, 0, 50, 50, 0};
	glVertex3fv(u[0]);
	glVertex3fv(u[1]);
	glVertex3fv(u[2]);
	// left
	GLfloat v[3][3] = {-100, 0, 0, -50, -50, 0, -50, 50, 0 };
	glVertex3fv(v[0]);
	glVertex3fv(v[1]);
	glVertex3fv(v[2]);
	// up
	GLfloat a[3][3] = { 0, 100, 0, 50, 50, 0, -50, 50, 0 };
	glVertex3fv(a[0]);
	glVertex3fv(a[1]);
	glVertex3fv(a[2]);
	// down
	GLfloat b[3][3] = { 0, -100, 0, 50, -50, 0, -50, -50, 0 };
	glVertex3fv(b[0]);
	glVertex3fv(b[1]);
	glVertex3fv(b[2]);

	glEnd();
	glutSwapBuffers();
	glutPostRedisplay();
}

void reshape(int width, int height) {
	printf("%dx%d\n", width, height);
	screenHeight = height;
	screenWidth = width;
}