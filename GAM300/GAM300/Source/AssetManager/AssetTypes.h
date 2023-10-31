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

static std::unordered_map<std::filesystem::path, Engine::GUID> DEFAULT_ASSETS
{
	//Default Textures
	{"None.dds", Engine::GUID(0)},

	//Default Geometry 
	{"None.geom", Engine::GUID(100)},
	{"Cube.geom", Engine::GUID(101)},
	{"Sphere.geom", Engine::GUID(102)},
	{"Capsule.geom", Engine::GUID(103)},
	{"Line.geom", Engine::GUID(104)},
	{"Plane.geom", Engine::GUID(105)},
	{"Segment3D.geom", Engine::GUID(106)},
	{"Quad.geom", Engine::GUID(107)},

	//Default Materials
	{"None.mat", Engine::GUID(200)},

	//Default Animations
	{"None.anim", Engine::GUID(300)},

	//Default Scripts
	{"None.cs", Engine::GUID(400)}
};

struct MetaFile : property::base
{
	Engine::GUID guid;
	property_vtable()
};

property_begin_name(MetaFile, ""){
	property_var(guid),
} property_vend_h(MetaFile);


// GUID, last file update time, file name, data
struct FileInfo
{
	FileInfo() {};
	FileInfo(std::filesystem::path _mFilePath) : mFilePath{_mFilePath} {}
	std::filesystem::path mFilePath;
};

using FileData = std::vector<char>;

struct Asset : FileInfo
{
	FileData mData;
	using Meta = MetaFile;
};

struct TextureImporter : MetaFile
{
	size_t maxTextureSize;
	property_vtable()
};

property_begin_name(TextureImporter,""){
	property_parent(MetaFile),
	property_var(maxTextureSize)
} property_vend_h(TextureImporter);


struct TextureAsset : Asset
{
	using Meta = MetaFile;
};

struct ScriptAsset : Asset
{
	using Meta = MetaFile;
};

struct AudioAsset : Asset
{
	using Meta = MetaFile;
};

struct FolderMeta : MetaFile
{
	bool folderAsset{ true };
	property_vtable()
};

property_begin_name(FolderMeta, ""){
	property_parent(MetaFile),
	property_var(folderAsset),
} property_vend_h(FolderMeta);

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

struct ModelImporter : MetaFile
{
	// GUID to index of asset in memory (Separate in the future for materials, animations, and textures)
	// For now its only for meshes
	std::vector<Engine::GUID> meshes; 
	std::vector<Engine::GUID> materials; 
	std::vector<Engine::GUID> animations; 
	property_vtable()
};

property_begin_name(ModelImporter, "ModelImporter") {
	property_parent(MetaFile),
	property_var(meshes),
	property_var(materials),
	property_var(animations),
} property_vend_h(ModelImporter);

// Asset that contains reference to all components of the model(Mesh, Material, Texture, Animations)
// User can add this asset into the scene and it will assign the materials onto the mesh etc
struct ModelAsset : Asset
{
	using Meta = ModelImporter;
};

using AssetTypes = TemplatePack<ModelAsset, MeshAsset, TextureAsset, ScriptAsset, AudioAsset, Asset>;
using GetAssetType = decltype(GetTypeGroup(AssetTypes()));
//File extension : Asset Type
static std::unordered_map<std::filesystem::path, size_t> AssetExtensionTypes =
{
	{".cs",		GetAssetType::E<ScriptAsset>()},
	{".dds",	GetAssetType::E<TextureAsset>()},
	{".geom",	GetAssetType::E<MeshAsset>()},
	{".model",	GetAssetType::E<ModelAsset>()},
	{".mp3",	GetAssetType::E<AudioAsset>()},
	{".wav",	GetAssetType::E<AudioAsset>()},
};


//Hash table that maps an ID to a pointer
template <typename T>
using AssetsTable = std::unordered_map<Engine::GUID, T>;