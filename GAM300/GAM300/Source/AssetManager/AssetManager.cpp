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
#include <unordered_set>

void AssetManager::Init()
{
	E_ASSERT(std::filesystem::exists(AssetPath), "Check if proper assets filepath exists!");

	//EVENT SUBSCRIPTIONS
	EVENTS.Subscribe(this, &AssetManager::CallbackFileModified);
	EVENTS.Subscribe(this, &AssetManager::CallbackGetAssetGUID);
	EVENTS.Subscribe(this, &AssetManager::CallbackDroppedAsset);

	// Models will have more folders, the others will be categorized based on the asset type (Character, environment, background)
	static std::unordered_set<fs::path> IGNORED_EXTENSIONS
	{
		".meta",
		".jpg",
		".png",
		".fbx",
	};
	// Models will have more folders, the others will be categorized based on the asset type (Character, environment, background)
	for (const auto& dir : std::filesystem::recursive_directory_iterator(AssetPath))
	{
		fs::path path = dir.path();
		if (IGNORED_EXTENSIONS.contains(path.extension())) // Skip if meta / fbx / desc file
		{
			continue;
		}

		// Deserialize from meta file and load the asset asynchronously
		this->AsyncLoadAsset(path);

	}
}

// Multi-threaded loading of assets
void AssetManager::AsyncLoadAsset(const fs::path& filePath)
{
	THREADS.EnqueueTask([this, filePath] { LoadAsset(filePath); });
}

void AssetManager::LoadAsset(const fs::path& filePath)
{
	ACQUIRE_SCOPED_LOCK(Assets);
	assets.AddAsset(filePath);
}


void AssetManager::Update(float dt)
{
	ACQUIRE_SCOPED_LOCK(Assets);
	UNREFERENCED_PARAMETER(dt);
	assets.ProcessBuffer();
}

void AssetManager::Exit()
{

}

// Multi-threaded unloading of assets
void AssetManager::AsyncUnloadAsset(const fs::path& filePath)
{
	THREADS.EnqueueTask([this, filePath] { UnloadAsset(filePath); });
}

void AssetManager::UnloadAsset(const fs::path& filePath)
{
	//May need to unique lock this
	ACQUIRE_SCOPED_LOCK(Assets);
	std::cout << "Done removing file from memory!" << std::endl;
}

// Multi-threaded unloading of assets
void AssetManager::AsyncUpdateAsset(const fs::path& filePath)
{
	THREADS.EnqueueTask([this, filePath] { UpdateAsset(filePath); });
}

void AssetManager::UpdateAsset(const fs::path& filePath)
{
	ACQUIRE_SCOPED_LOCK(Assets);
	assets.UpdateAsset(filePath);
}

// Get a loaded asset GUID
Engine::GUID AssetManager::GetAssetGUID(const fs::path& filePath)
{
	ACQUIRE_SCOPED_LOCK(Assets);
	return assets.GetGUID(filePath);
}



void AssetManager::CallbackFileModified(FileModifiedEvent* pEvent)
{
	//namespace fs = std::filesystem;
	//fs::path filePath{ pEvent->filePath};

	//if (filePath.empty())
	//{
	//	PRINT("EMPTY!\n");
	//}

	//std::string fileName = filePath.filename().generic_string();
	//std::string fileExtension = filePath.extension().generic_string();
	//std::string filePathEdited = filePath.generic_string();
	//std::replace(filePathEdited.begin(), filePathEdited.end(), '\\', '/');


	//if (fileExtension == ".fbx") // Call the fbx compiler
	//{
	//	const char* command{ "Compiler.exe" };

	//	system(command);

	//	return; // Return as file watcher will detect an addition of geom file and desc
	//}
	//else if (fileExtension == ".png" || fileExtension == ".jpg") // Call the texture compiler
	//{
	//	const char* command{ "TextureCompiler.exe" };

	//	system(command);
	//}

	//if (fileExtension == ".meta" || 
	//	filePath.string().find("~") != std::string::npos ||
	//	fileExtension == "")
	//{
	//	return;
	//}

	//fileName.erase(fileName.find_last_of('.'), strlen(fileExtension.c_str()) + 1);
	
	//switch (pEvent->fileState)
	//{
	//	case FileState::CREATED:
	//	{
	//		PRINT("CREATED ");
	//		LoadAsset(filePathEdited);
	//		break;
	//	}
	//	case FileState::DELETED:
	//	{
	//		PRINT("DELETED ");
	//		UnloadAsset(filePathEdited);
	//		break;
	//	}
	//	case FileState::MODIFIED:
	//	{
	//		UpdateAsset(filePathEdited);
	//		PRINT("MODIFIED ");
	//		break;
	//	}
	//	case FileState::RENAMED_OLD:
	//	{
	//		PRINT("RENAMED_OLD ");
	//		break;
	//	}
	//	case FileState::RENAMED_NEW:
	//	{
	//		PRINT("RENAMED_NEW ");
	//		break;
	//	}
	//	default:
	//	{
	//		break;
	//	}

	//}
	//if (filePath.extension() == ".cs")
	//{
	//	FileTypeModifiedEvent<FileType::SCRIPT> scriptModifiedEvent(filePath.stem().c_str(),pEvent->fileState);
	//	EVENTS.Publish(&scriptModifiedEvent);
	//}
	//PRINT(filePath.string(), "\n");
}

void AssetManager::CallbackGetAssetGUID(GetAssetEvent* pEvent)
{
	//pEvent->guid = GetAssetGUID(pEvent->filePath);
}

void AssetManager::CallbackDroppedAsset(DropAssetsEvent* pEvent)
{
	//std::list<std::filesystem::path> paths;
	//for (int i = 0; i < pEvent->pathCount; i++)
	//	paths.push_back(pEvent->paths[i]);

	//EditorGetCurrentDirectory e;
	//EVENTS.Publish(&e);

	//// Create directories / folders / files in the directory
	//for (auto path : paths)
	//{
	//	std::filesystem::path pathName = e.path + "\\" + path.filename().string();
	//	std::filesystem::copy(path, pathName);

	//	std::string::size_type i = e.path.find("Assets");
	//	if (i != std::string::npos)
	//		e.path.erase(i, 7);
	//	
	//	if(e.path.empty())
	//		pathName = path.filename().string();
	//	else
	//		pathName = e.path + "\\" + path.filename().string();

	//	if (std::filesystem::is_directory(path))
	//	{
	//		//FileAddProtocol(pathName.string(), path.filename().string(), "");
	//	}
	//	else
	//	{
	//		//FileAddProtocol(pathName.string(), path.filename().string(), path.extension().string());
	//	}
	//}
}