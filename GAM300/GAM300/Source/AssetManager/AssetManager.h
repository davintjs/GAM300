#pragma once

#include <unordered_map>
#include "Core/SystemInterface.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "Core/Events.h"

// GUID, last file update time, data
struct Asset
{
	std::vector<std::filesystem::file_time_type> mAssetsTime;
	std::unordered_map<std::string, std::pair<std::filesystem::file_time_type, std::vector<char>>> mFilesData;
};

ENGINE_SYSTEM(AssetManager)
{
public:
	const std::vector<char>& GetAsset(const std::string& assetGUID);

private:
	const std::string AssetPath = "Assets";

	void Init();
	void Update(float dt);
	void Exit();

	// Asset stuff
	Asset mTotalAssets;

	// Helper functions
	void AsyncLoadAsset(const std::string& metaFilePath);
	void LoadAsset(const std::string& metaFilePath);
	void AsyncUnloadAsset(const std::string& assetGUID);
	void UnloadAsset(const std::string& assetGUID);
	void AsyncUpdateAsset(const std::string& metaFilePath, const std::string& assetGUID);
	void UpdateAsset(const std::string& metaFilePath, const std::string& assetGUID);


	std::string GenerateGUID(const std::string& fileName);
	void CreateMetaFile(const std::string& fileName, const std::string& filePath, const std::string& fileType);
	void DeserializeAssetMeta(const std::string& filePath);

	void FileAddProtocol();
	void FileRemoveProtocol();
	void FileUpdateProtocol();

	//EVENT CALLBACKS
	void CallbackFileModified(FileModifiedEvent * pEvent);
};