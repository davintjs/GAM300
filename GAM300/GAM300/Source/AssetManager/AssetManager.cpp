#include "Precompiled.h"
#include "AssetManager/AssetManager.h"
#include "Utilities/ThreadPool.h"
#include "Core/EventsManager.h"
#include "Core/FileTypes.h"

// Currently only loads geom files, future requires editing to support other file types of assets

void AssetManager::Init()
{
	if (!std::filesystem::exists(AssetPath))
	{
		std::cout << "Check if proper assets filepath exists!" << std::endl;
		exit(0);
	}

	//EVENT SUBSCRIPTIONS
	EVENTS.Subscribe(this, &AssetManager::CallbackFileModified);
	
	std::string subFilePath{};
	// Models will have more folders, the others will be categorized based on the asset type (Character, environment, background)
	for (const auto& dir : std::filesystem::recursive_directory_iterator(AssetPath))
	{
		subFilePath = dir.path().generic_string();
		std::string subFilePathMeta = subFilePath;
		std::string fileType{};

		for (size_t i = subFilePath.find_last_of('.') + 1; i != strlen(subFilePath.c_str()); ++i)
		{
			fileType += subFilePath[i];
		}

		if (strcmp(fileType.c_str(), "geom")) // Skip if not geom / ...
		{
			continue;
		}

		// Removing extension to add .meta extension
		subFilePathMeta.erase(subFilePathMeta.find_last_of('.'), strlen(fileType.c_str()) + 1);
		subFilePathMeta += ".meta";

		if (!std::filesystem::exists(subFilePathMeta))
		{
			std::string fileName{};
			for (size_t j = subFilePath.find_last_of('/') + 1; j != subFilePath.find_last_of('.'); ++j)
			{
				fileName += subFilePath[j];
			}
			CreateMetaFile(fileName, subFilePathMeta, fileType);
		}

		// Add this asset file time to our tracking vector of last write time (Only if geom / ... file)
		this->mTotalAssets.mAssetsTime.push_back(std::filesystem::last_write_time(dir));
		
		// Deserialize from meta file and load the asset asynchronously
		this->AsyncLoadAsset(subFilePathMeta);
	}

	//SceneManager::Instance().GetCurrentScene(); // Should be loading according to scene, but temporarily not
}

// For run time update of files
void AssetManager::Update(float dt)
{
	//Change this to an assert
	if (!std::filesystem::exists(AssetPath))
	{
		std::cout << "Check if proper assets filepath exists!" << std::endl;
		exit(0);
	}

	std::vector<std::filesystem::file_time_type> temp{};

	std::string subFilePath{};
	for (const auto& it : std::filesystem::recursive_directory_iterator(AssetPath))
	{
		subFilePath = it.path().generic_string();
		std::string fileType{};

		for (size_t i = subFilePath.find_last_of('.') + 1; i != strlen(subFilePath.c_str()); ++i)
		{
			fileType += subFilePath[i];
		}

		if (strcmp(fileType.c_str(), "geom")) // Skip if not geom / ...
		{
			continue;
		}
		temp.push_back(it.last_write_time()); // For comparison with our assets' last write time
	}

	if (temp.size() != this->mTotalAssets.mAssetsTime.size()) // File was added or removed from folder
	{
		if (temp.size() > this->mTotalAssets.mAssetsTime.size()) // File added
		{
			FileAddProtocol();
		}
		else // File removed
		{
			FileRemoveProtocol();
		}
		this->mTotalAssets.mAssetsTime = temp; // Update vector of time to most current
	}
	else // No file added to folder, but check for update of last write time of existing files
	{
		for (int i = 0; i < this->mTotalAssets.mAssetsTime.size(); ++i)
		{
			if (this->mTotalAssets.mAssetsTime[i] != temp[i])
			{
				FileUpdateProtocol(); // Update the file data in memory
				this->mTotalAssets.mAssetsTime = temp; // Update vector of time to most current
				break; // Skip the rest as all has been updated
			}
		}
	}
}

void AssetManager::Exit()
{

}

// Multi-threaded loading of assets
void AssetManager::AsyncLoadAsset(const std::string& metaFilePath)
{
	THREADS.EnqueueTask([this, metaFilePath] { LoadAsset(metaFilePath); });
}

void AssetManager::LoadAsset(const std::string& metaFilePath)
{
	ACQUIRE_SCOPED_LOCK("Assets");
	DeserializeAssetMeta(metaFilePath);
}

// Multi-threaded unloading of assets
void AssetManager::AsyncUnloadAsset(const std::string& assetGUID)
{
	THREADS.EnqueueTask([this, assetGUID] { UnloadAsset(assetGUID); });
}

void AssetManager::UnloadAsset(const std::string& assetGUID)
{
	ACQUIRE_SCOPED_LOCK("Assets");
	mTotalAssets.mFilesData.erase(assetGUID);
	std::cout << "Done removing file from memory!" << std::endl;
}

// Multi-threaded unloading of assets
void AssetManager::AsyncUpdateAsset(const std::string& assetPath, const std::string& assetGUID)
{
	THREADS.EnqueueTask([this, assetPath, assetGUID] { UpdateAsset(assetPath, assetGUID); });
}

void AssetManager::UpdateAsset(const std::string& assetPath, const std::string& assetGUID)
{
	ACQUIRE_SCOPED_LOCK("Assets");

	mTotalAssets.mFilesData[assetGUID].first = std::filesystem::last_write_time(std::filesystem::directory_entry(assetPath)); // Update the last write time

	std::ifstream inputFile(assetPath.c_str());
	if (!inputFile)
	{
		std::cout << "Error opening file to update asset in memory!" << std::endl;
		exit(0);
	}

	std::vector<char> buff(std::istreambuf_iterator<char>(inputFile), {});
	mTotalAssets.mFilesData[assetGUID].second = std::move(buff); // Update the data in memory

	std::cout << "Done updating file in memory!" << std::endl;

	inputFile.close();
}

// Get a loaded asset
const std::vector<char>& AssetManager::GetAsset(const std::string& assetGUID)
{
	auto func =
	[this, &assetGUID] // Wait if the asset is not loaded yet
	{
		return (mTotalAssets.mFilesData.find(assetGUID) != mTotalAssets.mFilesData.end());
	};
	ACQUIRE_UNIQUE_LOCK
	(
		"Assets", func
	);

	return mTotalAssets.mFilesData[assetGUID].second;
}

std::string AssetManager::GenerateGUID(const std::string& fileName)
{
	std::stringstream stream{};
	for (size_t i = 0; i < fileName.length(); ++i)
	{
		int asc = static_cast<int>(fileName[i]); // Convert from char to int first
		stream << std::hex << asc; // Convert to hexadecimal
	}

	std::uniform_real_distribution<double> distribution(1, 1000);
	std::random_device rd;
	std::default_random_engine generator(rd());
	int number = static_cast<int>(distribution(generator));
	stream << std::hex << number;

	std::string GUID(stream.str()); // Concat the string of hex
	return GUID;
}

void AssetManager::CreateMetaFile(const std::string& fileName, const std::string& filePath, const std::string& fileType)
{
	std::string fileNameNum = GenerateGUID(fileName);
	std::string fileAssetPath = filePath;
	fileAssetPath.erase(fileAssetPath.find_last_of('.'), strlen("meta") + 1);
	fileAssetPath += '.' + fileType;

	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

	writer.StartObject();
	writer.String("GUID");
	writer.String(fileNameNum.c_str());
	writer.String("FileType");
	writer.String(fileType.c_str());
	writer.String("FileAssetPath");
	writer.String(fileAssetPath.c_str());
	writer.EndObject();

	std::ofstream ofs(filePath);
	ofs << sb.GetString();
	ofs.flush();
	ofs.close();

	return;
}

void AssetManager::DeserializeAssetMeta(const std::string& filePath)
{
	std::ifstream ifs(filePath);
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	ifs.close();

	rapidjson::Document doc;
	const std::string data(buffer.str());
	doc.Parse(data.c_str());

	const std::string assetPath = doc["FileAssetPath"].GetString();
	const std::string GUIDofAsset = doc["GUID"].GetString();

	std::ifstream inputFile(assetPath.c_str());
	if (!inputFile)
	{
		std::cout << "Error opening file to load asset into memory!" << std::endl;
		exit(0);
	}

	std::vector<char> buff(std::istreambuf_iterator<char>(inputFile), {});

	std::filesystem::directory_entry path(assetPath);
	std::filesystem::file_time_type pathTime = std::filesystem::last_write_time(path);
	this->mTotalAssets.mFilesData.insert(std::make_pair(GUIDofAsset, std::make_pair(pathTime, buff)));

	inputFile.close();
}

void AssetManager::FileAddProtocol()
{
	std::string subFilePath{};
	for (const auto& dir : std::filesystem::recursive_directory_iterator(AssetPath))
	{
		subFilePath = dir.path().generic_string();
		std::string subFilePathMeta = subFilePath;
		std::string fileType{};

		for (size_t i = subFilePath.find_last_of('.') + 1; i != strlen(subFilePath.c_str()); ++i)
		{
			fileType += subFilePath[i];
		}

		// Check if this file is new by searching for its meta file
		if (strcmp(fileType.c_str(), "geom")) // Skip if not geom / ...
		{
			continue;
		}

		// Removing extension to add .meta extension
		subFilePathMeta.erase(subFilePathMeta.find_last_of('.'), strlen(fileType.c_str()) + 1);
		subFilePathMeta += ".meta";

		if (!std::filesystem::exists(subFilePathMeta))
		{
			std::string fileName{};
			// Meta file does not exist, so this asset is new
			for (size_t j = subFilePath.find_last_of('/') + 1; j != subFilePath.find_last_of('.'); ++j)
			{
				fileName += subFilePath[j];
			}
			CreateMetaFile(fileName, subFilePathMeta, fileType);
		}

		// Deserialize from meta file and load the asset asynchronously
		this->AsyncLoadAsset(subFilePathMeta);
	}
}

void AssetManager::FileRemoveProtocol()
{
	std::string subFilePath{};
	for (const auto& dir : std::filesystem::recursive_directory_iterator(AssetPath))
	{
		subFilePath = dir.path().generic_string();
		std::string fileType{};

		for (size_t i = subFilePath.find_last_of('.') + 1; i != strlen(subFilePath.c_str()); ++i)
		{
			fileType += subFilePath[i];
		}

		if (!strcmp(fileType.c_str(), "meta"))
		{
			// Check if the asset associated with this meta file still exists in the asset folder
			std::ifstream ifs(subFilePath);
			std::stringstream buffer;
			buffer << ifs.rdbuf();
			ifs.close();

			rapidjson::Document doc;
			const std::string data(buffer.str());
			doc.Parse(data.c_str());

			std::string assetPath = doc["FileAssetPath"].GetString();
			std::string tempGUID = doc["GUID"].GetString();

			if (!std::filesystem::exists(assetPath)) // If no longer in folder, delete the meta file and also remove from memory
			{
				std::filesystem::remove(subFilePath); // Delete meta file
				this->AsyncUnloadAsset(tempGUID); // Unload asset from memory
			}
		}
	}
}

void AssetManager::FileUpdateProtocol()
{
	std::string subFilePath{};
	for (const auto& dir : std::filesystem::recursive_directory_iterator(AssetPath))
	{
		subFilePath = dir.path().generic_string();
		std::string fileType{};

		for (size_t i = subFilePath.find_last_of('.') + 1; i != strlen(subFilePath.c_str()); ++i)
		{
			fileType += subFilePath[i];
		}

		// Find the last write time of this file corresponding to it in memory
		if (!strcmp(fileType.c_str(), "meta"))
		{
			// Get the asset file path and GUID from meta file of the asset
			std::ifstream ifs(subFilePath);
			std::stringstream buffer;
			buffer << ifs.rdbuf();
			ifs.close();

			rapidjson::Document doc;
			const std::string data(buffer.str());
			doc.Parse(data.c_str());

			std::string assetPath = doc["FileAssetPath"].GetString();
			const std::string tempGUID = doc["GUID"].GetString();
			if (mTotalAssets.mFilesData[tempGUID].first != std::filesystem::last_write_time(std::filesystem::path(assetPath)))
			{
				// The asset file associated with this meta file was updated
				mTotalAssets.mFilesData[tempGUID].second.clear(); // Remove the data in memory
				this->AsyncUpdateAsset(assetPath, tempGUID); // Add the new data into memory
			}
		}
	}
}


void AssetManager::CallbackFileModified(FileModifiedEvent* pEvent)
{
	namespace fs = std::filesystem;
	fs::path filePath{ pEvent->filePath};


	switch (pEvent->fileState)
	{
		case FileState::CREATED:
		{
			if (filePath.extension() == ".meta")
				return;
			//PRINT("CREATED ");		
			fs::path subFilePath = filePath.parent_path();
			fs::path subFilePathMeta = subFilePath.append(filePath.filename().string()+".meta");

			//PRINT("META: ", subFilePathMeta, '\n');

			//for (size_t i = subFilePath.find_last_of('.') + 1; i != strlen(subFilePath.c_str()); ++i)
			//{
			//	fileType += subFilePath[i];
			//}

			//if (!std::filesystem::exists(subFilePathMeta))
			//{
			//	CreateMetaFile(filePath.filename(), subFilePathMeta.string(), fileType);
			//}
			break;
		}
		case FileState::DELETED:
		{
			PRINT("DELETED ");
			break;
		}
		case FileState::MODIFIED:
		{
			PRINT("MODIFIED ");
			break;
		}
		case FileState::RENAMED_OLD:
		{
			PRINT("RENAMED_OLD ");
			break;
		}
		case FileState::RENAMED_NEW:
		{
			PRINT("RENAMED_NEW ");
			break;
		}
		default:
		{
			PRINT("UNDEFINED ");
			break;
		}

	}
	if (filePath.extension() == ".cs")
	{
		FileTypeModifiedEvent<FileType::SCRIPT> scriptModifiedEvent(filePath.stem().c_str(),pEvent->fileState);
		EVENTS.Publish(&scriptModifiedEvent);
	}
}