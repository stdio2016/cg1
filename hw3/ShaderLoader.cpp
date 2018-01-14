#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include "include/glut.h"
#include "ShaderLoader.h"

bool LoadShader(const char *name, unsigned int shaderType) {
	FILE *f = fopen(name, "r");
	if (NULL == f) {
		printf("Unable to open shader '%s'\n", name);
		return false;
	}
	char buf[MAX_SHADER_CODE_SIZE];
	int n = fread(buf, 1, MAX_SHADER_CODE_SIZE, f);
	if (n < 0) {
		printf("Error reading file '%s'\n", name);
		fclose(f);
		return false;
	}
	fclose(f);
	if (n >= MAX_SHADER_CODE_SIZE) {
		printf("Shader '%s' is too big!\n", name);
		return false;
	}
	return false;
}