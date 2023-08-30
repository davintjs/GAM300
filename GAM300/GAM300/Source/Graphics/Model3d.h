#pragma once
#include "Precompiled.h"
#include "IOManager/Handler_GLFW.h"
#include "glslshader.h"
#include "../../glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h"
#include "LapSupGraphics/Compiler.h"
#include "LapSupGraphics/Mesh.h"

class Model {
public:

	// This 3D Model version
	void init(AssimpLoader* geom); // make vao vbo
	void setup_shader();
	void draw();

	// This below are all the things i use to testing with light stuffs
	void cubeinit();
	void setup_lightshader();// The Light
	void setup_affectedShader();// Impacted by Light
	void lightSource_draw();
	void affectedByLight_draw(glm::vec3 lightPos);

	glm::vec3 position;

	GLenum prim;
	GLuint vaoid;
	GLuint vboid;
	GLuint texturebuffer{};
	GLuint drawcount;
	GLSLShader shader;


	AssimpLoader* _geom;
};