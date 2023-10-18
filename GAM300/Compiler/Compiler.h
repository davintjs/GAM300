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
#include <glm/gtc/quaternion.hpp>

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

	//bone indexes which will influence this vertex
	int m_BoneIDs[4];
	//weights from each bone
	float m_Weights[4];
};

struct BoneInfo
{
	int id; /*id is index in finalBoneMatrices*/
	glm::mat4 offset; /*offset matrix transforms vertex from model space to bone space*/
};

class AssimpGLMHelpers
{

public:
	static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}
};

struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation
{
	glm::quat orientation;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

class Bone
{
public:
	Bone(const std::string& name, int ID, const aiNodeAnim* channel);


private:

	std::vector<KeyPosition> m_Positions;
	std::vector<KeyRotation> m_Rotations;
	std::vector<KeyScale> m_Scales;
	int m_NumPositions;
	int m_NumRotations;
	int m_NumScalings;

	glm::mat4 m_LocalTransform;
	std::string m_Name;
	int m_ID;
};

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class AnimationData
{
public:
	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
	void ReadMissingBones(const aiAnimation* animation, AnimationData& tempdata);
	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);
};

class ModelLoader
{
public:

	ModelLoader() {};
	ModelLoader(const std::string descriptorFilePath, const std::string geomFilePath);
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
	void SerializeBinaryGeom(const std::string filepath);

	// Deserialization of descriptor file to read the FBX model
	void DeserializeDescriptor(const std::string filepath);

private:

	Descriptor* _descriptor{ nullptr };

	std::vector<Geom_Mesh> _meshes{}; // Individual meshes in the model, which also contains its individual vertices and indices

	std::vector<Material> _materials{}; // Total materials of the WHOLE model (One mesh uses one material only)
	
	std::vector<AnimationData> Animation;

	std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;

	std::unordered_map <std::string, std::unordered_map<std::string, BoneInfo>> AllBoneInfoMaps;
	std::unordered_map <std::string, int> AllBoneCount;

	void SetVertexBoneDataToDefault(TempVertex& vertex);
	void SetVertexBoneData(TempVertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<TempVertex>& vertices, const aiMesh& mesh, const aiScene& scene);
};

#endif // !MODEL_H