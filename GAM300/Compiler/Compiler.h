#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>

#include "Mesh.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "glm/glm.hpp"

struct Descriptor
{
	glm::vec3 scale = { 1.f, 1.f, 1.f };
	glm::vec3 rotate = { 0.f, 0.f, 0.f };
	glm::vec3 translate = { 0.f, 0.f, 0.f };
	std::string filePath; // Path to intermediate file
	std::string meshName{}; // Mesh file name
};

class ModelLoader
{
public:

	ModelLoader() {};
	ModelLoader(const std::string descriptorFilePath, const std::string geomFilePath);
	~ModelLoader();

	void LoadModel();
	void ProcessGeom(const aiNode& node, const aiScene& scene);
	Mesh ProcessMesh(const aiMesh& mesh, const aiScene& scene);
	//void Optimize();
	void TransformVertices();
	void ImportMaterialAndTextures(const aiMaterial& material);

	void SerializeBinaryGeom(const std::string filepath);
	void DeserializeDescriptor(const std::string filepath);

public:

	Descriptor* _descriptor{ nullptr };

	std::vector<Mesh> _meshes{};
	std::vector<Vertex> _vertices{};
	std::vector<int32_t> _indices{};
	std::vector<Texture> _textures{};
	std::vector<Material> _materials{};

};

#endif // !MODEL_H