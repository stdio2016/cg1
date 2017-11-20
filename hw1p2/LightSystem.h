#pragma once
#include <vector>
#include <string>
#include "Vec3.h"

struct Light {
	Vec3 position; // light position
	float Ia[4]; // ambient term
	float Id[4]; // diffuse term
	float Is[4]; // speculat term
};

class LightSystem
{
public:
	std::vector<Light> lights;
	float ambient[4]; // ambient light from environment

	LightSystem();
	LightSystem(std::string filename);

	void Init();
	void LoadLight(std::string filename);

	~LightSystem();
};
