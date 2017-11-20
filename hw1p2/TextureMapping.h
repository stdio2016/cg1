#pragma once
#include "include/glut.h"

class TextureMapping
{
public:
	virtual void switchTexture(void);
};

class NoTextureMapping : public TextureMapping {
public:
	virtual void switchTexture(void);
	GLuint texId;
};

class SingleTextureMapping : public TextureMapping {
public:
	GLuint texId;
	SingleTextureMapping(GLuint texId);
	virtual void switchTexture(void);
};
