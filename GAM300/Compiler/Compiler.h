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
#include <unordered_map>

#include "Mesh.h"

#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Meshoptimizer/meshoptimizer.h"

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
	void ProcessBones(const aiNode& node, const aiScene& scene);
	void ProcessGeom(const aiNode& node, const aiScene& scene);
	Mesh ProcessMesh(const aiMesh& mesh, const aiScene& scene);
	void Optimize();
	void TransformVertices();
	void ImportMaterialAndTextures(const aiMaterial& material);

	void SerializeBinaryGeom(const std::string filepath);
	void DeserializeDescriptor(const std::string filepath);

public:

	Descriptor* _descriptor{ nullptr };

	std::vector<Mesh> _meshes{}; // Individual meshes in the model, which also contains it's individual vertices and indices

	// I think this bottom part we should eventually phase out, and save the individual meshes
	// vertices and indices instead of whole chunk at one go
	std::vector<Vertex> _vertices{}; // Total vertices of the WHOLE model
	std::vector<int32_t> _indices{}; // Total indices of the WHOLE model

	//std::vector<Texture> _textures{}; // Total textures of the WHOLE model
	std::vector<Material> _materials{}; // Total materials of the WHOLE model
};

#endif // !MODEL_H