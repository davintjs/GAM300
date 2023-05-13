#pragma once
#include "../PCH/Precompiled.h"
#include "IOManager/Handler_GLFW.h"
#include "glslshader.h"
#include "../../glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h"
#include "LapSupGraphics/Compiler.h"
#include "LapSupGraphics/Mesh.h"

class Model {
public:
	void init(AssimpLoader* geom); // make vao vbo
	void setup_shader();

	GLenum prim;
	GLuint vaoid;
	GLuint vboid;
	GLuint drawcount;
	GLSLShader shader;

	void draw();

	AssimpLoader* _geom;
};