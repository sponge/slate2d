#include "scene_gl.h"
#include <GL/glew.h>
#include "glstuff/shader.h"
#include "glstuff/shaderquad.h"

ShaderQuad *shq;

void GLScene::Startup(ClientInfo* info) {
	inf = info;
	
	shq = new ShaderQuad();

	auto shader = new Shader();
	shader->LoadFromFile("shaders/default.frag", "shaders/default.vert");

	if (!shader->valid) {
		Com_Error(ERR_DROP, "problem constructing shader");
	}

	shq->AttachShader(shader);
}

void GLScene::Update(float dt) {
	_dt = dt;
}

void GLScene::Render() {
	shq->Render();
}

GLScene::~GLScene() {

}