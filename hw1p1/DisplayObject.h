#pragma once
#include "Vec3.h"
class DisplayObject
{
public:
	size_t meshIndex;
	Vec3 scale;
	float angle;
	Vec3 rotationAxis;
	Vec3 transform;

	DisplayObject();
	~DisplayObject();
};

