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
	
	std::string subFilePath{};
	// Models will have more folders, the others will be categorized based on the asset type (Character, environment, background)
	for (const auto& dir : std::filesystem::recursive_directory_iterator("Assets"))
	{
		subFilePath = dir.path().generic_string();
		//if (!strcmp(subFilePath.c_str(), "Assets/Models")) // This is a folder that requires iteration
		//{
		//	
		//}
		std::string subFilePathMeta = subFilePath;
		std::string fileType{};
		std::string fileName{};

		for (size_t i = subFilePath.find_last_of('.') + 1; i != strlen(subFilePath.c_str()); ++i)
		{
			fileType += subFilePath[i];
		}

		if (strcmp(fileType.c_str(), "geom")) // Skip if not geom
		{
			continue;
		}

		for (size_t j = subFilePath.find_last_of('/') + 1; j != subFilePath.find_last_of('.'); ++j)
		{
			fileName += subFilePath[j];
		}

		// Remove extension only if is not geom file
		if (strcmp(fileType.c_str(), "geom"))
		{
			// Removing extension to add .meta extension
			subFilePathMeta.erase(subFilePathMeta.find_last_of('.'), strlen(fileType.c_str()) + 1);
			subFilePathMeta += ".meta";
		}
		else
		{
			subFilePathMeta += ".meta";
		}

		if (!std::filesystem::exists(subFilePathMeta))
		{
			CreateMetaFile(fileName, subFilePathMeta, fileType);
		}

		// Add this asset file time to our tracking vector of last write time
		this->mTotalAssets.mAssetsTime.push_back(std::filesystem::last_write_time(dir));
		
		// Deserialize from meta file and load the asset asynchronously
		this->AsyncLoadAsset(subFilePathMeta, fileName);
	}

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
void AssetManager::LoadAsset(const std::string& metaFilePath, const std::string& fileName)
{
	std::lock_guard<std::mutex> mLock(mAssetMutex); // Mutex lock if opening the same file in multiple threads (Maybe no need)
	DeserializeAssetMeta(metaFilePath, fileName);
	mAssetVariable.notify_all();
}

void AssetManager::AsyncLoadAsset(const std::string& metaFilePath, const std::string& fileName)
{
	AssetThread.EnqueueTask([this, metaFilePath, fileName] { LoadAsset(metaFilePath, fileName); });
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

	std::string GUID(stream.str()); // Concat the string of hex asc
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
	writer.Int64(std::stoi(fileNameNum));
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

void AssetManager::DeserializeAssetMeta(std::string filePath, std::string fileName)
{
	std::ifstream ifs(filePath);
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	ifs.close();

	rapidjson::Document doc;
	const std::string data(buffer.str());
	doc.Parse(data.c_str());

	std::string assetPath = doc["FileAssetPath"].GetString();
	const long long int GUIDofAsset = doc["GUID"].GetInt();

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