#pragma once

/*!************************************************************************
\file               Model.h
\author             Lian Khai Kiat
\par DP email:      l.kiat\@digipen.edu
\par Course:        CSD2150
\date               07/04/2023
\brief
Definition of geom class of geom compliler
**************************************************************************/
#ifndef MODEL_H
#define MODEL_H

#include "precompiled.h"
#include "glm/glm.hpp"

#include "Mesh.h"
//#include "../External/assimp/include/assimp/types.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


//#include "../srcviewer/Mesh.h"
//#include "meshoptimizer.h"
//
//
//#include "assimp/Importer.hpp"
//#include "assimp/scene.h"
//#include "assimp/postprocess.h"

//struct Texture
//{
//	std::string filepath;
//	//xgpu::texture instance;
//};
//
//struct SampleHolder
//{
//	int binding;
//	std::string type;
//};
//
//struct Material
//{
//	std::size_t GUID;
//	std::string matName;
//	std::vector<SampleHolder> _samples;
//};


struct Descriptor
{
	glm::vec3 scale = { 1.f, 1.f, 1.f };
	glm::vec3 rotate = { 0.f, 0.f, 0.f };
	glm::vec3 translate = { 0.f, 0.f, 0.f };
	std::string filePath; // Path to intermediate file
};

class AssimpLoader
{
public:

	AssimpLoader() {};
	AssimpLoader(const std::string descriptorFilePath, const std::string geomFilePath);
	~AssimpLoader();

	void LoadModel();
	void ProcessGeom(const aiNode& node, const aiScene& scene);
	Mesh ProcessMesh(const aiMesh& mesh, const aiScene& scene);
	//void Optimize();
	void TransformVertices();
	void ImportMaterialAndTextures(const aiMaterial& material, const aiScene& scene);

	void SerializeDescriptor(const std::string filepath);
	void SerializeBinaryGeom(const std::string filepath);
	void DeserializeDescriptor(const std::string filepath);

public:

	Descriptor* _descriptor{ nullptr };

	std::vector<Mesh> _meshes{};
	std::vector<Vertex> _vertices{};
	std::vector<int32_t> _indices{};
	std::vector<Texture> _textures{};
	std::vector<Material> _materials{};/**/

};

#endif // !MODEL_H