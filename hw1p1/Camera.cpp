#include "Camera.h"
#include "include/glut.h"
#include <fstream>
#include <cstdio>

Camera::Camera()
{
	Init();
}

void Camera::Init() {
	// default values
	eye = Vec3(200, 300, 150);
	vat = Vec3(0, 0, 0);
	vup = Vec3(0, 1, 0);
	fovy = 60;
	dnear = 1;
	dfar = 1000;

	viewX = 0;
	viewY = 0;
	viewWidth = 500;
	viewHeight = 500;
}

Camera::Camera(std::string filename) {
	LoadSettings(filename);
}

void Camera::LoadSettings(std::string filename) {
	std::ifstream f;
	f.open(filename);
	if (!f.is_open()) {
		printf("Cannot open file \"%s\"\n", filename.c_str());
		return;
	}
	while (!f.eof()) {
		std::string cmd;
		f >> cmd;
		if (cmd.compare("eye") == 0) {
			f >> eye[0] >> eye[1] >> eye[2];
		}
		else if (cmd.compare("vat") == 0) {
			f >> vat[0] >> vat[1] >> vat[2];
		}
		else if (cmd.compare("vup") == 0) {
			f >> vup[0] >> vup[1] >> vup[2];
		}
		else if (cmd.compare("fovy") == 0) {
			f >> fovy;
		}
		else if (cmd.compare("dnear") == 0) {
			f >> dnear;
		}
		else if (cmd.compare("dfar") == 0) {
			f >> dfar;
		}
		else if (cmd.compare("viewport") == 0) {
			f >> viewX >> viewY >> viewWidth >> viewHeight;
		}
	}
	f.close();
}
