// this exists so that we can include rlgl and raymath and have them compiled in
// c compiler mode by visual studio since it uses C99 stuff

#define RAYMATH_IMPLEMENTATION
#include <raymath.h>

#define RLGL_IMPLEMENTATION
#ifdef __EMSCRIPTEN__
#define GRAPHICS_API_OPENGL_ES2
#else
#define GRAPHICS_API_OPENGL_33
#endif
#include <rlgl.h>

#ifdef __EMSCRIPTEN__
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#define FONTSTASH_IMPLEMENTATION
#include "external/fontstash.h"
#define GLFONTSTASH_IMPLEMENTATION
#include "external/gl3corefontstash.h"

bool initGL(int width, int height) {
#ifndef __EMSCRIPTEN__
	if (glewInit() != GLEW_OK) {
		return false;
	}
#endif

	rlglInit(width, height);

	// Initialize viewport and internal projection/modelview matrices
	rlViewport(0, 0, width, height);
	rlMatrixMode(RL_PROJECTION);                        // Switch to PROJECTION matrix
	rlLoadIdentity();                                   // Reset current matrix (PROJECTION)
	rlOrtho(0, width, height, 0, 0.0f, 1.0f); // Orthographic projection with top-left corner at (0,0)
	rlMatrixMode(RL_MODELVIEW);                         // Switch back to MODELVIEW matrix
	rlLoadIdentity();                                   // Reset current matrix (MODELVIEW)

	rlClearColor(0, 0, 0, 255);                   // Define clear color

	return true;
}
