/*!***************************************************************************************
\file			MeshManager.h
\project
\author         Euan Lim, Davin Tan,Jake Lian, Theophelia Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the Mesh Manager and it's related structs:
	1. Loads Geoms
	2. Creates our own meshes - Cube,Sphere,Line
	3. Sets up all the VAOs, VBOs and set up for instancing

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once
#include "Precompiled.h"
#include "glslshader.h"
#include "../../glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h"
#include "AnimationManager.h"
#include "TextureManager.h"
#include "AssetManager/AssetManager.h"

#include "../../Compiler/Mesh.h"

#include "Model3d.h"

#define MAX_BONE_INFLUENCE 4

struct InstanceProperties;

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

//class AnimationMesh: public Mesh
//{
//};

// While Porting into the game
struct gVertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 tex;
	glm::ivec4 color;
};

class gMesh {
public:
	std::vector<gVertex> _vertices; // This individual mesh vertices
	std::vector<unsigned int> _indices; // This individual mesh indices

	glm::vec3 mPosCompressionScale{}; // Scale value according to the bounding box of the vertices positions of this sub mesh
	glm::vec2 mTexCompressionScale{}; // Scale value according to the bounding box of the texture coordinates of this sub mesh

	glm::vec3 mPosCompressionOffset{}; // This individual mesh vertices' positions' center offset from original
	glm::vec2 mTexCompressionOffset{}; // This individual mesh textures' coordinates' center offset from original

	int materialIndex = 0; // Material index

	gMesh() {};

};

// This is the "MiddleMan between loading from geom into making a mesh
class GeomImported
{
public:
	std::vector<gMesh> mMeshes; // Total submeshes of this geom

	// Model loader values
	//glm::vec3 mPosCompressionScale;
	//glm::vec2 mTexCompressionScale;

	std::vector<Material> _materials{};
};

// testing animation stuff, trying to make it fit existing system more
struct gAnimVertex: public gVertex
{
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

class gAnimMesh {
public:
	std::vector<gAnimVertex> _vertices; // This individual mesh vertices
	std::vector<unsigned int> _indices; // This individual mesh indices
	//std::vector<TextureInfo> _textures; //temporarily dont think about textures, mb can repurpose and take from geom file


	//glm::vec3 mPosCompressionScale{}; // Scale value according to the bounding box of the vertices positions of this sub mesh
	//glm::vec2 mTexCompressionScale{}; // Scale value according to the bounding box of the texture coordinates of this sub mesh

	//glm::vec3 mPosCompressionOffset{}; // This individual mesh vertices' positions' center offset from original
	//glm::vec2 mTexCompressionOffset{}; // This individual mesh textures' coordinates' center offset from original

	//int materialIndex = 0; // Material index

	gAnimMesh () {};
	//gAnimMesh(std::vector<gAnimVertex> vertices, std::vector<unsigned int> indices) : _vertices{ vertices }, _indices{ indices } {};

};

// This is the "MiddleMan between loading from geom into making a mesh
class AnimGeomImported
{
public:
	std::vector<gAnimMesh> mMeshes; // Total submeshes of this geom
	//std::vector<TextureInfo> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

	// Model loader values
	//glm::vec3 mPosCompressionScale;
	//glm::vec2 mTexCompressionScale;

	//std::vector<Material> _materials{};
	//Animation oneAnimation; // temp, mb need to make it a vec to store more anim next time

};


#define MeshManager MESH_Manager::Instance()

SINGLETON(MESH_Manager)
{

public:
	
	void Init();
	void Update(float dt);
	void Exit();

	void GetGeomFromFiles(const std::string& filePath, const std::string& fileName);


	// This is used when we are going to draw, u need to take the geom then render it
	Mesh* DereferencingMesh(std::string mesh_Name) 
	{ 
		if (mContainer.find(mesh_Name) == mContainer.end())
		{
			return nullptr;
		}
		return &(mContainer.find(mesh_Name)->second); 
	
	}// Either Geom or Vaoid



	// FUnction to load model

	//void AddTexture(char const* Filename, std::string GUID);
	//GLuint& GetTexture(std::string GUID);
	//GLuint CreateTexture(char const* Filename);
	std::unordered_map<std::string, Mesh> mContainer;
	std::map<std::string, InstanceProperties>* properties;
private:
	// To load Geoms from FBXs
	GeomImported DeserializeGeoms(const std::string& filePath, const std::string& fileName);
	AnimGeomImported DeserializeAnimGeoms(const std::string& filePath, const std::string& fileName); // i think can combine w the original fn
	// Decompress
	void DecompressVertices(std::vector<gVertex>& mMeshVertices, 
		const std::vector<Vertex>& oVertices,
		const glm::vec3& mPosCompressScale,
		const glm::vec2& mTexCompressScale,
		const glm::vec3& mPosOffset,
		const glm::vec2& mTexOffset);


	void CreateInstanceCube();
	void CreateInstanceSphere();
	void CreateInstanceLine();
	void CreateInstanceSegment3D();

	// Creates all the related buffers - Blinn Phong
	unsigned int InstanceSetup(InstanceProperties& prop);

	// Creates all the related buffers - PBR
	unsigned int InstanceSetup_PBR(InstanceProperties& prop);

	// Did not make this version because i realized that its all within instance properties
	//unsigned int InstanceSetup_MAT(InstanceProperties& prop);

	void debugAABB_setup(glm::vec3 minpt, glm::vec3 maxpt, InstanceProperties& prop); // vao

};
