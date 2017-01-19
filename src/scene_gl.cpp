#include "scene_gl.h"
#include <GL/glew.h>
#include "glstuff/shader.h"

GLuint vao;
GLuint vbo;
GLuint ebo;
Shader *shader;

void GLScene::Startup(ClientInfo* info) {

	inf = info;
	
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

	shader = new Shader();
	shader->LoadFromFile("shaders/default.frag", "shaders/default.vert");

	if (!shader->valid) {
		Com_Error(ERR_DROP, "problem constructing shader");
	}

    GLint posAttrib = glGetAttribLocation(shader->program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	GLint texAttrib = glGetAttribLocation(shader->program, "iFragCoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
}

void GLScene::Update(float dt) {
	_dt = dt;
}

void GLScene::Render() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw a rectangle from the 2 triangles using 6 indices
	shader->Bind();
	GLint uniColor = glGetUniformLocation(shader->program, "triangleColor");
	glUniform3f(uniColor, 1.0f, 0.0f, 0.0f);

	GLint time = glGetUniformLocation(shader->program, "iGlobalTime");
	glUniform1f(time, SDL_GetTicks() / 1000.0f);

	GLint delta = glGetUniformLocation(shader->program, "iGlobalDelta");
	glUniform1f(delta, _dt);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

GLScene::~GLScene() {
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}