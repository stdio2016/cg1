#include <cstdio>
#include <cmath>
#include "include/glut.h"
#include "Mesh.h"
#include "SceneManager.h"

void keyboardInput(unsigned char key, int x, int y);
void display(void);
void reshape(int width, int height);
int screenWidth, screenHeight;
GLfloat theta = 0.57, distance = 400;

Mesh *obj;
SceneManager *sc;

int main(int argc, char *argv[]) {
	sc = new SceneManager("scene.scene");
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
		theta += 0.1f;
		break;
	case 'd': case 'D':
		theta -= 0.1f;
		break;
	}
	printf("key %d %d %d\n", key, x, y);
	glutPostRedisplay();
}

void light()
{
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_position[] = { 150, 150, 150, 1.0 };

	glShadeModel(GL_SMOOTH);

	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient);
}

void setMaterial(const Material &mat) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat.Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat.Ks);
	glMaterialfv(GL_FRONT, GL_SHININESS, &mat.Ns);
}

void drawObject(Mesh *obj) {
	int lastMaterial = -1;
	glBegin(GL_TRIANGLES);
	for (size_t i = 0; i < obj->fTotal; i++) {
		if (lastMaterial != obj->faceList[i].m) {
			glEnd();
			lastMaterial = obj->faceList[i].m;
			setMaterial(obj->mList[lastMaterial]);
			glBegin(GL_TRIANGLES);
		}
		//glBegin(GL_TRIANGLES);
		for (int j = 0; j < 3; j++) {
			glNormal3fv(obj->nList[obj->faceList[i].v[j].n].p);
			glVertex3fv(obj->vList[obj->faceList[i].v[j].v].p);
		}
		//glEnd();
	}
	glEnd();
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

	light();

	drawObject(obj);

	glutSwapBuffers();
}

void reshape(int width, int height) {
	printf("%dx%d\n", width, height);
	screenHeight = height;
	screenWidth = width;
}