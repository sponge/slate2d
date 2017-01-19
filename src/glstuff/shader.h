#pragma once
#include <GL/glew.h>

struct Shader {
	Shader();
	bool LoadFromFile(const char * fragFile, const char * vertFile);
	bool valid;
	GLuint vertex;
	GLuint fragment;
	GLuint shader;

	void Bind();
	void Unbind();

};