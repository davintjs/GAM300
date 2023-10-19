#include <filesystem>
#include "Utilities/GUID.h"
#include "Utilities/TemplatePack.h"
#include "glm/vec3.hpp"
#include <list>

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
using AssetsList = std::list<T>;

using AssetTypes = TemplatePack<MeshAsset, TextureAsset, Folder, ScriptAsset, MetaAsset>;
using GetAssetType = decltype(GetTypeGroup(AssetTypes()));
//File extension : Asset Type
static std::unordered_map<std::string, size_t> AssetExtensionTypes =
{
	{".cs",		GetAssetType::E<ScriptAsset>()},
	{".dds",	GetAssetType::E<TextureAsset>()},
	{".fbx",	GetAssetType::E<MeshAsset>()},
	{".meta",	GetAssetType::E<MetaAsset>()},
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
		return std::get<AssetsList<T>>(assets);
	}

	void AddAsset(const std::filesystem::path& filePath)
	{
		size_t assetType = AssetExtensionTypes[filePath.extension().string()];
		if (([&](auto type) 
		{
			using T = decltype(type);
			if (GetAssetType::E<T>() == assetType)
			{
				std::get<AssetsList<T>>(assets).emplace_back();
				return true;
			}
			return false;
		}
		(Ts{}) || ...))
		{
			return;
		}
	}
private:
	std::tuple<AssetsTable<Ts>...> assets;
};

using AllAssets = decltype(AllAssetsGroup(AssetTypes()));