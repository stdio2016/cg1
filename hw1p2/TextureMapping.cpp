#include "TextureMapping.h"

void TextureMapping::switchTexture() {
	// do nothing
}

void NoTextureMapping::switchTexture() {
	glDisable(GL_TEXTURE_2D);
}

SingleTextureMapping::SingleTextureMapping(GLuint texId) {
	this->texId = texId;
}

void SingleTextureMapping::switchTexture() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texId);
}
