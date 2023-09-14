#pragma once

#include <unordered_map>
#include "Core/SystemInterface.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "Core/Events.h"
#include "Graphics/TextureManager.h"
#include "Graphics/MeshManager.h"
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

struct Asset
{
	std::unordered_map<std::string, FileInfo> mFilesData;
	std::unordered_map<std::string, std::vector<std::string>> mExtensionFiles; // File extension, file names (For Zac)
};

ENGINE_SYSTEM(AssetManager)
{
public:
	const std::vector<char>& GetAsset(const std::string& fileName);
	std::string GetAssetGUID(const std::string& fileName);

private:
	const std::string AssetPath = "Assets";
	bool FileAdded = false; // For the filewatcher (Adding file calls both add and modified so this is for me to bypass modified)

	void Init();
	void Update(float dt);
	void Exit();

	// Asset stuff
	Asset mTotalAssets;

	// Helper functions
	void AsyncLoadAsset(const std::string& metaFilePath, const std::string& fileName, bool isDDS = false);
	void LoadAsset(const std::string& metaFilePath, const std::string& fileName, bool isDDS = false);
	void AsyncUnloadAsset(const std::string& assetGUID);
	void UnloadAsset(const std::string& assetGUID);
	void AsyncUpdateAsset(const std::string& metaFilePath, const std::string& assetGUID);
	void UpdateAsset(const std::string& metaFilePath, const std::string& assetGUID);

	std::string GenerateGUID(const std::string& fileName);
	void CreateMetaFile(const std::string& fileName, const std::string& filePath, const std::string& fileType);
	void DeserializeAssetMeta(const std::string& filePath, const std::string& fileName, bool isDDS = false);

	void FileAddProtocol();
	void FileRemoveProtocol();
	void FileUpdateProtocol();

	// EVENT CALLBACKS
	void CallbackFileModified(FileModifiedEvent * pEvent);
};