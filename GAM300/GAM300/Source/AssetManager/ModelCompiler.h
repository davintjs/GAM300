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

#include "../../Compiler/BoundingBox.h"

#include "assimp/scene.h"
#include "assimp/mesh.h"

#include "Core/SystemInterface.h"
#include "AssetManager/ModelClassAndStruct.h"

// Bean: Temporary
#include "Graphics/AnimationManager.h"

#define MODELCOMPILER ModelCompiler::Instance()

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

	// Import materials and textures of the FBX model
	void ImportMaterialAndTextures(const aiMaterial& _material);

	// Extract bone setting from the vertices in the model for animation
	void ExtractBoneWeightForVertices(std::vector<ModelVertex>& _vert, const aiMesh& _mesh, const aiScene& _scene);
	
	// Serialization of the FBX model to custom binary format
	void SerializeBinaryGeom(const std::filesystem::path& _filePath);

	void SerializeBinaryAnim(const std::filesystem::path& _filePath);

	// Checks the file extension to ensure that it is the correct file type
	void CheckExtension(const std::filesystem::path& _filePath);

	GeomComponents* pModel = nullptr;	// Pointer to the model
};

#endif // !MODELCOMPILER_H