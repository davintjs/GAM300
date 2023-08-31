#include "Precompiled.h"
#include "AssetManager/AssetManager.h"

// Loads the required resources only for the first level
void AssetManager::Init()
{
	if (!std::filesystem::exists("Assets"))
	{
		std::cout << "Check if proper assets filepath exists!" << std::endl;
		exit(0);
	}
	
	// Store all last updated time of each file
	for (const auto& dir : std::filesystem::recursive_directory_iterator("Assets"))
	{
		this->mTotalAssets.mAssetsTime.push_back(std::filesystem::last_write_time(dir));
	}

	this->LoadAsset("Assets");

	//SceneManager::Instance().GetCurrentScene(); // Should be loading according to scene, but temporarily not
}

// For run time update of files
void AssetManager::Update()
{
	if (!std::filesystem::exists("Assets"))
	{
		std::cout << "Check if proper assets filepath exists!" << std::endl;
		exit(0);
	}

	std::vector<std::filesystem::file_time_type> temp{};

	for (const auto& it : std::filesystem::recursive_directory_iterator("Assets"))
	{
		temp.push_back(it.last_write_time());
	}

	if (temp.size() != this->mTotalAssets.mAssetsTime.size()) // New file was added to folder
	{
		this->mTotalAssets.mAssetsTime = temp; // Update vector of time to most current
		// Add the file into memory

	}
	else // No file added to folder, but check for update of last write time of existing files
	{
		for (int i = 0; i < this->mTotalAssets.mAssetsTime.size(); ++i)
		{
			if (this->mTotalAssets.mAssetsTime[i] != temp[i])
			{
				this->mTotalAssets.mAssetsTime[i] = temp[i]; // Update to the most current time changed
				// Update the file data in memory
			}
		}
	}
}

void AssetManager::Exit()
{

}

// Multi-threaded loading of assets
void AssetManager::LoadAsset(const std::string& filePath)
{
	std::lock_guard<std::mutex> mLock(mAssetMutex); // Mutex lock if opening the same file in multiple threads (Maybe no need)

	// Should move these string trimming to another function, then this function just for loading only
	if (!std::filesystem::exists(filePath))
	{
		std::cout << "Check if proper assets filepath exists!" << std::endl;
		exit(0);
	}

	std::string subFilePath{};
	for (const auto& dir : std::filesystem::directory_iterator(filePath))
	{
		subFilePath = dir.path().generic_string();
		std::string subFilePathMeta = subFilePath;
		std::string fileType{};
		std::string fileName{};

		for (size_t i = subFilePath.find_last_of('.') + 1; i != strlen(subFilePath.c_str()); ++i)
		{
			fileType += subFilePath[i];
		}

		if (!strcmp(fileType.c_str(), "meta")) // Skip this file if is meta file
		{
			continue;
		}
	}
	
	mAssetVariable.notify_all();
}

void AssetManager::AsyncLoadAsset(const std::string& assetPath)
{
	AssetThread.EnqueueTask([this, assetPath] { LoadAsset(assetPath); });
}

// Get a loaded asset
const std::vector<char>& AssetManager::GetAsset(const int& assetGUID)
{
	std::unique_lock<std::mutex> mLock(mAssetMutex);
	mAssetVariable.wait(mLock, [this, &assetGUID] // Wait if the asset is not loaded yet
		{
			return (mTotalAssets.mFilesData.find(assetGUID) != mTotalAssets.mFilesData.end());
		});

	return mTotalAssets.mFilesData[assetGUID].second;
}