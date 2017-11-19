#include "LightSystem.h"
#include <fstream>
LightSystem::LightSystem()
{
	Init();
}

LightSystem::LightSystem(std::string filename) {
	LoadLight(filename);
}

void LightSystem::Init() {
	lights.clear();
	for (int i = 0; i < 4; i++) {
		ambient[i] = 0;
	}
}

void LightSystem::LoadLight(std::string filename) {
	Init();
	std::ifstream f;
	f.open(filename);
	if (!f.is_open()) {
		printf("Cannot open file \"%s\"\n", filename.c_str());
		return;
	}
	while (!f.eof()) {
		std::string cmd;
		f >> cmd;
		if (cmd.compare("light") == 0) {
			Light I;
			f >> I.position.p[0] >> I.position.p[1] >> I.position.p[2];
			f >> I.Ia[0] >> I.Ia[1] >> I.Ia[2];
			f >> I.Id[0] >> I.Id[1] >> I.Id[2];
			f >> I.Is[0] >> I.Is[1] >> I.Is[2];
			I.Ia[3] = I.Id[3] = I.Is[3] = 1.0;
			lights.push_back(I);
		}
		else if (cmd.compare("ambient") == 0) {
			f >> ambient[0] >> ambient[1] >> ambient[2];
		}
	}
	f.close();
}

LightSystem::~LightSystem()
{
}
