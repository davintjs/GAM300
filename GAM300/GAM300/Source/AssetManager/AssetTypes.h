#include <filesystem>
#include "Utilities/GUID.h"
#include "Utilities/TemplatePack.h"
#include "glm/vec3.hpp"
#include <list>
#include <yaml-cpp/yaml.h>

#include <Properties.h>

#pragma once
// GUID, last file update time, file name, data
struct FileInfo
{
	FileInfo() {};
	FileInfo(std::filesystem::path _mFilePath) : mFilePath{_mFilePath} {}
	std::filesystem::path mFilePath;
	Engine::GUID guid;
};

struct Asset : FileInfo
{
	std::vector<char> mData;
};

//property_begin_name(FileInfo, "File")
//{
//	property_var(guid),
//	property_var(mFilePath).Name("File Path"),
//} property_vend_h(FileInfo)

struct TextureAsset : Asset
{

};

struct ScriptAsset : Asset
{

};

struct AudioAsset : Asset
{

};

struct MetaAsset : Asset
{

};

struct Folder : FileInfo
{
	std::list<FileInfo*> folderMembers;
};


struct MeshAsset
{
	std::vector<glm::vec3> mVertices;
	std::vector<unsigned int> mIndices;
};

//Hash table that maps an ID to a pointer
template <typename T>
using AssetsTable = std::unordered_map<Engine::GUID,T>;

using AssetTypes = TemplatePack<MeshAsset, TextureAsset, Folder, ScriptAsset, MetaAsset, AudioAsset>;
using GetAssetType = decltype(GetTypeGroup(AssetTypes()));
//File extension : Asset Type
static std::unordered_map<std::string, size_t> AssetExtensionTypes =
{
	{".cs",		GetAssetType::E<ScriptAsset>()},
	{".dds",	GetAssetType::E<TextureAsset>()},
	{".geom",	GetAssetType::E<MeshAsset>()},
	{".meta",	GetAssetType::E<MetaAsset>()},
	{".mp3",	GetAssetType::E<AudioAsset>()},
	{".wav",	GetAssetType::E<AudioAsset>()},
	{"",		GetAssetType::E<Folder>()},
};