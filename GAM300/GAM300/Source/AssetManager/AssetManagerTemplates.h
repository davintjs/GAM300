#pragma once

#include "AssetTypes.h"
#include "Core/EventsManager.h"
#include <vector>
#include <unordered_set>

//Hash table that maps an ID to a pointer
template <typename T>
using AssetsTable = std::unordered_map<Engine::GUID, T>;

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

	void AddAsset(const std::filesystem::path& filePath)
	{

		static std::unordered_set<fs::path> IGNORED_EXTENSIONS
		{
			".meta",
			".jpg",
			".png",
			".fbx",
		};

		if (IGNORED_EXTENSIONS.contains(filePath.extension())) // Skip if meta / fbx / desc file
		{
			if (filePath.extension() == ".png" || filePath.extension() == ".jpg")
			{
				fs::path ddsPath = filePath;
				ddsPath.replace_extension(".dds");
				if (fs::exists(ddsPath))
					return;
				std::string command{ "TextureCompiler.exe " };
				command += filePath.string();
				system(command.c_str());
			}
			return;
		}

		size_t assetType = AssetExtensionTypes[filePath.extension().string()];
		if (([&](auto type)
			{
				using T = decltype(type);
				if (GetAssetType::E<T>() == assetType)
				{
					Engine::GUID guid = GetGUID(filePath);
					if constexpr (std::is_base_of<Asset, T>())
					{
						//load data
						std::ifstream inputFile(filePath.c_str());
						E_ASSERT(inputFile, "Error opening file to update asset in memory!");
						T& asset{ std::get<AssetsTable<T>>(assets)[guid]};
						asset.mFilePath = filePath;
						asset.mData.assign(
							std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());
						std::cout << "Done updating file in memory!" << filePath << std::endl;
						inputFile.close();
						std::get<AssetsBuffer<T>>(assetsBuffer).emplace_back(std::make_pair(ASSET_LOADED,&asset));
					}
					else
					{
						std::get<AssetsTable<T>>(assets)[guid];
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

	bool RemoveAsset(const std::filesystem::path& filePath)
	{
		size_t assetType = AssetExtensionTypes[filePath.extension().string()];
		if (([&](auto type)
			{
				using T = decltype(type);
				if (GetAssetType::E<T>() == assetType)
				{
					Engine::GUID guid = GetGUID(filePath);
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

	void UpdateAsset(const std::filesystem::path& filePath)
	{
		size_t assetType = AssetExtensionTypes[filePath.extension().string()];
		if (([&](auto type)
			{
				using T = decltype(type);
				if (GetAssetType::E<T>() == assetType)
				{
					Engine::GUID guid = GetGUID(filePath);
					if constexpr (std::is_base_of<Asset, T>())
					{
						T& asset = std::get<AssetsTable<T>>(assets)[guid];
						//load data
						//std::ifstream inputFile(filePath.c_str());
						//E_ASSERT(inputFile, "Error opening file to update asset in memory!");
						//asset.mData.clear();
						//asset.mData.assign({ std::istreambuf_iterator<char>(inputFile) });
						//std::cout << "Done updating file in memory!" << filePath << std::endl;
						//inputFile.close();
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

	void ProcessBuffer()
	{
		(([&](auto type) 
		{
			using T = decltype(type);
			auto& buffer{std::get<AssetsBuffer<T>>(assetsBuffer)};
			for (auto& pair : buffer)
			{
				fs::path path{pair.second->mFilePath };
				switch (pair.first)
				{
					case ASSET_LOADED:
					{
						AssetLoadedEvent<T> e{ path,GetGUID(path),*pair.second };
						EVENTS.Publish(&e);
						break;
					}
					case ASSET_UPDATED:
					{
						break;
					}
					case ASSET_UNLOADED:
					{
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

	Engine::GUID GetGUID(const std::filesystem::path& filePath)
	{
		E_ASSERT(std::filesystem::exists(filePath), "File does not exist, ", filePath);
		std::filesystem::path metaPath = filePath;
		metaPath += ".meta";
		Engine::GUID guid;
		if (std::filesystem::exists(metaPath))
		{
			std::vector<YAML::Node> data = YAML::LoadAllFromFile(metaPath.string());
			for (YAML::Node& node : data)
			{
				YAML::detail::iterator_value kv = *node.begin();
				if (node["guid"]) // Deserialize guid
				{
					guid = Engine::GUID(kv.second.as<std::string>());
					return guid;
				}
			}
		}
		//Store newly generated guid since it couldnt be found
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "guid" << YAML::Value << guid.ToHexString();
		std::ofstream fs(metaPath);
		fs << out.c_str();
		fs.close();

		//// Mark meta files as hidden files
		const wchar_t* fileLPCWSTR = metaPath.wstring().c_str();
		int attribute = GetFileAttributes(fileLPCWSTR);
		if ((attribute & FILE_ATTRIBUTE_HIDDEN) == 0)
		{
			SetFileAttributes(fileLPCWSTR, attribute | FILE_ATTRIBUTE_HIDDEN);
		}
		return guid;
	}

	fs::path GetFilePath(const Engine::GUID& guid)
	{
		fs::path path;
		(([&](auto type)
			{
				using T = decltype(type);
				auto& table = std::get<AssetsTable<T>>(assets);
				if (table.find(guid) != table.end())
					return table[guid];
			})(Ts{}), ...);
		return path;
	}
private:
	std::tuple<AssetsTable<Ts>...> assets;
	std::tuple<AssetsBuffer<Ts>...> assetsBuffer;
};

using AllAssets = decltype(AllAssetsGroup(AssetTypes()));