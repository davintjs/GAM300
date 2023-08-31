#pragma once

#include <unordered_map>
#include "Core/EngineCore.h"
#include "AssetManager/ThreadPool.h"

// GUID, last file update time, data
struct Asset
{
	std::vector<std::filesystem::file_time_type> mAssetsTime;
	std::unordered_map<int, std::pair<std::filesystem::file_time_type, std::vector<char>>> mFilesData;
};

ENGINE_SYSTEM(AssetManager)
{
public:
	void Init();
	void Update();
	void Exit();

	void AsyncLoadAsset(const std::string & assetPath);
	const std::vector<char>& GetAsset(const int& assetGUID);

private:
	// Thread stuff
	ThreadPool AssetThread;
	std::mutex mAssetMutex;
	std::condition_variable mAssetVariable;

	// Asset stuff
	Asset mTotalAssets;

	void LoadAsset(const std::string & filePath);
	void CreateMetaFile(const std::string & filePath, const std::string & fileType);
};