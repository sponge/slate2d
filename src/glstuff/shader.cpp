#include "shader.h"
#include "../local.h"
#include <gl/GLU.h>
#include <physfs.h>

Shader::Shader()
{
}

bool Shader::LoadFromFile(const char * fragFile, const char * vertFile)
{
	const GLchar* vertexSource;
	const GLchar* fragmentSource;

	int outSz;
	outSz = FS_ReadFile(vertFile, (void **)&vertexSource);

	if (outSz <= 0) {
		return false;
	}

	outSz = FS_ReadFile(fragFile, (void **)&fragmentSource);

	if (outSz <= 0) {
		return false;
	}


	GLint status;

	// Create and compile the vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSource, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		delete vertexSource;
		delete fragmentSource;
		return false;
	}

	// Create and compile the fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSource, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		delete vertexSource;
		delete fragmentSource;
		return false;
	}

	// Link the vertex and fragment shader into a shader program
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glBindFragDataLocation(program, 0, "fragColor");
	glLinkProgram(program);

	valid = true;

	delete vertexSource;
	delete fragmentSource;

	return true;
}

void Shader::Bind()
{
	glUseProgram(program);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

Shader::~Shader() {
	glDeleteProgram(program);
	glDeleteShader(fragment);
	glDeleteShader(vertex);
}