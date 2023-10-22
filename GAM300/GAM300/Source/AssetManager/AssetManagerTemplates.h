#pragma once

#include "AssetTypes.h"
#include "Core/EventsManager.h"

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
					Engine::GUID guid = GetGUID(filePath);
					if constexpr (std::is_base_of<Asset, T>())
					{
						//load data
						std::ifstream inputFile(filePath.c_str());
						E_ASSERT(inputFile, "Error opening file to update asset in memory!");
						T& asset{ std::get<AssetsTable<T>>(assets)[guid]};
						asset.mData.assign(
							std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());
						std::cout << "Done updating file in memory!" << filePath << std::endl;
						inputFile.close();
						AssetLoadedEvent<T> e{ filePath,guid,asset };
						EVENTS.Publish(&e);
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
private:
	std::tuple<AssetsTable<Ts>...> assets;
};

using AllAssets = decltype(AllAssetsGroup(AssetTypes()));