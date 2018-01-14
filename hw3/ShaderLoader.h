#pragma once
#ifndef SHADERLOADER_INCLUDED
#define SHADERLOADER_INCLUDED
// I think most shaders are small
#define MAX_SHADER_CODE_SIZE 65536

bool LoadShader(const char *filename, unsigned int shaderType);
#endif