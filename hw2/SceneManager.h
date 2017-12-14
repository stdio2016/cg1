#pragma once
#ifndef SCENEMANAGER_INCLUDED
#define SCENEMANAGER_INCLUDED
#include "Camera.h"
#include "LightSystem.h"
#include "Mesh.h"
#include "DisplayObject.h"
#include "TextureMapping.h"
#include <string>
#include <vector>
#include <map>
#include "include/glut.h"

class SceneManager
{
public:
	// constants
	static const std::string DefaultCameraFileName;
	static const std::string DefaultLightFileName;
	static const std::string DefaultSceneFileName;
	static const std::string DefaultModelFolder;

	Camera camera;
	LightSystem light;

	std::vector<DisplayObject> displayObjs;
	std::map<std::string, size_t> meshMap;
	std::vector<Mesh *> meshes;
	std::vector<TextureMapping *> textureMappings;

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
	// load texture
	int LoadTexture(std::string filename, GLuint texId);
	int LoadCubemapTexture(std::string files[6], GLuint texids);

	// draw a mesh object
	void drawObject(DisplayObject obj);
	void drawObject(Mesh *mesh, const TextureMapping *tm);
	void setMaterial(const Material &mat);

	// light setting
	void lightSetup(void);

	// camera setting
	void cameraSetup(void);

	// maybe one day it will be useful
	void drawSceneInMirror(int mirror1, float mirror1x, int mirror2, float mirror2x);
	void mirrorCamera(int mirror, int level, float mirrorX, float *eyex);
	void drawScene(void);
	void drawSceneInStencil(int level);
	void drawMirrorInStencil(int level, int mirror);
	void drawScene(int level);
};

#endif
