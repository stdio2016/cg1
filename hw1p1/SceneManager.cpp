#include "SceneManager.h"
#include <fstream>
// constants
const std::string SceneManager::DefaultCameraFileName = "view.view";
const std::string SceneManager::DefaultLightFileName = "light.light";
const std::string SceneManager::DefaultSceneFileName = "scene.scene";

SceneManager::SceneManager(): camera()
{
	Init();
}

void SceneManager::Init() {
	camera = Camera();
	light = LightSystem();
}

SceneManager::SceneManager(std::string filename) {
	LoadScene(filename);
}

void SceneManager::LoadScene(std::string filename) {
	// load camera file
	camera = Camera(SceneManager::DefaultCameraFileName);
	// load lights file
	light = LightSystem(SceneManager::DefaultLightFileName);
	// load scene file
	std::ifstream f;
	f.open(filename);
	if (!f.is_open()) {
		printf("Cannot open file \"%s\"\n", filename.c_str());
		return;
	}
	while (!f.eof()) {
		std::string cmd;
		f >> cmd;
		if (cmd.compare("model") == 0) {
			std::string name;
			f >> name;
			DisplayObject obj;
			size_t meshId = meshMap[name];
			if (meshId == 0) {
				Mesh *m = new Mesh(name.c_str());
				meshes.push_back(m);
				meshId = meshes.size();
				meshMap[name] = meshId;
			}
			obj.meshIndex = meshId;
			f >> obj.scale[0] >> obj.scale[1] >> obj.scale[2];
			f >> obj.angle;
			f >> obj.rotationAxis[0] >> obj.rotationAxis[1] >> obj.rotationAxis[2];
			f >> obj.transform[0] >> obj.transform[1] >> obj.transform[2];
			displayObjs.push_back(obj);
		}
	}
	f.close();
}

SceneManager::~SceneManager()
{
	for (int i = 0; i < meshes.size(); i++) {
		delete meshes[i];
		meshes[i] = NULL;
	}
}
