/*!***************************************************************************************
\file			Compiler.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations of the following:
	1. Loading of FBX files with assimp loader
		a. Creation of descriptor file for the mesh
		b. Deserialization of FBX file
		b. Reading and storing of meshes
		c. Compression of vertices
	2. Serialization of meshes into custom binary format

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

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
	ModelLoader(const std::filesystem::path& geomFilePath);
	~ModelLoader();

	// Load the FBX file with import options to be processed
	void LoadModel();

	// Process the FBX file bones for animation
	void ProcessBones(const aiNode& node, const aiScene& scene);

	// Process the imported FBX file read from assimp library
	void ProcessGeom(const aiNode& node, const aiScene& scene);
	Geom_Mesh ProcessMesh(const aiMesh& mesh, const aiScene& scene);

	// Optimization of vertices of the FBX model
	void Optimize(std::vector<TempVertex>& vert, std::vector<unsigned int>& ind);

	// Compression of vertices to reduce custom binary custom file size
	void CompressVertices(std::vector<Vertex>& CompressVertices,
							const std::vector<TempVertex> tempVertex,
							std::pair<glm::vec3, glm::vec2>& mOffsets,
							std::pair<glm::vec3, glm::vec2>& mScales);

	// Applies the transformations stated in the descriptor file onto the FBX model
	void TransformVertices(std::vector<TempVertex> vert);

	// Import materials and textures of the FBX model
	void ImportMaterialAndTextures(const aiMaterial& material);

	// Serialization of the FBX model to custom binary format
	void SerializeBinaryGeom(const std::filesystem::path& path);

	// Deserialization of descriptor file to read the FBX model
	void DeserializeDescriptor(const std::string filepath);

private:

	Descriptor* _descriptor{ nullptr };

	std::vector<Geom_Mesh> _meshes{}; // Individual meshes in the model, which also contains its individual vertices and indices

	std::vector<Material> _materials{}; // Total materials of the WHOLE model (One mesh uses one material only)
};

#endif // !MODEL_H