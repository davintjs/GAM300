#pragma once

#include "AssetTypes.h"

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
					if constexpr (std::is_base_of<Asset, T>)
					{
						//load data
						std::ifstream inputFile(filePath.c_str());
						E_ASSERT(inputFile, "Error opening file to update asset in memory!");
						std::vector<char> buff(std::istreambuf_iterator<char>(inputFile), {});
						std::cout << "Done updating file in memory!" << filePath << std::endl;
						inputFile.close();
						std::get<AssetsTable<T>>(assets)[guid] = { buff };
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
					if constexpr (std::is_base_of<Asset, T>)
					{
						T& asset = std::get<AssetsTable<T>>(assets)[guid];
						//load data
						std::ifstream inputFile(filePath.c_str());
						E_ASSERT(inputFile, "Error opening file to update asset in memory!");
						asset.buff.clear();
						asset.assign(std::istreambuf_iterator<char>(inputFile));
						std::cout << "Done updating file in memory!" << filePath << std::endl;
						inputFile.close();
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
		std::fstream fs(metaPath);
		Engine::GUID guid;
		std::vector<YAML::Node> data;
		// Load the scene again for linkages
		data = YAML::LoadAllFromFile(metaPath.string());
		//Try and see if theres a guid, else assign a guid
		for (YAML::Node& node : data)
		{
			YAML::detail::iterator_value kv = *node.begin();
			if (node["guid"]) // Deserialize guid
			{
				guid = kv.second.as<std::string>();
				fs.close();
				return guid;
			}
		}

		//Store newly generated guid since it couldnt be found
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "guid" << YAML::Value << guid.ToHexString();
		fs << out.c_str();
		fs.close();

		// Mark meta files as hidden files
		std::wstring wideStr = std::wstring(metaPath.begin(), metaPath.end());
		const wchar_t* fileLPCWSTR = wideStr.c_str();
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