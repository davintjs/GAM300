#pragma once

#include <unordered_map>
#include "Core/SystemInterface.h"
#include "AssetManager/ThreadPool.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

// GUID, last file update time, data
struct Asset
{
	std::vector<std::filesystem::file_time_type> mAssetsTime;
	std::unordered_map<long long int, std::pair<std::filesystem::file_time_type, std::vector<char>>> mFilesData;
};

ENGINE_SYSTEM(AssetManager)
{
public:
	const std::vector<char>& GetAsset(const int& assetGUID);

private:
	void Init();
	void Update();
	void Exit();

	// Thread stuff
	ThreadPool AssetThread;
	std::mutex mAssetMutex;
	std::condition_variable mAssetVariable;

	// Asset stuff
	Asset mTotalAssets;

	// Helper functions
	void AsyncLoadAsset(const std::string & metaFilePath, const std::string & fileName);
	void LoadAsset(const std::string& metaFilePath, const std::string& fileName);

	std::string GenerateGUID(const std::string& fileName);
	void CreateMetaFile(const std::string& fileName, const std::string& filePath, const std::string& fileType);
	void DeserializeAssetMeta(std::string filePath, std::string fileName);
};