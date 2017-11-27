#pragma once
#include "include/glut.h"
#include "Mesh.h"

class TextureMapping
{
public:
	virtual void switchTexture(void);
	// used after switchTexture, before other switchTexture
	virtual void rollback(void);
	virtual void handleMeshTexture(Mesh *obj, size_t i) const;
	virtual ~TextureMapping() {}
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
	virtual void rollback(void);
	virtual void handleMeshTexture(Mesh *obj, size_t i) const;
};

class MultiTextureMapping : public TextureMapping {
public:
	GLuint texId1, texId2;
	MultiTextureMapping(GLuint texId1, GLuint texId2);
	virtual void switchTexture(void);
	virtual void rollback(void);
	virtual void handleMeshTexture(Mesh *obj, size_t i) const;
};
