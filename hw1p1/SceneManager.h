#pragma once
#ifndef SCENEMANAGER_INCLUDED
#define SCENEMANAGER_INCLUDED
#include "Camera.h"
#include "LightSystem.h"
#include "Mesh.h"
#include "DisplayObject.h"
#include <string>
#include <vector>
#include <map>

class SceneManager
{
public:
	// constants
	static const std::string DefaultCameraFileName;
	static const std::string DefaultLightFileName;
	static const std::string DefaultSceneFileName;

	Camera camera;
	LightSystem light;

	std::vector<DisplayObject> displayObjs;
	std::map<std::string, size_t> meshMap;
	std::vector<Mesh *> meshes;

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
	void drawObject(DisplayObject *obj);

	// light setting
	void lightSetup(void);

	// camera setting
	void cameraSetup(void);
};

#endif