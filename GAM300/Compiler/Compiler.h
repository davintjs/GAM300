#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>
#include <unordered_map>

#include "Mesh.h"
#include "BoundingBox.h"

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

struct TempVertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 tex;
	glm::ivec4 color;
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
	void Optimize(std::vector<TempVertex>& vert, std::vector<unsigned int>& ind);
	void CompressVertices(std::vector<Vertex>& CompressVertices,
								const std::vector<TempVertex> tempVertex,
								std::pair<glm::vec3, glm::vec2>& mOffsets);
	void TransformVertices(std::vector<TempVertex> vert);
	void ImportMaterialAndTextures(const aiMaterial& material);

	void SerializeBinaryGeom(const std::string filepath);
	void DeserializeDescriptor(const std::string filepath);

private:

	Descriptor* _descriptor{ nullptr };

	std::vector<Mesh> _meshes{}; // Individual meshes in the model, which also contains its individual vertices and indices

	glm::vec3 mPosCompressionScale; // Scale value according to the bounding box of the vertices positions containing the whole model
	glm::vec2 mTexCompressionScale; // Scale value according to the bounding box of the texture coordinates containing the whole model

	// I think this bottom part we should eventually phase out, and save the individual meshes
	// vertices and indices instead of whole chunk at one go
	//std::vector<Vertex> _vertices{}; // Total vertices of the WHOLE model
	//std::vector<unsigned int> _indices{}; // Total indices of the WHOLE model

	//std::vector<Texture> _textures{}; // Total textures of the WHOLE model
	std::vector<Material> _materials{}; // Total materials of the WHOLE model (One mesh uses one material only)
};

#endif // !MODEL_H