#pragma once
#include "Vec3.h"
class DisplayObject
{
public:
	size_t meshIndex;
	size_t texIndex;
	Vec3 scale;
	float angle;
	Vec3 rotationAxis;
	Vec3 transform;

	DisplayObject();
	~DisplayObject();
};

