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

enum AssetState
{
	ASSET_LOADED,
	ASSET_UPDATED,
	ASSET_UNLOADED
};

template <typename T>
using AssetsBuffer = std::vector<std::pair<AssetState,T*>>;

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

	void AddAsset(const std::filesystem::path& filePath, FileData* pData = nullptr)
	{
		size_t assetType = GetAssetType(filePath);
		if (([&](auto type)
			{
				using T = decltype(type);

				if (GetAssetType::E<T>() == assetType)
				{
					//if constexpr (std::is_same<T, ModelAsset>())
					//{
					//	// Check if there is an existing meta file, read main geom file
					//	fs::path oldMeta{ filePath };
					//	oldMeta += ".meta";
					//	bool success = Deserialize<ModelImporter>(oldMeta, metaFile);
					//	if (!success)
					//		Serialize(oldMeta, metaFile);


					//	//std::filesystem::path path = filePath;
					//	//path.replace_extension(".fbx");
					//	// Get all model components
					//	ModelComponents mc = MODELDECOMPILER.DeserializeModel(filePath.string(), metaFile.guid);
					//	//ModelComponents mc = MODELCOMPILER.LoadModel(path.string(), metaFile.guid);
					//	// Check for existing guid within the geom meta file
					//	if (mc.meshes.size() != metaFile.meshes.size())
					//	{
					//		int i = 0;
					//		for (MeshAsset& meshAsset : mc.meshes)
					//		{
					//			meshAsset.mFilePath = filePath.stem();
					//			meshAsset.mFilePath += "_" + std::to_string(i++) + ".geom";

					//			// Assign GUID
					//			Engine::GUID guid = GetGUID(meshAsset.mFilePath);
					//			metaFile.meshes.push_back(guid);

					//			std::get<AssetsTable<MeshAsset>>(assets)[guid] = std::move(meshAsset);
					//			std::get<AssetsBuffer<MeshAsset>>(assetsBuffer).emplace_back(std::make_pair(ASSET_LOADED, &std::get<AssetsTable<MeshAsset>>(assets)[guid]));
					//		}

					//		Serialize(oldMeta, metaFile);
					//	}
					//	else // Has existing guid
					//	{
					//		for (int i = 0; i < mc.meshes.size(); i++)
					//		{
					//			mc.meshes[i].mFilePath = filePath.stem();
					//			mc.meshes[i].mFilePath += "_" + std::to_string(i) + ".geom";
					//			
					//			std::get<AssetsTable<MeshAsset>>(assets)[metaFile.meshes[i]] = std::move(mc.meshes[i]);
					//			std::get<AssetsBuffer<MeshAsset>>(assetsBuffer).emplace_back(std::make_pair(ASSET_LOADED, &std::get<AssetsTable<MeshAsset>>(assets)[metaFile.meshes[i]]));
					//		}
					//	}
					//	
					//	return true;
					//}
					if constexpr (std::is_base_of<Asset, T>())
					{

						Engine::GUID<T> guid = GetGUID<T>(filePath, true);
						if (fs::is_directory(filePath))
							return true;
						T& asset{ std::get<AssetsTable<T>>(assets)[guid]};
						asset.mFilePath = filePath;
						//Pre-existing data
						if (pData)
						{
							asset.mData = std::move(*pData);
						}
						else
						{
							//load data
							std::ifstream inputFile(filePath.c_str());
							E_ASSERT(inputFile, "Error opening file to update asset in memory!");
							asset.mData.assign(
								std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());
							PRINT("Done adding ", filePath, " into memory!", '\n');
							inputFile.close();
						}
						std::get<AssetsBuffer<T>>(assetsBuffer).emplace_back(std::make_pair(ASSET_LOADED,&asset));
						return true;
					}
					else
					{
						Engine::GUID<T> guid = GetGUID<T>(filePath, true);
						std::get<AssetsTable<T>>(assets)[guid];
						return true;
					}
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
					Engine::GUID<T> guid = GetGUID<T>(filePath);
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
					Engine::GUID<T> guid = GetGUID<T>(filePath, true);
					if constexpr (std::is_base_of<Asset, T>())
					{
						T& asset{ std::get<AssetsTable<T>>(assets)[guid] };
						asset.mFilePath = filePath;
						std::ifstream inputFile(filePath.c_str());
						asset.mData.assign(
							std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());
						PRINT("Done updating ", filePath, " in memory!", '\n');
						inputFile.close();
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

	template <typename T>
	constexpr auto Importer()
	{
		if constexpr (std::is_same_v<T,ModelAsset>)
		{
			return ModelImporter();
		}
		else
		{
			return DefaultImporter();
		}
	}

	void RenameAsset(const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
	{
		FileData* fileData = GetFileData(oldPath);
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
					Engine::GUID<T> guid = GetGUID<T>(newPath, true);
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
					Engine::GUID<T> guid = GetGUID<T>(newPath, true);
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
				
				switch (pair.first)
				{
					case ASSET_LOADED:
					{
						AssetLoadedEvent<T> e{ path,GetGUID<T>(path),*pair.second };
						EVENTS.Publish(&e);
						break;
					}
					case ASSET_UPDATED:
					{
						AssetUpdatedEvent<T> e{ path,GetGUID<T>(path),*pair.second };
						EVENTS.Publish(&e);
						break;
					}
					case ASSET_UNLOADED:
					{
						
						AssetUnloadedEvent<T> e{ path,GetGUID<T>(path)};
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
	Engine::GUID<AssetType> GetGUID(const std::filesystem::path& filePath, bool update = false)
	{
		size_t assetType = GetAssetType(filePath);
		std::filesystem::path metaPath = filePath;
		metaPath += ".meta";
		using AssetImporter = decltype(Importer<AssetType>());
		AssetImporter mFile;
		Engine::GUID<AssetType> tempGUID{ mFile.guid };
		if (filePath.extension() == ".geom")
		{
			auto& table = std::get<AssetsTable<MeshAsset>>(assets);
			for (auto& pair : table)
			{
				if (pair.second.mFilePath == filePath)
				{
					return pair.first;
				}
			}
		}
		bool success = Deserialize<AssetType>(metaPath, mFile);
		if (!success)
		{
			if (([&](auto type)
			{
				using T = decltype(type);
				if (GetAssetType::E<T>() == assetType)
				{
					auto& table = std::get<AssetsTable<T>>(assets);
					for (auto& pair : table)
					{
						if (pair.second.mFilePath == filePath)
						{
							mFile.guid = pair.first;
							return true;
						}
					}
					//Could not find
					Serialize(metaPath, mFile);
					return true;
				}
				return false;
			}
			(Ts{}) || ...));
		}
		//Failed to find guid
		if (tempGUID == mFile.guid || update)
			Serialize(metaPath, mFile);
		return mFile.guid;
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

	FileData* GetFileData(const std::filesystem::path& filePath)
	{
		size_t assetType = GetAssetType(filePath);
		FileData* pData;
		if (([&](auto type)
			{
				using T = decltype(type);
				if (GetAssetType::E<T>() == assetType)
				{
					Engine::GUID<T> guid = GetGUID<T>(filePath);
					if constexpr (std::is_base_of<Asset, T>())
					{
						T& asset{ std::get<AssetsTable<T>>(assets)[guid] };
						pData = &asset.mData;
					}
					return true;
				}
				return false;
			}
			(Ts{}) || ...))
		{
			return pData;
		}
		return pData;
	}
private:
	std::tuple<AssetsTable<Ts>...> assets;
	std::tuple<AssetsBuffer<Ts>...> assetsBuffer;
};

using AllAssets = decltype(AllAssetsGroup(AssetTypes()));