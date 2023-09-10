#pragma once
#include "Precompiled.h"
#include "IOManager/Handler_GLFW.h"
#include "glslshader.h"
#include "../../glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h"
// #include "LapSupGraphics/Compiler.h"
// #include "LapSupGraphics/Mesh.h"
#include "TextureManager.h"
#include "AssetManager/AssetManager.h"

#include "../../Compiler/Mesh.h"

// Just a wrapper class to hold all these values
class Geom
{
public:
	std::vector<Mesh> mMeshes; // Total submeshes of this geom

	// Model loader values
	//glm::vec3 mPosCompressionScale;
	//glm::vec2 mTexCompressionScale;

	std::vector<Material> _materials{};
};

//tempory model
struct GeneralModel {

	// this is a problem
	// coz got different number of instance
	// then the array all different...
	// maybe need sort and align models with the AOS?
	// tmr then think...

	int instance_count = 0; // the number of instance using this geom
	GLuint fbx_VAO;
	GLuint fbx_VBO;
	std::vector<GLuint> tex_VAO;
	std::vector<unsigned int> texure_index; // 
	void addtexure(); // adds all the texture that teh geom needs

};


// need restructure this whole entire thing
// separate instanced stuff
// maybe even seperate from the default cube.. think about this tmr khai kiat
class Model {
public:

	// This 3D Model version
	//void init(AssimpLoader* geom); // make vao vbo
	void init(); // make vao vbo
	void setup_shader();
	void draw();

	// This below are all the things i use to testing with light stuffs
	void cubeinit();
	void instance_cubeinit();
	void setup_instanced_shader();
	void setup_lightshader();// The Light
	void setup_affectedShader();// Impacted by Light
	void lightSource_draw();
	void affectedByLight_draw(glm::vec3 lightPos);
	void instanceDraw(int entitycount);

	

	glm::vec3 position;

	GLenum prim;
	GLuint vaoid;
	GLuint vboid;
	GLuint texturebuffer{};
	GLuint drawcount;
	GLSLShader shader;

	std::vector<GLuint> FBX_vaoid;
	std::vector<GLuint> FBX_vboid;
	std::vector<GLuint> FBX_drawcount;

	std::vector<GeneralModel> _mGeneral_model;

	// debug AABB
	GLuint vaoidAABB;
	GLSLShader shaderAABB;
	glm::vec3 pntAABB[8];
	std::vector<glm::ivec2> idxAABB{};
	void debugAABB_init(); // vao & shader
	void debugAABB_draw(glm::mat4& SRT);

	void lineinit();
	void debugline_draw(glm::mat4& SRT);



	//AssimpLoader* _geom;


	//AssimpLoader* _geom; 

	std::vector<Geom> totalGeoms; // Contains all geom of the project

private:
	void DeserializeGeoms(const std::string filePath);
};
