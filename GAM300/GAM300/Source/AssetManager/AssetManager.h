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
#include "Core/SystemInterface.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "Core/Events.h"
#include "Graphics/GraphicStructsAndClass.h"
#include "Graphics/TextureManager.h"
#include "Graphics/MeshManager.h"
#include "Audio/AudioManager.h"

#define ASSETMANAGER AssetManager::Instance()

// GUID, last file update time, file name, data
struct FileInfo
{
	FileInfo() {};
	FileInfo(std::filesystem::file_time_type ft, std::string fn, std::vector<char> data) : mFileTime(ft), mFileName(fn), mData(data) {}
	~FileInfo()
	{
		mData.clear();
	}

	std::filesystem::file_time_type mFileTime;
	std::string mFileName;
	std::vector<char> mData;
};

struct MeshAsset
{
	std::vector<glm::vec3> mVertices;
	std::vector<unsigned int> mIndices;
};

struct Asset
{
	std::unordered_map<std::string, FileInfo> mFilesData;
	std::unordered_map<std::string, std::vector<std::string>> mExtensionFiles; // File extension, file names (For Zac)
	std::unordered_map<std::string, MeshAsset> mMeshesAsset; // File name, mesh vertices and indices (For Sean)
};

ENGINE_SYSTEM(AssetManager)
{
public:
	// Returns the asset data with the given fileName
	const std::vector<char>& GetAssetWithFileName(const std::string& fileName);
	const std::vector<char>& GetAssetWithGUID(const std::string& GUID);

	// Returns the GUID of the given fileName
	std::string GetAssetGUID(const std::string& fileName);

	// Get the mesh asset
	std::unordered_map<std::string, MeshAsset>& GetMeshAsset();

	// Adds mesh asset for storing
	void StoreMeshVertex(const std::string& mKey, const glm::vec3& mVertex);
	void StoreMeshIndex(const std::string& mKey, const int& mIndex);

	// AssetManager initialization to load assets into memory
	void Init();

	// Update loop
	void Update(float dt);

	// Exit
	void Exit();

private:
	const std::string AssetPath = "Assets";

	// Asset stuff
	Asset mTotalAssets;

	// Helper functions
	// Asynchronously load asset into memory
	void AsyncLoadAsset(const std::string& metaFilePath, const std::string& fileName, bool isDDS = false);
	void LoadAsset(const std::string& metaFilePath, const std::string& fileName, bool isDDS = false);

	// Asynchronously unload asset from memory
	void AsyncUnloadAsset(const std::string& assetGUID);
	void UnloadAsset(const std::string& assetGUID);

	// Asynchronously update asset in memory
	void AsyncUpdateAsset(const std::string& metaFilePath, const std::string& assetGUID);
	void UpdateAsset(const std::string& metaFilePath, const std::string& assetGUID);

	// Generate GUID for meta files
	std::string GenerateGUID(const std::string& fileName);

	// Create meta file for the asset file
	void CreateMetaFile(const std::string& fileName, const std::string& filePath, const std::string& fileType);

	// Deserialize from the meta file to retrieve asset data
	void DeserializeAssetMeta(const std::string& filePath, const std::string& fileName, bool isDDS = false);

	// Addition of files during engine runtime
	void FileAddProtocol(const std::string& filePath, const std::string& fileName, const std::string& fileExtension);

	// Removing of files during engine runtime
	void FileRemoveProtocol(const std::string& filePath, const std::string& fileName, const std::string& fileExtension);

	// Updating of files during runtime
	void FileUpdateProtocol(const std::string& filePath, const std::string& fileExtension);

	// FileWatching system using event callbacks
	void CallbackFileModified(FileModifiedEvent* pEvent);

	// Get the GUID of the asset using event callbacks
	void CallbackGetAssetGUID(GetAssetEvent* pEvent);

	// Get the dropped files using event callbacks
	void CallbackDroppedAsset(DropAssetsEvent* pEvent);
};