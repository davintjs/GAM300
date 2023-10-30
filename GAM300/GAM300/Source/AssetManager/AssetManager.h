/*!***************************************************************************************
\file			AssetManager.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations of the following:
	1. AssetManager Initialization 
		a. Loading assets into memory with multi-threading
	2. AssetManager FileWatching Protocols
		a. Asynchronously load, unload or update assets in memory with multi-threading
	3. Getter and helper functions

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#include <unordered_map>
#include <string>
#include "Core/SystemInterface.h"

#include "Core/Events.h"
#include <filesystem>
#include "AssetManagerTemplates.h"

#define ASSET AssetManager::Instance()

namespace fs = std::filesystem;
ENGINE_SYSTEM(AssetManager)
{
public:
	// Returns the asset data with the given fileName
	const std::vector<char>& GetAssetWithFileName(const std::string& fileName);

	// Returns the GUID of the given fileName
	Engine::GUID GetAssetGUID(const fs::path& filePath);

	// Get the mesh asset
	std::unordered_map<std::string, MeshAsset>& GetMeshAsset();

	// AssetManager initialization to load assets into memory
	void Init();
	void Update(float dt);
	void Exit();
private:
	const std::string AssetPath = "Assets";

	// Helper functions
	// Asynchronously load asset into memory
	void AsyncLoadAsset(const fs::path& filePath);
	void LoadAsset(const fs::path& filePath);

	// Asynchronously unload asset from memory
	void AsyncUnloadAsset(const fs::path& filePath);
	void UnloadAsset(const fs::path& filePath);

	// Asynchronously update asset in memory
	void AsyncUpdateAsset(const fs::path& filePath);
	void UpdateAsset(const fs::path& filePath);

	void AsyncRenameAsset(const fs::path & oldPath, const fs::path & newPath);
	void RenameAsset(const fs::path & oldPath, const fs::path & newPath);

	void GetAssets();

	template <typename... Ts>
	void SubscribeGetAssets(TemplatePack<Ts...>);

	// FileWatching system using event callbacks
	void CallbackFileModified(FileModifiedEvent* pEvent);

	// Get the GUID of the asset using event callbacks
	void CallbackGetAsset(GetAssetEvent* pEvent);

	// Get the table of the asset using event callbacks
	template <typename AssetType>
	void CallbackGetAssets(GetAssetsEvent<AssetType> *pEvent);

	// Get the Filepath of the asset using event callbacks
	template <typename AssetType>
	void CallbackGetFilePath(GetFilePathEvent<AssetType> * pEvent);
	
	AllAssets assets{};

	// Get the dropped files using event callbacks
	void CallbackDroppedAsset(DropAssetsEvent* pEvent);

	bool IsCompilable(const fs::path & path);

	void Compile(const fs::path & path);
};