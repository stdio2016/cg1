#include "SceneManager.h"
// constants
const std::string SceneManager::DefaultCameraFileName = "camera.camera";

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
	// load camera file from
	camera = Camera(SceneManager::DefaultCameraFileName);
}

SceneManager::~SceneManager()
{
}
