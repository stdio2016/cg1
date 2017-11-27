#include "TextureMapping.h"

void TextureMapping::switchTexture() {
	// do nothing
}

void TextureMapping::rollback() {
	// do nothing
}

void NoTextureMapping::switchTexture() {
	glDisable(GL_TEXTURE_2D);
}

void TextureMapping::handleMeshTexture(Mesh *obj, size_t i) const {
	for (int j = 0; j < 3; j++) {
		glNormal3fv(obj->nList[obj->faceList[i].v[j].n].p);
		glVertex3fv(obj->vList[obj->faceList[i].v[j].v].p);
	}
}

SingleTextureMapping::SingleTextureMapping(GLuint texId) {
	this->texId = texId;
}

void SingleTextureMapping::switchTexture() {
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
}

void SingleTextureMapping::rollback() {
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void SingleTextureMapping::handleMeshTexture(Mesh *obj, size_t i) const {
	for (int j = 0; j < 3; j++) {
		glTexCoord2fv(obj->tList[obj->faceList[i].v[j].t].p);
		glNormal3fv(obj->nList[obj->faceList[i].v[j].n].p);
		glVertex3fv(obj->vList[obj->faceList[i].v[j].v].p);
	}
}

MultiTextureMapping::MultiTextureMapping(GLuint texId1, GLuint texId2) {
	this->texId1 = texId1;
	this->texId2 = texId2;
}

void MultiTextureMapping::switchTexture() {
	//bind texture 0
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texId1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

	//bind texture 1
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texId2);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
}

void MultiTextureMapping::rollback() {
	//unbind texture 1
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	//unbind texture 0
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MultiTextureMapping::handleMeshTexture(Mesh *obj, size_t i) const {
	for (int j = 0; j < 3; j++) {
		glMultiTexCoord2fv(GL_TEXTURE0, obj->tList[obj->faceList[i].v[j].t].p);
		glMultiTexCoord2fv(GL_TEXTURE1, obj->tList[obj->faceList[i].v[j].t].p);
		glNormal3fv(obj->nList[obj->faceList[i].v[j].n].p);
		glVertex3fv(obj->vList[obj->faceList[i].v[j].v].p);
	}
}
