/*!***************************************************************************************
\file			ModelCompiler.h
\project
\author         Davin Tan
\co-author      Sean Ngo

\par			Course: GAM300
\date           29/10/2023

\brief
    This file contains the declarations of the following:
    1. Loading of FBX files with assimp loader
		a. Creation of meta file for the mesh
		b. Deserialization of FBX file
		b. Reading and storing of meshes
		c. Compression of vertices
	2. Serialization of meshes into custom binary format

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef MODELCOMPILER_H
#define MODELCOMPILER_H

#include <vector>
#include <filesystem>

#include "../../Compiler/Mesh.h"
#include "../../Compiler/BoundingBox.h"

#include "assimp/scene.h"
#include "assimp/mesh.h"

#include "Core/SystemInterface.h"

// Bean: Temporary
#include "Graphics/AnimationManager.h"

#define MODELCOMPILER ModelCompiler::Instance()

struct TempVertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 tex;
	glm::ivec4 color;

	// Animation Related Properties
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};

// The model components that are extracted from the aiScene which contains the fbx/obj data
// essentially only a portion of the data extracted from the file is needed thus this struct
// is technically the model itself which the engine will be using
struct ModelComponents
{
	std::vector<Geom_Mesh> meshes{};	// Individual meshes in the model, which also contains its individual vertices and indices
	std::vector<Material> materials{};	// Total materials of the WHOLE model (One mesh uses one material only)
	AnimationModel animations{};		// The animations contained on this model
};

SINGLETON(ModelCompiler)
{
public:

	// Load the FBX file with import options to be processed
	ModelComponents LoadModel(const std::filesystem::path& _filePath);

private:

	// Process the FBX file bones for animation
	void ProcessBones(const aiNode& _node, const aiScene& _scene);

	// Process the imported FBX file read from assimp library
	void ProcessNode(const aiNode& _node, const aiScene& _scene);

	// Process the mesh from the imported FBX file
	Geom_Mesh ProcessMesh(const aiMesh& _mesh, const aiScene& _scene);

	// Optimization of vertices of the FBX model
	void Optimize(std::vector<TempVertex>& _vert, std::vector<unsigned int>& _ind);

	// Compression of vertices to reduce custom binary custom file size
	void CompressVertices(std::vector<Vertex>& _compressVertices,
		const std::vector<TempVertex> _tempVertex,
		std::pair<glm::vec3, glm::vec2>& _mOffsets,
		std::pair<glm::vec3, glm::vec2>& _mScales);

	// Applies the transformations stated in the descriptor file onto the FBX model
	void TransformVertices(std::vector<TempVertex> _vert);

	// Import materials and textures of the FBX model
	void ImportMaterialAndTextures(const aiMaterial& _material);

	// Extract bone setting from the vertices in the model for animation
	void ExtractBoneWeightForVertices(std::vector<TempVertex>& _vert, const aiMesh& _mesh, const aiScene& _scene);
	
	// Serialization of the FBX model to custom binary format
	void SerializeBinaryGeom(const std::filesystem::path& _filePath);

	// Checks the file extension to ensure that it is the correct file type
	void CheckExtension(const std::filesystem::path& _filePath);

	ModelComponents* pModel = nullptr;	// Pointer to the model
};

#endif // !MODELCOMPILER_H