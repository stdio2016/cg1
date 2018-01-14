#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include "include/GL/glew.h"
#include "ShaderLoader.h"

void showShaderError(unsigned int shader) {
	GLint len = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	GLchar *dat = new GLchar[len];
	GLint lenall = len;
	glGetShaderInfoLog(shader, len, &lenall, dat);
	puts(dat);
	delete dat;
}

void showShaderProgError(unsigned int prog) {
	GLint len = 0;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len == 0) return;
	GLchar *dat = new GLchar[len];
	GLint lenall = len;
	glGetProgramInfoLog(prog, len, &lenall, dat);
	puts(dat);
	delete dat;
}

unsigned int LoadShader(const char *name, unsigned int shaderType) {
	FILE *f = fopen(name, "r");
	if (NULL == f) {
		printf("Unable to open shader '%s'\n", name);
		return 0;
	}
	GLchar *buf = new GLchar[MAX_SHADER_CODE_SIZE];
	GLchar **bufp = &buf;
	int n = fread(buf, 1, MAX_SHADER_CODE_SIZE, f);
	fclose(f);
	if (n < 0) {
		printf("Error reading file '%s'\n", name);
		delete buf;
		return 0;
	}
	if (n >= MAX_SHADER_CODE_SIZE) {
		printf("Shader '%s' is too big!\n", name);
		delete buf;
		return 0;
	}
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0) { printf("error creating shader\n"); delete buf; return 0; }

	int *sizearr = &n; // only 1 item in this array
	glShaderSource(shader, 1, bufp, sizearr);
	delete buf;

	glCompileShader(shader);
	GLint isCompiled = false;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (!isCompiled) {
		printf("shader '%s' compile error\n", name);
		showShaderError(shader);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}
