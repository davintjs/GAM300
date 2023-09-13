#pragma once
#include "Precompiled.h"
#include "IOManager/Handler_GLFW.h"
#include "glslshader.h"
#include "../../glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h"
#include "TextureManager.h"
#include "AssetManager/AssetManager.h"

#include "../../Compiler/Mesh.h"

#include "Model3d.h"

// ACTUAL MESH USED IN GAME ENGINE
struct Mesh
{
	std::vector<GLuint> Vaoids;
	std::vector<GLuint> Vboids;
	std::vector<GLuint> Drawcounts;
	std::vector<GLuint> SRT_Buffer_Index;

	GLenum prim;

	glm::vec3 vertices_min{};
	glm::vec3 vertices_max{};


	unsigned int index;
};


// This is the "MiddleMan between loading from geom into making a mesh
class GeomImported
{
public:
	std::vector<Geom_Mesh> mMeshes; // Total submeshes of this geom

	// Model loader values
	//glm::vec3 mPosCompressionScale;
	//glm::vec2 mTexCompressionScale;

	std::vector<Material> _materials{};
};





#define MeshManager MESH_Manager::Instance()

SINGLETON(MESH_Manager)
{

public:

	void Init();
	void Update(float dt);
	void Exit();

	void GetGeomFromFiles(const std::string filePath);


	// This is used when we are going to draw, u need to take the geom then render it
	Mesh& DereferencingMesh(std::string mesh_Name);// Either Geom or Vaoid



	// FUnction to load model

	//void AddTexture(char const* Filename, std::string GUID);
	//GLuint& GetTexture(std::string GUID);
	//GLuint CreateTexture(char const* Filename);
	std::unordered_map<std::string, Mesh> mContainer;

private:
	// To load Geoms from FBXs
	GeomImported DeserializeGeoms(const std::string filePath);

	void CreateInstanceCube();

	unsigned int InstanceSetup(GLuint vaoid , unsigned int index);


};
