#include <filesystem>
#include "Utilities/GUID.h"
#include "Utilities/TemplatePack.h"
#include "glm/vec3.hpp"
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

	//Default Shaders
	{"Test.shader", Engine::GUID(400)},
	
	//Default Scripts
	{"None.cs", Engine::GUID(1000)}
};

struct MetaFile : property::base
{
	Engine::GUID guid;
	property_vtable()
};

property_begin_name(MetaFile, "")
{
	property_var(guid),
} property_vend_h(MetaFile)


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

property_begin_name(TextureImporter,"")
{
	property_parent(MetaFile),
	property_var(maxTextureSize),
} property_vend_h(TextureImporter)


struct TextureAsset : Asset
{
};

struct ScriptAsset : Asset
{

};

struct AudioAsset : Asset
{

};

struct FolderMeta : MetaFile
{
	bool folderAsset{ true };
	property_vtable()
};

property_begin_name(FolderMeta, "")
{
	property_parent(MetaFile),
	property_var(folderAsset),
} property_vend_h(FolderMeta)

struct MeshAsset : Asset
{
	std::vector<glm::vec3> mVertices;
	std::vector<unsigned int> mIndices;
};

struct ShaderAsset : Asset
{
	// Map of variable name to type enum
	//std::unordered_map<std::string, size_t> variables;

	std::string vertexShaderBuffer;
	std::string fragmentShaderBuffer;

};

using AssetTypes = TemplatePack<MeshAsset, TextureAsset, ScriptAsset, AudioAsset, ShaderAsset, Asset>;
using GetAssetType = decltype(GetTypeGroup(AssetTypes()));

template <typename AssetType>
using AssetsTable = std::unordered_map < Engine::GUID, AssetType>;
//File extension : Asset Type
static std::unordered_map<std::filesystem::path, size_t> AssetExtensionTypes =
{
	{".cs",		GetAssetType::E<ScriptAsset>()},
	{".dds",	GetAssetType::E<TextureAsset>()},
	{".geom",	GetAssetType::E<MeshAsset>()},
	{".mp3",	GetAssetType::E<AudioAsset>()},
	{".wav",	GetAssetType::E<AudioAsset>()},
	{".shader", GetAssetType::E<ShaderAsset>()},
};