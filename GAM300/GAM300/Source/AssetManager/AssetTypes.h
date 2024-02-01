#pragma once
#include <filesystem>
#include "Utilities/GUID.h"
#include "Utilities/TemplatePack.h"
#include <glm/glm.hpp>
#include <list>
#include "ModelClassAndStruct.h"

#ifndef ASSET_TYPES_H
#define ASSET_TYPES_H

#define ASSET_CUBE 1
#define ASSET_SPHERE 2
#define ASSET_LINE 3
#define ASSET_SEG3D 4

#define ADD_SUB_ASSET(asset,guid) AddSubAssetEvent sbae{asset,guid}; EVENTS.Publish(&sbae)

struct DefaultImporter;

struct Asset
{
	std::filesystem::path mFilePath;
	DefaultImporter* importer;
};

struct TextureAsset : Asset{};


struct ScriptAsset : Asset {};

struct AudioAsset : Asset {};

struct AnimationAsset : Asset
{
	std::vector<Bone> bones; // Bean: We need to copy this for animator 
	std::map<std::string, BoneInfo> boneInfoMap; // Bean: We need to copy this for animator
	std::vector<AnimationState> animationStates;  // Maps the name of the animation with the range using vec2 where x rep start, y rep end
	
	AssimpNodeData rootNode;
	
	int ticksPerSecond;
	int boneCounter;
	float duration;
};

struct MeshAsset : Asset
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

struct ShaderAsset : Asset
{
	// Map of variable name to type enum
	//std::unordered_map<std::string, size_t> variables;

	std::string vertexShaderBuffer;
	std::string fragmentShaderBuffer;

};

struct MaterialAsset : Asset { };

// Asset that contains reference to all components of the model(Mesh, Material, Texture, Animations)
// User can add this asset into the scene and it will assign the materials onto the mesh etc
struct ModelAsset : Asset
{
};

struct FontAsset : Asset { };


using AssetTypes = TemplatePack<ModelAsset, MeshAsset, TextureAsset, ScriptAsset, AudioAsset, ShaderAsset, MaterialAsset, AnimationAsset, FontAsset, Asset>;
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
	{".anim",	GetAssetType::E<AnimationAsset>()},
	{".material", GetAssetType::E<MaterialAsset>()},
	{".shader", GetAssetType::E<ShaderAsset>()},
	{".font", GetAssetType::E<FontAsset>()},
};

#endif