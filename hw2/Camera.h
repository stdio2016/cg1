#pragma once
#ifndef CAMERA_INCLUDED
#define CAMERA_INCLUDED
#include "Vec3.h"
#include <string>

class Camera
{
public:
	Vec3 eye; // eye position
	Vec3 vat; // camera look at
	Vec3 vup; // up vector
	double fovy; // field of view y angle
	double dnear; // near plane
	double dfar; // far plane

	// define the position of the viewport
	int viewX;
	int viewY;
	int viewWidth;
	int viewHeight;

	// initializer
	Camera();
	void Init();

	// initialize from file
	Camera(std::string filename);

	// load camera setting from file
	void LoadSettings(std::string filename);
};

#endif // CAMERA_INCLUDED
