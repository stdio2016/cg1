#include "SceneManager.h"
// constants
const std::string SceneManager::DefaultCameraFileName = "view.view";
const std::string SceneManager::DefaultLightFileName = "light.light";

SceneManager::SceneManager(): camera()
{
	Init();
}

void SceneManager::Init() {
	camera = Camera();
}

SceneManager::SceneManager(std::string filename) {
	LoadScene(filename);
}

void SceneManager::LoadScene(std::string filename) {
	// load camera file
	camera = Camera(SceneManager::DefaultCameraFileName);
	// load lights file
	light = LightSystem(SceneManager::DefaultLightFileName);
}

SceneManager::~SceneManager()
{
}
