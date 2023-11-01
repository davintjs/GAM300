/*!***************************************************************************************
\file			GeomDecompiler.h
\project		
\author         Davin Tan
\co-author      Sean Ngo

\par			Course: GAM300
\date           31/10/2023

\brief
    This file contains the declarations of the following:
    1. Loads Geoms files and extract the meshes, material, textures and animations

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef GEOMDECOMPILER_H
#define GEOMDECOMPILER_H

#include "AssetManager/ModelClassAndStruct.h"

#define GEOMDECOMPILER GeomDecompiler::Instance()

SINGLETON(GeomDecompiler)
{
public:
	// To load Geoms from FBXs
	ModelComponents DeserializeGeoms(const std::string& _filePath, const Engine::GUID& _guid);

	// Decompress
	void DecompressVertices(std::vector<ModelVertex>& _meshVertices,
		const std::vector<Vertex>& _oVertices,
		const glm::vec3& _posCompressScale,
		const glm::vec2& _texCompressScale,
		const glm::vec3& _posOffset,
		const glm::vec2& _texOffset);
private:

};

#endif // !GEOMDECOMPILER_H