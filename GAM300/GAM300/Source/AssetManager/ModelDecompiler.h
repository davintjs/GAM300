/*!***************************************************************************************
\file			ModelDecompiler.h
\project		
\author         Davin Tan
\co-author      Sean Ngo

\par			Course: GAM300
\date           31/10/2023

\brief
    This file contains the declarations of the following:
    1. Loads model files and extract the meshes, material, textures and animations

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef MODELDECOMPILER_H
#define MODELDECOMPILER_H

#include "AssetManager/ModelClassAndStruct.h"
#include "AssetTypes.h"
#include "ImporterTypes.h"

#define MODELDECOMPILER ModelDecompiler::Instance()

SINGLETON(ModelDecompiler)
{
public:
	// To load Geoms from FBXs
	void DeserializeModel(const std::string& _filePath, ModelImporter& importer);
	
	// Load meshes from the FBX file
	void DeserializeMeshes(std::ifstream& ifs, const std::string & _filePath, ModelImporter & importer);

	// Load materials from the FBX file
	void DeserializeMaterials(std::ifstream& ifs, const std::string & _filePath, ModelImporter & importer);

	// Load textures from the FBX file
	void DeserializeTextures(std::ifstream& ifs, const std::string & _filePath, ModelImporter & importer);

	// Load animations from the FBX file
	void DeserializeAnimations(std::ifstream& ifs, const std::string & _filePath, ModelImporter & importerx);

	// Load animation nodes recursively to retrieve bone data
	void DeserializeRecursiveNode(std::ifstream& ifs, const std::string & _filePath, AssimpNodeData& _node);

	// Decompress vertices
	void DecompressVertices(std::vector<ModelVertex>& _meshVertices,
		const std::vector<Vertex>& _oVertices,
		const std::vector<VertexBoneInfo>& _boneInfo,
		const glm::vec3& _posCompressScale,
		const glm::vec2& _texCompressScale,
		const glm::vec3& _posOffset,
		const glm::vec2& _texOffset);

	//Decompress indices
	void DecompressIndices(std::vector<unsigned int>& _meshIndices, const std::vector<std::uint16_t>& _oIndices);
private:

};

#endif // !MODELDECOMPILER_H