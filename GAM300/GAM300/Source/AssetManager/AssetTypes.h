#include <filesystem>
#include "Utilities/GUID.h"
#include "Utilities/TemplatePack.h"
#include <glm/glm.hpp>
#include <list>

#pragma once

#ifndef ASSET_TYPES_H
#define ASSET_TYPES_H

#define ASSET_CUBE 1
#define ASSET_SPHERE 2
#define ASSET_LINE 3
#define ASSET_SEG3D 4

#define ASSET(className) struct className : Asset

#define MAX_BONE_INFLUENCE 4
struct ModelVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 textureCords;
	glm::ivec4 color;

	// Animation Related Properties
	int boneIDs[MAX_BONE_INFLUENCE];
	float weights[MAX_BONE_INFLUENCE];

};

using FileData = std::vector<char>;

struct Asset
{
	FileData mData;
	std::filesystem::path mFilePath;
};

ASSET(TextureAsset){};


ASSET(ScriptAsset) {};

ASSET(AudioAsset) {};

ASSET(MeshAsset)
{
	std::vector<ModelVertex> vertices;	// This individual mesh vertices
	std::vector<glm::mat4> bindPoses;	// The bind pose at each index refers to the bone with the same index
	std::vector<unsigned int> indices;	// This individual mesh indices

	glm::vec3 boundsMin{};				// The min position of the mesh
	glm::vec3 boundsMax{};				// The max position of the mesh

	unsigned int numVertices;
	unsigned int numIndices;
	unsigned int numBones;
	unsigned int numBindPoses;
	unsigned int materialIndex;
};

ASSET(ShaderAsset)
{
	// Map of variable name to type enum
	//std::unordered_map<std::string, size_t> variables;

	std::string vertexShaderBuffer;
	std::string fragmentShaderBuffer;

};

ASSET(MaterialAsset){ };

ASSET(AnimationAsset){};

// Asset that contains reference to all components of the model(Mesh, Material, Texture, Animations)
// User can add this asset into the scene and it will assign the materials onto the mesh etc
ASSET(ModelAsset)
{
	std::vector<Engine::GUID<MeshAsset>> meshes;
	std::vector<Engine::GUID<MaterialAsset>> materials;
	std::vector<Engine::GUID<AnimationAsset>> animations;
};

using AssetTypes = TemplatePack<ModelAsset, MeshAsset, TextureAsset, ScriptAsset, AudioAsset, ShaderAsset, MaterialAsset, AnimationAsset,Asset>;
using GetAssetType = decltype(GetTypeGroup(AssetTypes()));

template <typename AssetType>
using AssetsTable = std::unordered_map < Engine::GUID<AssetType>, AssetType>;
//File extension : Asset Type
static std::unordered_map<std::filesystem::path, size_t> AssetExtensionTypes =
{
	{".cs",		GetAssetType::E<ScriptAsset>()},
	{".dds",	GetAssetType::E<TextureAsset>()},
	{".geom",	GetAssetType::E<MeshAsset>()},
	{".model",	GetAssetType::E<ModelAsset>()},
	{".mp3",	GetAssetType::E<AudioAsset>()},
	{".wav",	GetAssetType::E<AudioAsset>()},
	{".material", GetAssetType::E<MaterialAsset>()},
	{".shader", GetAssetType::E<ShaderAsset>()},
};

#endif