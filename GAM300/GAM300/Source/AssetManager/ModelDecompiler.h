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

#define MODELDECOMPILER ModelDecompiler::Instance()

SINGLETON(ModelDecompiler)
{
public:
	// To load Geoms from FBXs
	ModelComponents DeserializeModel(const std::string& _filePath, const Engine::GUID<ModelAsset>& _guid);
	
	// Load meshes from the FBX file
	void DeserializeMeshes(std::ifstream& ifs, ModelComponents& _model);

	// Load materials from the FBX file
	void DeserializeMaterials(std::ifstream& ifs, ModelComponents& _model);

	// Load textures from the FBX file
	void DeserializeTextures(std::ifstream& ifs, ModelComponents& _model);

	// Load animations from the FBX file
	void DeserializeAnimations(std::ifstream& ifs, ModelComponents& _model);

	// Load animation nodes recursively to retrieve bone data
	void DeserializeRecursiveNode(std::ifstream& ifs, ModelComponents& _model, AssimpNodeData& _node);

	// Decompress
	void DecompressVertices(std::vector<ModelVertex>& _meshVertices,
		const std::vector<Vertex>& _oVertices,
		const glm::vec3& _posCompressScale,
		const glm::vec2& _texCompressScale,
		const glm::vec3& _posOffset,
		const glm::vec2& _texOffset);
private:

};

#endif // !MODELDECOMPILER_H