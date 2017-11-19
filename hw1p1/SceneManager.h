#pragma once
#ifndef SCENEMANAGER_INCLUDED
#define SCENEMANAGER_INCLUDED
#include "Camera.h"
#include "Mesh.h"
#include <string>
#include <vector>
class SceneManager
{
public:
	// constants
	static const std::string DefaultCameraFileName;

	Camera camera;
	// TODO: display objects

	// initializer
	SceneManager(void);
	void Init(void);

	// initialize from file
	SceneManager(std::string filename);
	void LoadScene(std::string filename);

	// draw scene
	void display(void);

	// destructor
	~SceneManager();
private:
	// draw a mesh object
	void drawObject(Mesh *obj);

	// light setting
	void lightSetup(void);

	// camera setting
	void cameraSetup(void);
};

#endif