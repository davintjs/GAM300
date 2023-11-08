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

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once
#include "Precompiled.h"
#include "glslshader.h"
#include "../../glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h"
#include "AnimationManager.h"
#include "TextureManager.h"

#include "../../Compiler/Mesh.h"

#include "Model3d.h"

#define MAX_BONE_INFLUENCE 4

struct InstanceProperties;
struct DefaultRenderProperties;

using InstanceContainer = std::unordered_map<GLuint, InstanceProperties>; // <vao, properties>
// ACTUAL MESH USED IN GAME ENGINE
struct Mesh
{
	glm::vec3 vertices_min{};
	glm::vec3 vertices_max{};

	unsigned int index = 0;
	unsigned int numBones = 0;

	GLuint vaoID;
	GLuint vboID;
	GLuint drawCounts;
	GLuint SRTBufferIndex;

	GLenum prim;
};

#define MESHMANAGER MeshManager::Instance()

SINGLETON(MeshManager)
{

public:
	
	void Init();

	MeshAsset* GetMeshAsset(const Engine::GUID& meshID);

	void AddMesh(const MeshAsset& _meshAsset, const Engine::GUID& _guid);

	// This is used when we are going to draw, u need to take the geom then render it
	Mesh* DereferencingMesh(const Engine::GUID& meshID) 
	{ 
		if (mContainer.find(meshID) == mContainer.end())
		{
			return nullptr;
		}
		return &(mContainer.find(meshID)->second);
	}// Either Geom or Vaoid

	//GLuint GetVAOfromGUID(std::string GUID);

	// FUnction to load model

	//void AddTexture(char const* Filename, std::string GUID);
	//GLuint& GetTexture(std::string GUID);
	//GLuint CreateTexture(char const* Filename);
	//Handle mesh adding here
	void CallbackMeshAssetLoaded(AssetLoadedEvent<MeshAsset>* pEvent);

	//Handle mesh removal here
	void CallbackMeshAssetUnloaded(AssetUnloadedEvent<MeshAsset>* pEvent);
	std::unordered_map<Engine::GUID, GLuint> vaoMap; // <GUID, VAO> ... for now not guid, use meshname instead
	std::unordered_map<Engine::GUID, Mesh> mContainer;
	InstanceContainer* instanceProperties;
	//std::vector<InstanceContainer>* instanceContainers; // subscript represents shadertype
	std::vector<DefaultRenderProperties>* defaultProperties;

	std::unordered_map<Engine::GUID, geometryDebugData> offsetAndBoundContainer;

private:

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

	void debugAABB_setup(glm::vec3 minpt, glm::vec3 maxpt, const Engine::GUID& _guid, InstanceProperties& prop); // vao

};
