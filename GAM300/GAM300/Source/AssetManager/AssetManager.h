#pragma once

#include <unordered_map>
#include "Core/EngineCore.h"
#include "AssetManager/ThreadPool.h"

// GUID, last file update time, data
struct Asset
{
	std::vector<std::filesystem::file_time_type> mAssetsTime;

	std::unordered_map<int, std::pair<std::filesystem::file_time_type, std::string>> mJsonFiles;
	std::unordered_map<int, std::pair<std::filesystem::file_time_type, std::vector<char>>> mBinaryFiles;
};

ENGINE_SYSTEM(AssetManager)
{
public:
	void Init();
	void Update();
	void Exit();

private:
	// Thread stuff
	ThreadPool AssetThread;
	std::mutex mAssetMutex;
	std::condition_variable mAssetVariable;

	// Asset stuff
	Asset mTotalAssets;
}