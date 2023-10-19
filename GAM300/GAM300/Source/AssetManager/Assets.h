#include <filesystem>
#include "Utilities/GUID.h"
#include "Utilities/TemplatePack.h"
#include "glm/vec3.hpp"
#include <list>
#include <yaml-cpp/yaml.h>

#pragma once
// GUID, last file update time, file name, data
struct FileInfo
{
	FileInfo() {};
	FileInfo(std::filesystem::path _mFilePath) : mFileName(_mFilePath.filename().stem().string()) {}
	std::filesystem::path mFilePath;
	Engine::GUID guid;
	std::string mFileName;
	std::string FileName() const
	{
		return mFilePath.filename().string();
	}
};

struct TextureAsset
{

};

struct ScriptAsset
{

};

struct MetaAsset
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

struct Asset : FileInfo
{
	std::vector<char> mData;
};

//Hash table that maps an ID to a pointer
template <typename T>
using AssetsTable = std::unordered_map<Engine::GUID,T>;

using AssetTypes = TemplatePack<MeshAsset, TextureAsset, Folder, ScriptAsset, MetaAsset>;
using GetAssetType = decltype(GetTypeGroup(AssetTypes()));
//File extension : Asset Type
static std::unordered_map<std::string, size_t> AssetExtensionTypes =
{
	{".cs",		GetAssetType::E<ScriptAsset>()},
	{".dds",	GetAssetType::E<TextureAsset>()},
	//{".fbx",	GetAssetType::E<MeshAsset>()},
	//{".meta",	GetAssetType::E<MetaAsset>()},
	{"",		GetAssetType::E<Folder>()},
};

//Hash table of single component types
template<typename... Ts>
struct AllAssetsGroup
{
	constexpr AllAssetsGroup(TemplatePack<Ts...>) {}
	AllAssetsGroup() = default;
	template<typename T>
	auto& GetAssets()
	{
		return std::get<AssetsTable<T>>(assets);
	}

	void AddAsset(const std::filesystem::path& filePath)
	{
		size_t assetType = AssetExtensionTypes[filePath.extension().string()];
		if (([&](auto type) 
		{
			using T = decltype(type);
			if (GetAssetType::E<T>() == assetType)
			{
				Engine::GUID guid = CreateMeta(filePath);
				std::get<AssetsTable<T>>(assets)[guid] = {};
				return true;
			}
			return false;
		}
		(Ts{}) || ...))
		{
			return;
		}
	}

	Engine::GUID CreateMeta(const std::filesystem::path& filePath)
	{
		std::filesystem::path metaPath = filePath;
		metaPath += ".meta";
		Engine::GUID guid;
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "guid" << YAML::Value << guid.ToHexString();
		std::ofstream ofs(metaPath);
		ofs << out.c_str();
		ofs.close();
		return guid;
	}
private:
	std::tuple<AssetsTable<Ts>...> assets;
};

using AllAssets = decltype(AllAssetsGroup(AssetTypes()));