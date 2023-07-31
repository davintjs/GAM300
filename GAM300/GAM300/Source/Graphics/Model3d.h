#pragma once
#include "Precompiled.h"
#include "IOManager/Handler_GLFW.h"
#include "glslshader.h"
#include "../../glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h"
#include "LapSupGraphics/Compiler.h"
#include "LapSupGraphics/Mesh.h"

//#include "../../Compiler/Mesh.h"

// Just a wrapper class to hold all these values
//class Geom
//{
//public:
//	std::vector<Mesh> mMeshes; // Total submeshes of this geom
//
//	// Model loader values
//	glm::vec3 mPosCompressionScale;
//	glm::vec2 mTexCompressionScale;
//	std::vector<Material> _materials{};
//};

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

	//std::vector<Geom> totalGeoms; // Contains all geom of the project

private:
	void DeserializeGeoms(const std::string filePath);
};