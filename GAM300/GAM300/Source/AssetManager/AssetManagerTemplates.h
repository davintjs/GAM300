#pragma once

#include "AssetTypes.h"
#include "Core/EventsManager.h"
#include <vector>
#include <unordered_set>
#include <Utilities/YAMLUtils.h>
#include <Utilities/Serializer.h>

#include "ModelDecompiler.h"
#include "Graphics/MeshManager.h"


#include "ImporterTypes.h"

namespace chron = std::chrono;




template <typename T>
static auto& GetImporterTable()
{
	static ImporterTable<T> table;
	return table;
}

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

	template <typename T>
	T& ProcessAsset(const std::filesystem::path& filePath)
	{
		auto& importer = GetImporter<T>(filePath);
		fs::path metaPath{ filePath };
		metaPath += ".meta";
		auto& table{ std::get<AssetsTable<T>>(assets) };
		T& asset{ table[importer.guid] };
		asset.mFilePath = filePath;
		auto& buffer = std::get<AssetsBuffer<T>>(assetsBuffer);
		if constexpr (std::is_same<T, ModelAsset>())
		{
			MODELDECOMPILER.DeserializeModel(filePath.string(), importer);
		}
		buffer.emplace_back(std::make_pair(ASSET_LOADED, &asset));
		return asset;
	}

	template <typename AssetType>
	void AddSubAsset(AssetType& asset, AssetImporter<AssetType>& importer)
	{
		Engine::GUID<AssetType> guid = importer.guid;
		GetImporterTable<AssetType>()[asset.mFilePath] = importer;
		auto& table{ std::get<AssetsTable<AssetType>>(assets)};
		
		table[guid] = asset;
		auto& buffer = std::get<AssetsBuffer<AssetType>>(assetsBuffer);
		buffer.emplace_back(std::make_pair(ASSET_LOADED, &table[guid]));
	}

	void AddAsset(const std::filesystem::path& filePath)
	{
		size_t assetType = GetAssetType(filePath);
		if (fs::is_directory(filePath))
			return;
		if (([&](auto type)
		{
			using T = decltype(type);
			//Match file extensions to file type
			if (GetAssetType::E<T>() == assetType)
			{
				T& asset = ProcessAsset<T>(filePath);
				return true;
			}
			return false;
		}
		(Ts{}) || ...))
		{
			return;
		}
	}

	bool RemoveAsset(const std::filesystem::path& filePath)
	{
		size_t assetType = GetAssetType(filePath);
		if (([&](auto type)
			{

				using T = decltype(type);
				if (GetAssetType::E<T>() == assetType)
				{
					Engine::GUID<T> guid = GetImporter<T>(filePath).guid;
					T& asset{ std::get<AssetsTable<T>>(assets)[guid] };
					T* tempAsset = new T;
					tempAsset->mFilePath = filePath;
					std::get<AssetsTable<T>>(assets).erase(guid);
					std::get<AssetsBuffer<T>>(assetsBuffer).emplace_back(std::make_pair(ASSET_UNLOADED, tempAsset));
					return true;
				}
				return false;
			}
			(Ts{}) || ...))
		{
			return true;
		}
		return false;
	}

	void UpdateAsset(const std::filesystem::path& filePath)
	{
		size_t assetType = GetAssetType(filePath);
		if (([&](auto type)
			{
				using T = decltype(type);
				if (GetAssetType::E<T>() == assetType)
				{
					Engine::GUID<T> guid = GetImporter<T>(filePath).guid;
					if constexpr (std::is_base_of<Asset, T>())
					{
						T& asset{ std::get<AssetsTable<T>>(assets)[guid] };
						asset.mFilePath = filePath;
						std::get<AssetsBuffer<T>>(assetsBuffer).emplace_back(std::make_pair(ASSET_UPDATED, &asset));
					}
					else
					{
						std::get<AssetsTable<T>>(assets)[guid] = {};
					}
					return true;
				}
				return false;
			}
			(Ts{}) || ...))
		{
			return;
		}
	}

	void RenameAsset(const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
	{
		size_t oldExtension{ GetAssetType(oldPath) };
		size_t newExtension{ GetAssetType(newPath) };

		fs::path oldMeta{ oldPath };
		oldMeta += ".meta";
		fs::path newMeta{ newPath };
		newMeta += ".meta";
		fs::rename(oldMeta, newMeta);
		//Deal with meta file conversion here
		if (oldExtension != newExtension)
		{
			//Look for oldExtension
			if (([&](auto type)
			{
				using T = decltype(type);
				if (GetAssetType::E<T>() == oldExtension)
				{
					Engine::GUID<T> guid = GetImporter<T>(newPath).guid;
					return true;
				}
				return false;
			}
			(Ts{}) || ...));
		}
		else
		{
			//Look for oldExtension
			if (([&](auto type)
			{
				using T = decltype(type);
				if (GetAssetType::E<T>() == oldExtension)
				{
					Engine::GUID<T> guid = GetImporter<T>(newPath).guid;
					auto& table{ std::get<AssetsTable<T>>(assets) };
					table[guid].mFilePath = newPath;
					return true;
				}
				return false;
			}
			(Ts{}) || ...));
		}

		//AddAsset(newPath,fileData);
		//RemoveAsset(oldPath);
	}

	void ProcessBuffer()
	{
		(([&](auto type) 
		{
			using T = decltype(type);
			if (std::is_same_v<ScriptAsset,T>)
				return false;
			auto& buffer{std::get<AssetsBuffer<T>>(assetsBuffer)};
			for (auto& pair : buffer)
			{
				fs::path path{pair.second->mFilePath };
				auto& importer = GetImporter<T>(path);
				switch (pair.first)
				{
					case ASSET_LOADED:
					{
						AssetLoadedEvent<T> e{ path,importer.guid,*pair.second };
						EVENTS.Publish(&e);
						break;
					}
					case ASSET_UPDATED:
					{
						AssetUpdatedEvent<T> e{ path,importer.guid,*pair.second };
						EVENTS.Publish(&e);
						break;
					}
					case ASSET_UNLOADED:
					{
						
						AssetUnloadedEvent<T> e{ path,importer.guid};
						EVENTS.Publish(&e);
						fs::path metaPath = path;
						metaPath += ".meta";
						//Actual file does not exist
						if (std::filesystem::exists(metaPath))
						{
							std::filesystem::remove(metaPath);
							delete pair.second;
						}
						break;
					}
					default:
					{
						E_ASSERT(false, "Invalid asset state");
						break;
					}
				}
			}
			buffer.clear();
		})(Ts{}), ...);
	}

	template <typename AssetType>
	auto& GetImporter(const std::filesystem::path& filePath, bool update = false)
	{
		//ImporterTable<AssetType>& importerTable{ GetImporterTable<AssetType>() };
		//if (!importerTable.contains(filePath))
		size_t assetType = GetAssetType(filePath);
		std::filesystem::path metaPath = filePath;
		metaPath += ".meta";
		auto& importerTable = GetImporterTable<AssetType>();
		auto pairIt = importerTable.find(filePath);
		//Not loaded into memory
		if (pairIt == importerTable.end())
		{
			auto& importer = importerTable[filePath];
			//Attempt to load into memory
			bool success = Deserialize(metaPath, importer);
			if (!success)
				Serialize(metaPath, importer);
			return importer;
		}
		return pairIt->second;
	}

	size_t GetAssetType(const std::filesystem::path& path)
	{
		if (AssetExtensionTypes.contains(path.extension().string()))
		{
			return AssetExtensionTypes[path.extension().string()];
		}
		return GetAssetType::E<Asset>();
	}

	template <typename AssetType>
	fs::path GetFilePath(const Engine::GUID<AssetType>& guid)
	{
		auto& table = std::get<AssetsTable<AssetType>>(assets);
		if (table.find(guid) != table.end())
		{
			return table[guid].mFilePath;
		}
		return "";
	}

	//Compare to meta file to see if file was modified while engine was closed
	bool IsModified(const std::filesystem::path& filePath)
	{
		std::filesystem::path metaPath = filePath;
		metaPath += ".meta";
		//If no meta, assume modified as it is just added
		if (!std::filesystem::exists(metaPath))
		{
			//GetGUID(filePath,true);
			return true;
		}
		if (fs::last_write_time(filePath) > fs::last_write_time(metaPath))
		{
			//GetGUID(filePath, true);
			return true;
		}
		return false;
	}

private:
	std::tuple<AssetsTable<Ts>...> assets;
	std::tuple<AssetsBuffer<Ts>...> assetsBuffer;
};

using AllAssets = decltype(AllAssetsGroup(AssetTypes()));