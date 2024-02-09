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
// Bean: Include model compiler here temporarily, should use events instead
#include "ModelCompiler.h"
#include "TextureCompiler.h"
#include "FontCompiler.h"

static std::vector<fs::path> COMPILABLE_EXTENSIONS
{
	{".jpg"},
	{".png"},
	{".fbx"},
	{".obj"},
	{".ttf"},
	//{".cs"},
};

void AssetManager::Compile(const fs::path& path)
{
	if (path.extension() == ".fbx" || path.extension() == ".obj")
	{
		// Bean: Need to store all the material, shader, animation, mesh somewhere in asset manager
		MODELCOMPILER.LoadModel(path);
	}
	else if (path.extension() == ".png" || path.extension() == ".jpg")
	{
		// Bean: From TextureCompiler.h
		LoadTexture(path);
	}
	else if (path.extension() == ".ttf")
	{
		// Toe: From FontCompiler.h
		LoadFont(path);
	}
	//else
	//{
	//	system(command.c_str());
	//}
}

bool AssetManager::IsCompilable(const fs::path& path)
{
	//Read metafile
	return std::find(COMPILABLE_EXTENSIONS.begin(), COMPILABLE_EXTENSIONS.end(), path.extension()) != COMPILABLE_EXTENSIONS.end() 
		&& assets.IsModified(path);
}



template <typename... Ts>
void AssetManager::SubscribeGetAssets(TemplatePack<Ts...>)
{
	(([&](auto type)
	{
		using T = decltype(type);
		EVENTS.Subscribe(this, &AssetManager::CallbackGetAsset<T>);
		EVENTS.Subscribe(this,&AssetManager::CallbackGetAssets<T>);
		EVENTS.Subscribe(this, &AssetManager::CallbackAddSubAssetEvent<T>);
		EVENTS.Subscribe(this,&AssetManager::CallbackGetFilePath<T>);
		EVENTS.Subscribe(this, &AssetManager::CallbackGetAssetByGUID<T>);
	})(Ts{}), ...);
}

void AssetManager::Init()
{
	E_ASSERT(std::filesystem::exists(AssetPath), "Check if proper assets filepath exists!");


	//EVENT SUBSCRIPTIONS
	EVENTS.Subscribe(this, &AssetManager::CallbackFileModified);
	EVENTS.Subscribe(this, &AssetManager::CallbackDroppedAsset);
	SubscribeGetAssets(AssetTypes());

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
	}
	THREADS.EnqueueTask([this, filePath] { LoadAsset(filePath); });
}

void AssetManager::LoadAsset(const fs::path& filePath)
{
	ACQUIRE_SCOPED_LOCK(Assets);
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
		//fs::path nonMeta = filePath;
		//nonMeta.replace_extension("");
		//if (fs::exists(nonMeta))
		//{
		//	PRINT("I see you just tried to delete a meta file... So you have chosen death\n");
		//	assets.GetGUID(nonMeta);
		//}
		return;
	}
	assets.RemoveAsset(filePath);
	std::cout << "Done removing file from memory!" << std::endl;
}

// Multi-threaded unloading of assets
void AssetManager::AsyncUpdateAsset(const fs::path& filePath)
{
	if (filePath.extension() == ".meta")
		return;
	if (fs::is_directory(filePath))
		return;
	THREADS.EnqueueTask([this, filePath] { UpdateAsset(filePath); });
}

void AssetManager::UpdateAsset(const fs::path& filePath)
{
	ACQUIRE_SCOPED_LOCK(Assets);
	if (IsCompilable(filePath))
		Compile(filePath);
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
		if (!fs::exists(nonMeta))
		{
			fs::remove(newPath);
			return;
		}

		//Ok rename meta file done by assetmanager
		return;
	}
	assets.RenameAsset(oldPath,newPath);
}

template <typename AssetType>
void AssetManager::CallbackGetAssets(GetAssetsEvent<AssetType>* pEvent)
{
	pEvent->pAssets = &assets.GetAssets<AssetType>();
}


template <typename AssetType>
void AssetManager::CallbackAddSubAssetEvent(AddSubAssetEvent<AssetType>* pEvent)
{
	E_ASSERT(!pEvent->asset.mFilePath.empty(), "You cannot add a sub asset of empty filepath");
	AssetImporter<AssetType> importer{ pEvent->guid };
	assets.AddSubAsset(pEvent->asset, importer);
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
			//AsyncUpdateAsset(filePath);
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

template <typename T>
void AssetManager::CallbackGetAsset(GetAssetEvent<T>* pEvent)
{
	pEvent->guid = assets.GetImporter<T>(pEvent->filePath).guid;
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

template <typename AssetType>
void AssetManager::CallbackGetFilePath(GetFilePathEvent<AssetType>* pEvent)
{
	pEvent->filePath = assets.GetFilePath(pEvent->guid);
}


template <typename AssetType>
void AssetManager::CallbackGetAssetByGUID(GetAssetByGUIDEvent<AssetType>* pEvent)
{
	pEvent->asset = &assets.GetAssets<AssetType>()[pEvent->guid];
	pEvent->importer = &GetImporterTable<AssetType>()[pEvent->asset->mFilePath];
}