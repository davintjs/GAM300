#include <filesystem>
#include "Utilities/GUID.h"
#include "Utilities/TemplatePack.h"
#include <glm/glm.hpp>
#include <list>

#include <Properties.h>

#pragma once

//static std::unordered_map<std::filesystem::path, Engine::GUID> DEFAULT_ASSETS
//{
//	{"None.geom", Engine::GUID(0)},
//	{"Cube.geom", Engine::GUID(1)},
//	{"Sphere.geom", Engine::GUID(2)},
//	{"Capsule.geom", Engine::GUID(3)},
//	{"Line.geom", Engine::GUID(4)},
//	{"Plane.geom", Engine::GUID(5)},
//	{"Segment3D.geom", Engine::GUID(6)},
//	{"None.dds", Engine::GUID(7)},
//	{"None.mat", Engine::GUID(8)},
//	{"None.anim", Engine::GUID(9)},
//};

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



// GUID, last file update time, file name, data
struct FileInfo
{
	FileInfo() {};
	FileInfo(std::filesystem::path _mFilePath) : mFilePath{ _mFilePath } {}
	std::filesystem::path mFilePath;
};

using FileData = std::vector<char>;

struct Asset : FileInfo
{
	FileData mData;
	using Meta = MetaFile;
};


struct MetaFile : property::base
{
	Engine::GUID<Asset> guid;
	property_vtable()
};

property_begin_name(MetaFile, ""){
	property_var(guid),
} property_vend_h(MetaFile);

struct TextureAsset : Asset
{
};

struct ScriptAsset : Asset
{
};

struct AudioAsset : Asset
{
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

	using Meta = MetaFile;
};

struct ShaderAsset : Asset
{
	// Map of variable name to type enum
	//std::unordered_map<std::string, size_t> variables;

	std::string vertexShaderBuffer;
	std::string fragmentShaderBuffer;

};


struct MaterialAsset : Asset
{

};


// Asset that contains reference to all components of the model(Mesh, Material, Texture, Animations)
// User can add this asset into the scene and it will assign the materials onto the mesh etc
struct ModelAsset : Asset
{
	std::vector<Engine::GUID<MeshAsset>> meshes;
	std::vector<Engine::GUID<MaterialAsset>> materials;
	std::vector<Engine::GUID<AnimationAsset>> animations;
};

struct AnimationAsset : Asset
{

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

//Takes in a guid, does a huge switch case, returns Asset*
template <typename T, typename... Ts>
struct GetAssetHelper
{
	template <typename T, typename... Ts>
	GetAssetHelper()
	{

	}
};
