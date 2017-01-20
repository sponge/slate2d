#pragma once
#include <GL/glew.h>
#include "shader.h"

struct ShaderQuad {
    ShaderQuad();
    ~ShaderQuad();

    Shader *shader;

    void AttachShader(Shader *sh);
    void Render();

private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};