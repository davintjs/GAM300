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

#include "assimp/scene.h"
#include "assimp/mesh.h"

#include "Core/SystemInterface.h"
#include "AssetManager/ModelClassAndStruct.h"

#define MODELCOMPILER ModelCompiler::Instance()

class BoundingBox3D
{
public:
	// Constructor
	BoundingBox3D()
	{
		mMin.x = FLT_MAX;
		mMin.y = FLT_MAX;
		mMin.z = FLT_MAX;

		mMax.x = -FLT_MAX;
		mMax.y = -FLT_MAX;
		mMax.z = -FLT_MAX;
	}
	BoundingBox3D(const glm::vec3& min, const glm::vec3& max) : mMin(min), mMax(max) { return; }

	glm::vec3 mMin{};    // Center point
	glm::vec3 mMax{};    // Center to corner half extents.
};

// The geom components that are extracted from the aiScene which contains the fbx/obj data
// essentially only a portion of the data extracted from the file is needed
struct GeomComponents
{
	std::vector<Geom_Mesh> meshes{};	// Individual meshes in the model, which also contains its individual vertices and indices
	std::vector<Material> materials{};	// Total materials of the WHOLE model (One mesh uses one material only)
	std::vector<Animation> animations{};// The animations contained on this model
};

SINGLETON(ModelCompiler)
{
public:

	// Load the FBX file with import options to be processed and choose whether to serialize the model
	// into a geom file
	GeomComponents LoadModel(const std::filesystem::path& _filePath, const bool& _serialize = true);

private:

	// Process the FBX file bones for animation
	void ProcessBones(const aiNode& _node, const aiScene& _scene);

	// Process the imported FBX file read from assimp library
	void ProcessNode(const aiNode& _node, const aiScene& _scene);

	// Process the mesh from the imported FBX file
	Geom_Mesh ProcessMesh(const aiMesh& _mesh, const aiScene& _scene);

	// Optimization of vertices of the FBX model
	void Optimize(std::vector<ModelVertex>& _vert, std::vector<unsigned int>& _ind);

	// Compression of vertices to reduce custom binary custom file size
	void CompressVertices(std::vector<Vertex>& _compressVertices,
		const std::vector<ModelVertex> _tempVertex,
		std::pair<glm::vec3, glm::vec2>& _mOffsets,
		std::pair<glm::vec3, glm::vec2>& _mScales);

	// Compression of indices to reduce custom binary custom file size
	void CompressIndices(std::vector<std::uint16_t>&_compressedIndices, std::vector<unsigned int>& _tempInd);

	// Import materials and textures of the FBX model
	void ImportMaterialAndTextures(const aiMaterial& _material);

	// Extract bone setting from the vertices in the model for animation
	void ExtractBoneWeightForVertices(std::vector<ModelVertex>& _vert, const aiMesh& _mesh, const aiScene& _scene);

	// Read bones in the animation
	void ReadMissingBones(const aiAnimation* _tempAnimation, Animation& _animation);

	// Read the hierarchy data for the animation
	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);
	
	// Serialization of the FBX model to custom binary format
	void SerializeBinaryGeom(const std::filesystem::path& _filePath);

	// Serialize Meshes
	void SerializeBinaryMeshes(std::ofstream& _serializeFile);

	// Serialize Materials
	void SerializeBinaryMaterials(std::ofstream& _serializeFile);

	// Serialize Animations
	void SerializeBinaryAnimations(std::ofstream& _serializeFile);

	// Recusively serialize the assimp node data
	void SerializeBinaryRecursiveNode(std::ofstream& _serializeFile, AssimpNodeData& _nodeData);

	// Checks the file extension to ensure that it is the correct file type
	void CheckExtension(const std::filesystem::path& _filePath);

	GeomComponents* pModel = nullptr;	// Pointer to the model
	bool hasAnimation = false;
};

#endif // !MODELCOMPILER_H