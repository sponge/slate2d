#include "shaderquad.h"
#include "../local.h"
#include <GL/glew.h>

ShaderQuad::ShaderQuad() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create a Vertex Buffer Object and copy the vertex data to it;
	glGenBuffers(1, &vbo);

	GLfloat vertices[] = {
		-1.0f,  1.0f, -1.0f, -1.0f, // Top-left
         1.0f,  1.0f,  1.0f, -1.0f, // Top-right
         1.0f, -1.0f,  1.0f,  1.0f, // Bottom-right
		-1.0f, -1.0f, -1.0f,  1.0f  // Bottom-left
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &ebo);

	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

ShaderQuad::~ShaderQuad() {
    glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void ShaderQuad::AttachShader(Shader *sh) {
    shader = sh;

    GLint posAttrib = glGetAttribLocation(sh->program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	GLint texAttrib = glGetAttribLocation(sh->program, "iFragCoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
}

void ShaderQuad::Render() {
	shader->Bind();

	GLint time = glGetUniformLocation(shader->program, "iGlobalTime");
	glUniform1f(time, SDL_GetTicks() / 1000.0f);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}