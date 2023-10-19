/*!***************************************************************************************
\file			AssetManager.cpp
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definitions of the following:
	1. AssetManager Initialization
		a. Loading assets into memory with multi-threading
	2. AssetManager FileWatching Protocols
		a. Asynchronously load, unload or update assets in memory with multi-threading
	3. Getter and helper functions

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "AssetManager/AssetManager.h"
#include "Utilities/ThreadPool.h"
#include "Core/EventsManager.h"
#include "Core/FileTypes.h"
#include "yaml-cpp/yaml.h"

void AssetManager::Init()
{
	E_ASSERT(std::filesystem::exists(AssetPath), "Check if proper assets filepath exists!");

	//EVENT SUBSCRIPTIONS
	EVENTS.Subscribe(this, &AssetManager::CallbackFileModified);
	EVENTS.Subscribe(this, &AssetManager::CallbackGetAssetGUID);
	EVENTS.Subscribe(this, &AssetManager::CallbackDroppedAsset);
	
	MeshManager.Init();

	// Models will have more folders, the others will be categorized based on the asset type (Character, environment, background)

	std::string subFilePath{};
	// Models will have more folders, the others will be categorized based on the asset type (Character, environment, background)
	for (const auto& dir : std::filesystem::recursive_directory_iterator(AssetPath))
	{
		subFilePath = dir.path().generic_string();
		std::string subFilePathMeta = subFilePath, assetPath = subFilePath;
		std::string fileType = ".";
		std::string fileName{};
		
		if (!dir.is_directory())
		{
			// Check if is file with no extension
			auto check = subFilePath.find_last_of('.');
			E_ASSERT(check != std::string::npos, "File with no extension found! Remove it from the assets folder.");

			for (size_t i = check + 1; i != strlen(subFilePath.c_str()); ++i)
			{
				fileType += subFilePath[i];
			}
		}

		if (dir.is_directory())
		{
			fileName = std::filesystem::path(dir).filename().generic_string();
		}
		else
		{
			for (size_t j = subFilePath.find_last_of('/') + 1; j != subFilePath.find_last_of('.'); ++j)
			{
				fileName += subFilePath[j];
			}
		}

		// Add into file extensions list

		if (!strcmp(fileType.c_str(), ".meta")) // Skip if meta / fbx / desc file
		{
			continue;
		}

		subFilePathMeta += ".meta";
		if (!std::filesystem::exists(subFilePathMeta))
		{
			CreateMetaFile(fileName, subFilePathMeta, fileType);
		}

		// Mark meta files as hidden files
		std::wstring wideStr = std::wstring(subFilePathMeta.begin(), subFilePathMeta.end());
		const wchar_t* fileLPCWSTR = wideStr.c_str();
		int attribute = GetFileAttributes(fileLPCWSTR);
		if ((attribute & FILE_ATTRIBUTE_HIDDEN) == 0)
		{
			SetFileAttributes(fileLPCWSTR, attribute | FILE_ATTRIBUTE_HIDDEN);
		}

		// We still want to create the meta files for these that's why we skip here instead of above
		if (!strcmp(fileType.c_str(), ".jpg") || !strcmp(fileType.c_str(), ".png"))
		{
			continue;
		}

		// Deserialize from meta file and load the asset asynchronously
		if (!dir.is_directory())
		{
			this->AsyncLoadAsset(subFilePathMeta, fileName);

			if (!strcmp(fileType.c_str(), ".dds")) // if dds ...
			{
				//this->AsyncLoadAsset(subFilePathMeta, fileName, true);
				std::string filetype = assetPath/* + ".dds"*/;
				Engine::GUID guid = GetAssetGUID(fileName);
				while (guid == Engine::GUID(""))
					guid = GetAssetGUID(fileName);
				TextureManager.AddTexture(assetPath.c_str(), GetAssetGUID(fileName));

			}
			else if (!strcmp(fileType.c_str(), ".geom"))
			{
				MeshManager.GetGeomFromFiles(subFilePath, fileName);
			}
			else if (!strcmp(fileType.c_str(), ".mp3") || !strcmp(fileType.c_str(), ".wav")) {
				AUDIOMANAGER.AddMusic(subFilePath.c_str(), fileName);
				AUDIOMANAGER.AddSFX(subFilePath.c_str(), fileName);
				//AUDIOMANAGER.AddMusic(assetPath.c_str(), fileName);
			}
		}
	}
}

// For run time update of files
void AssetManager::Update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
}

void AssetManager::Exit()
{

}

// Multi-threaded loading of assets
void AssetManager::AsyncLoadAsset(const std::string& metaFilePath, const std::string& fileName, bool isDDS)
{
	THREADS.EnqueueTask([this, metaFilePath, fileName, isDDS] { LoadAsset(metaFilePath, fileName, isDDS); });
}

void AssetManager::LoadAsset(const std::string& metaFilePath, const std::string& fileName, bool isDDS)
{
	ACQUIRE_SCOPED_LOCK(Assets);
	DeserializeAssetMeta(metaFilePath, fileName, isDDS);
}

// Multi-threaded unloading of assets
void AssetManager::AsyncUnloadAsset(const Engine::GUID& assetGUID)
{
	THREADS.EnqueueTask([this, assetGUID] { UnloadAsset(assetGUID); });
}

void AssetManager::UnloadAsset(const Engine::GUID& assetGUID)
{
	//May need to unique lock this
	ACQUIRE_SCOPED_LOCK(Assets);
	mAssets.erase(assetGUID);
	std::cout << "Done removing file from memory!" << std::endl;
}

// Multi-threaded unloading of assets
void AssetManager::AsyncUpdateAsset(const std::string& assetPath, const Engine::GUID& assetGUID)
{
	THREADS.EnqueueTask([this, assetPath, assetGUID] { UpdateAsset(assetPath, assetGUID); });
}

void AssetManager::UpdateAsset(const std::string& assetPath, const Engine::GUID& assetGUID)
{
	ACQUIRE_SCOPED_LOCK(Assets);

	std::ifstream inputFile(assetPath.c_str());
	E_ASSERT(inputFile, "Error opening file to update asset in memory!");

	std::vector<char> buff(std::istreambuf_iterator<char>(inputFile), {});
	mAssets[assetGUID].mData = std::move(buff); // Update the data in memory

	std::cout << "Done updating file in memory!" << assetPath << std::endl;

	inputFile.close();
}

// Get a loaded asset
const std::vector<char>& AssetManager::GetAssetWithFileName(const std::string& fileName)
{
	Engine::GUID data{};
	auto func =
	[this, &fileName, &data] // Wait if the asset is not loaded yet
	{
		for (const auto& [Key, Val] : mAssets)
		{
			if (Val.mFileName == fileName)
			{
				data = Key;
				return true;
			}
		}
		return false;
	};
	ACQUIRE_UNIQUE_LOCK
	(
		Assets, func
	);

	return mAssets[data].mData;
}

// Get a loaded asset
const std::vector<char>& AssetManager::GetAssetWithGUID(const Engine::GUID& GUID)
{
	auto func =
	[this, &GUID] // Wait if the asset is not loaded yet
	{
		if (mAssets.find(GUID) != mAssets.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	};
	ACQUIRE_UNIQUE_LOCK
	(
		Assets, func
	);

	return mAssets[GUID].mData;
}

// Get a loaded asset GUID
Engine::GUID AssetManager::GetAssetGUID(const std::string& fileName)
{
	ACQUIRE_SCOPED_LOCK(Assets);
	for (const auto& [key, val] : mAssets)
	{
		if (val.mFileName == fileName)
		{
			return key;
		}
	}
	return Engine::GUID("");
}

std::unordered_map<std::string, MeshAsset>& AssetManager::GetMeshAsset()
{
	return mMeshesAsset;
}

void AssetManager::StoreMeshVertex(const std::string& mKey, const glm::vec3& mVertex)
{
	mMeshesAsset[mKey].mVertices.push_back(mVertex);
}

void AssetManager::StoreMeshIndex(const std::string& mKey, const int& mIndex)
{
	mMeshesAsset[mKey].mIndices.push_back(mIndex);
}

void AssetManager::CreateMetaFile(const std::string& fileName, const std::string& filePath, const std::string& fileType)
{
	Engine::GUID guid;
	std::string fileAssetPath = filePath;
	fileAssetPath.erase(fileAssetPath.find_last_of('.'), strlen(".meta") + 1);
	if (strcmp(fileType.c_str(), ""))
	{
		fileAssetPath += fileType;
	}
	
	YAML::Emitter out;


	out << YAML::BeginMap;
	out << YAML::Key << "guid" << YAML::Value << guid.ToHexString();

	std::ofstream ofs(filePath);
	ofs << out.c_str();
	ofs.flush();
	ofs.close();

	return;
}

void AssetManager::DeserializeAssetMeta(const std::filesystem::path& filePath, const std::string& fileName, bool /*isDDS*/)
{
	std::ifstream ifs(filePath);
	PRINT(filePath,'\n');
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	ifs.close();

	std::vector<YAML::Node> data;

	// Load the scene again for linkages
	data = YAML::LoadAllFromFile(filePath.string());

	std::string GUIDofAsset;

	for (YAML::Node& node : data)
	{
		YAML::detail::iterator_value kv = *node.begin();
		if (node["guid"]) // Deserialize Gameobject
		{
			GUIDofAsset = kv.second.as<std::string>();
		}
	}
	if (GUIDofAsset.size() < 16)
		PRINT("Less than 16 GUID ", filePath, '\n');

	std::filesystem::path assetPath = filePath;
	assetPath.replace_extension("");
	std::ifstream inputFile(assetPath.c_str());
	E_ASSERT(inputFile, "Error opening file to load asset into memory!");

	std::vector<char> buff(std::istreambuf_iterator<char>(inputFile), {});

	std::filesystem::directory_entry path(assetPath);
	std::filesystem::path fPath{ assetPath };
	Asset tempFI{ fPath };
	tempFI.mData = buff;
	this->mAssets.insert(std::make_pair(GUIDofAsset, tempFI));
	assets.AddAsset(fPath);
	inputFile.close();
}

void AssetManager::FileAddProtocol(const std::string& filePath, const std::string& fileName, const std::string& fileExtension)
{
	ACQUIRE_SCOPED_LOCK(Assets);

	// Remove extension in filePath and add .meta
	std::string filePathMeta = "Assets/" + filePath;
	filePathMeta += ".meta";

	// Remove the . in fileExtension
	std::string fileExtensionEdited = fileExtension;
	fileExtensionEdited.erase(fileExtension.find('.'));

	CreateMetaFile(fileName, filePathMeta, fileExtension);

	// Mark meta files as hidden files
	std::wstring wideStr = std::wstring(filePathMeta.begin(), filePathMeta.end());
	const wchar_t* fileLPCWSTR = wideStr.c_str();
	int attribute = GetFileAttributes(fileLPCWSTR);
	if ((attribute & FILE_ATTRIBUTE_HIDDEN) == 0)
	{
		SetFileAttributes(fileLPCWSTR, attribute | FILE_ATTRIBUTE_HIDDEN);
	}

	if (fileExtension == ".jpg" || fileExtension == ".png")
	{
		return; // Do not load jpg and png files as we want to load the dds version
	}

	// Deserialize from meta file and load the asset asynchronously
	this->AsyncLoadAsset(filePathMeta, fileName);
}

void AssetManager::FileRemoveProtocol(const std::string& filePath, const std::string& fileName, const std::string& fileExtension)
{
	ACQUIRE_SCOPED_LOCK(Assets);

	std::string filePathMeta = "Assets/" + filePath;
	filePathMeta.erase(filePathMeta.find_last_of('.'), strlen(fileExtension.c_str()) + 1);
	filePathMeta += ".meta";

	std::ifstream ifs(filePathMeta);
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	ifs.close();

	std::vector<YAML::Node> data;

	// Load the scene again for linkages
	data = YAML::LoadAllFromFile(filePath);

	std::string GUIDofAsset;

	for (YAML::Node& node : data)
	{
		if (node["guid"]) // Deserialize Gameobject
		{
			GUIDofAsset = node.as<std::string>();
		}
	}

	std::filesystem::remove(filePathMeta); // Delete meta file

	this->AsyncUnloadAsset(GUIDofAsset); // Unload asset from memory
}

// Change meta file name (And path to the updated name)
void AssetManager::FileUpdateProtocol(const std::string& filePath, const std::string& fileExtension)
{
	ACQUIRE_SCOPED_LOCK(Assets);

	// Remove extension in filePath and add .meta
	std::string filePathMeta = "Assets/" + filePath;
	filePathMeta.erase(filePathMeta.find_last_of('.'), strlen(fileExtension.c_str()) + 1);
	filePathMeta += ".meta";

	// Get the asset file path and GUID from meta file of the asset
	std::ifstream ifs(filePathMeta);
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	ifs.close();

	std::string GUIDofAsset;

	std::vector<YAML::Node> data;

	// Load the scene again for linkages
	data = YAML::LoadAllFromFile(filePath);

	for (YAML::Node& node : data)
	{
		if (node["guid"]) // Deserialize Gameobject
		{
			GUIDofAsset = node.as<std::string>();
		}
	}

	std::string assetPath{ filePath.begin(),filePath.begin() + filePath.find_first_of(".") };
	if (!std::filesystem::is_directory(assetPath))
	{
		// The asset file associated with this meta file was updated
		mAssets[GUIDofAsset].mData.clear(); // Remove the data in memory
		this->AsyncUpdateAsset(assetPath, GUIDofAsset); // Add the new data into memory
	}
}


void AssetManager::CallbackFileModified(FileModifiedEvent* pEvent)
{
	namespace fs = std::filesystem;
	fs::path filePath{ pEvent->filePath};

	if (filePath.empty())
	{
		PRINT("EMPTY!\n");
	}

	std::string fileName = filePath.filename().generic_string();
	std::string fileExtension = filePath.extension().generic_string();
	std::string filePathEdited = filePath.generic_string();
	std::replace(filePathEdited.begin(), filePathEdited.end(), '\\', '/');


	if (fileExtension == ".fbx") // Call the fbx compiler
	{
		const char* command{ "Compiler.exe" };

		system(command);

		return; // Return as file watcher will detect an addition of geom file and desc
	}
	else if (fileExtension == ".png" || fileExtension == ".jpg") // Call the texture compiler
	{
		const char* command{ "TextureCompiler.exe" };

		system(command);
	}

	if (fileExtension == ".meta" || 
		filePath.string().find("~") != std::string::npos ||
		fileExtension == "")
	{
		return;
	}

	fileName.erase(fileName.find_last_of('.'), strlen(fileExtension.c_str()) + 1);
	
	switch (pEvent->fileState)
	{
		case FileState::CREATED:
		{
			PRINT("CREATED ");
			FileAddProtocol(filePathEdited, fileName, fileExtension);
			break;
		}
		case FileState::DELETED:
		{
			PRINT("DELETED ");
			FileRemoveProtocol(filePathEdited, fileName, fileExtension);
			break;
		}
		case FileState::MODIFIED:
		{
			FileUpdateProtocol(filePathEdited, fileExtension);
			PRINT("MODIFIED ");
			break;
		}
		case FileState::RENAMED_OLD:
		{
			PRINT("RENAMED_OLD ");
			break;
		}
		case FileState::RENAMED_NEW:
		{
			PRINT("RENAMED_NEW ");
			break;
		}
		default:
		{
			break;
		}

	}
	if (filePath.extension() == ".cs")
	{
		FileTypeModifiedEvent<FileType::SCRIPT> scriptModifiedEvent(filePath.stem().c_str(),pEvent->fileState);
		EVENTS.Publish(&scriptModifiedEvent);
	}
	PRINT(filePath.string(), "\n");
}

void AssetManager::CallbackGetAssetGUID(GetAssetEvent* pEvent)
{
	pEvent->guid = GetAssetGUID(pEvent->fileName);
}

void AssetManager::CallbackDroppedAsset(DropAssetsEvent* pEvent)
{
	std::list<std::filesystem::path> paths;
	for (int i = 0; i < pEvent->pathCount; i++)
		paths.push_back(pEvent->paths[i]);

	EditorGetCurrentDirectory e;
	EVENTS.Publish(&e);

	// Create directories / folders / files in the directory
	for (auto path : paths)
	{
		std::filesystem::path pathName = e.path + "\\" + path.filename().string();
		std::filesystem::copy(path, pathName);

		std::string::size_type i = e.path.find("Assets");
		if (i != std::string::npos)
			e.path.erase(i, 7);
		
		if(e.path.empty())
			pathName = path.filename().string();
		else
			pathName = e.path + "\\" + path.filename().string();

		if (std::filesystem::is_directory(path))
		{
			FileAddProtocol(pathName.string(), path.filename().string(), "");
		}
		else
		{
			FileAddProtocol(pathName.string(), path.filename().string(), path.extension().string());
		}
	}
}