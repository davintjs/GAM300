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

static std::unordered_set<fs::path> TEMP_EXTENSIONS
{
	".dds",
	".geom"
};

static std::unordered_map<fs::path, std::string> COMPILABLE_EXTENSIONS
{
	{".jpg", "TextureCompiler.exe "},
	{".png", "TextureCompiler.exe "},
	{".fbx", "ModelCompiler.exe "},
	{".obj", "ModelCompiler.exe "},
};

void AssetManager::Compile(const fs::path& path)
{
	std::string command = COMPILABLE_EXTENSIONS[path.extension()] + path.string();
	system(command.c_str());
}

bool AssetManager::IsCompilable(const fs::path& path)
{
	//Read metafile
	return assets.IsModified(path) && COMPILABLE_EXTENSIONS.contains(path.extension());
}

void AssetManager::Init()
{
	E_ASSERT(std::filesystem::exists(AssetPath), "Check if proper assets filepath exists!");


	//EVENT SUBSCRIPTIONS
	EVENTS.Subscribe(this, &AssetManager::CallbackFileModified);
	EVENTS.Subscribe(this, &AssetManager::CallbackGetAsset);
	EVENTS.Subscribe(this, &AssetManager::CallbackDroppedAsset);
	EVENTS.Subscribe(this, &AssetManager::CallbackGetFilePath);

	for (const auto& dir : std::filesystem::recursive_directory_iterator(AssetPath))
	{
		fs::path path = dir.path();
		// Deserialize from meta file and load the asset asynchronously
		this->AsyncLoadAsset(dir.path());

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
	if (filePath.extension() == ".meta")
	{
		fs::path nonMeta = filePath;
		nonMeta.replace_extension("");
		//Actual file does not exist
		if (!std::filesystem::exists(nonMeta))
		{
			std::filesystem::remove(filePath);
		}
		return;
	}
	if (IsCompilable(filePath))
		Compile(filePath);
	assets.AddAsset(filePath);
}


void AssetManager::Update(float dt)
{
	//Add a bool 
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
	//One does not simply delete a meta file while the engine is running >:C
	if (filePath.extension() == ".meta")
	{
		fs::path nonMeta = filePath;
		nonMeta.replace_extension("");
		if (fs::exists(nonMeta))
		{
			PRINT("I see you just tried to delete a meta file... So you have chosen death\n");
			assets.CreateMeta(nonMeta);
		}
		return;
	}
	assets.RemoveAsset(filePath);
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


void AssetManager::AsyncRenameAsset(const fs::path& oldPath, const fs::path& newPath)
{
	ACQUIRE_SCOPED_LOCK(Assets);
	THREADS.EnqueueTask([this, oldPath, newPath] { RenameAsset(oldPath,newPath); });
}

void AssetManager::RenameAsset(const fs::path& oldPath, const fs::path& newPath)
{
	if (oldPath.extension() == ".meta")
	{
		//Trying to convert a meta file to something else...
		if (newPath.extension() != ".meta")
		{
			fs::rename(newPath, oldPath);
			return;
		}

		fs::path nonMeta = oldPath;
		nonMeta.replace_extension("");
		//Old assigned file still exists
		if (fs::exists(nonMeta))
		{
			fs::rename(newPath, oldPath);
			return;
		}

		nonMeta = newPath;
		nonMeta.replace_extension("");
		//Old assigned file does not exist and new assigned file does not exist
		if (fs::exists(nonMeta))
		{
			fs::remove(newPath);
			return;
		}

		//Ok rename meta file done by assetmanager
		return;
	}
	assets.RenameAsset(oldPath,newPath);
}

// Get a loaded asset GUID
Engine::GUID AssetManager::GetAssetGUID(const fs::path& filePath)
{
	ACQUIRE_SCOPED_LOCK(Assets);
	auto it = DEFAULT_ASSETS.find(filePath);
	if (it != DEFAULT_ASSETS.end())
	{
		return it->second;
	}
	return assets.GetGUID(filePath);
}



void AssetManager::CallbackFileModified(FileModifiedEvent* pEvent)
{
	fs::path filePath{ "Assets/"};
	filePath += pEvent->filePath;

	fs::path extension{ filePath.extension() };

	if (filePath.empty())
		return;

	static fs::path oldPath{};
	//If added

	////If deleted, generate a new one
	//if (extension == ".meta")
	//{
	//	fs::path nonMeta = filePath;
	//	nonMeta.replace_extension("");
	//	if (std::filesystem::exists(nonMeta))
	//	{

	//	}
	//}

	//temp file, invalid file
	if (filePath.string().find("~") != std::string::npos)
		return;

	//temp file, invalid file
	if (!fs::is_directory(filePath) && !filePath.has_extension())
		return;

	
	switch (pEvent->fileState)
	{
		case FileState::CREATED:
		{
			PRINT("CREATED ");
			AsyncLoadAsset(filePath);
			break;
		}
		case FileState::DELETED:
		{
			PRINT("DELETED ");
			AsyncUnloadAsset(filePath);
			break;
		}
		case FileState::MODIFIED:
		{
			AsyncUpdateAsset(filePath);
			PRINT("MODIFIED ");
			break;
		}
		case FileState::RENAMED_OLD:
		{
			oldPath = filePath;
			PRINT("RENAMED_OLD ");
			break;
		}
		case FileState::RENAMED_NEW:
		{
			AsyncRenameAsset(oldPath,filePath);
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

void AssetManager::CallbackGetAsset(GetAssetEvent* pEvent)
{
	pEvent->guid = GetAssetGUID(pEvent->filePath);
}

void AssetManager::CallbackDroppedAsset(DropAssetsEvent* pEvent)
{

	EditorGetCurrentDirectory e;
	EVENTS.Publish(&e);

	// Create directories / folders / files in the directory
	for (int i = 0; i < pEvent->pathCount; ++i)
	{
		const std::filesystem::path& path = pEvent->paths[i];
		std::filesystem::path pathName = e.path + "\\" + path.filename().string();
		std::filesystem::copy(path, pathName);
		AsyncLoadAsset(pathName);
	}
}


void AssetManager::CallbackGetFilePath(GetFilePathEvent* pEvent)
{
	pEvent->filePath = assets.GetFilePath(pEvent->guid);
}