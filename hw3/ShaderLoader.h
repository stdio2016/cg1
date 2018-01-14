#pragma once
#ifndef SHADERLOADER_INCLUDED
#define SHADERLOADER_INCLUDED
// I think most shaders are small
#define MAX_SHADER_CODE_SIZE 65536


void showShaderProgError(unsigned int prog);
// return 0 = error
unsigned int LoadShader(const char *filename, unsigned int shaderType);
#endif
