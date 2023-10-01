#include "Precompiled.h"
#include "AssetManager/AssetManager.h"
#include "Utilities/ThreadPool.h"
#include "Core/EventsManager.h"
#include "Core/FileTypes.h"

void AssetManager::Init()
{
	E_ASSERT(std::filesystem::exists(AssetPath), "Check if proper assets filepath exists!");

	//EVENT SUBSCRIPTIONS
	EVENTS.Subscribe(this, &AssetManager::CallbackFileModified);
	EVENTS.Subscribe(this, &AssetManager::CallbackGetAssetGUID);
	
	std::string subFilePath{};
	// Models will have more folders, the others will be categorized based on the asset type (Character, environment, background)
	for (const auto& dir : std::filesystem::recursive_directory_iterator(AssetPath))
	{
		subFilePath = dir.path().generic_string();
		std::string subFilePathMeta = subFilePath, assetPath = subFilePath;
		std::string fileType{};
		std::string fileName{};
		
		if (!dir.is_directory())
		{
			// Check if is file with no extension
			auto check = subFilePath.find_last_of('.');
			E_ASSERT(check != std::string::npos, "File with no extension found! Remove it from the assets folder.");

			for (size_t i = check + 1; i != strlen(subFilePath.c_str()); ++i)
			{
				fileType += subFilePath[i];
			}
		}

		if (dir.is_directory())
		{
			fileName = std::filesystem::path(dir).filename().generic_string();
		}
		else
		{
			for (size_t j = subFilePath.find_last_of('/') + 1; j != subFilePath.find_last_of('.'); ++j)
			{
				fileName += subFilePath[j];
			}
		}

		// Add into file extensions list
		mTotalAssets.mExtensionFiles[fileType].push_back(fileName);

		if (!strcmp(fileType.c_str(), "meta") || !strcmp(fileType.c_str(), "fbx") || !strcmp(fileType.c_str(), "desc")) // Skip if meta / fbx / desc file
		{
			continue;
		}
		// Removing extension to add .meta extension
		if (dir.is_directory())
		{
			subFilePathMeta += ".meta";
		}
		else
		{
			subFilePathMeta.erase(subFilePathMeta.find_last_of('.'), strlen(fileType.c_str()) + 1);
			subFilePathMeta += ".meta";
		}
		if (!std::filesystem::exists(subFilePathMeta))
		{
			CreateMetaFile(fileName, subFilePathMeta, fileType);
			mTotalAssets.mExtensionFiles["meta"].push_back(fileName);
		}

		// Mark meta files as hidden files
		std::wstring wideStr = std::wstring(subFilePathMeta.begin(), subFilePathMeta.end());
		const wchar_t* fileLPCWSTR = wideStr.c_str();
		int attribute = GetFileAttributes(fileLPCWSTR);
		if ((attribute & FILE_ATTRIBUTE_HIDDEN) == 0)
		{
			SetFileAttributes(fileLPCWSTR, attribute | FILE_ATTRIBUTE_HIDDEN);
		}

		// We still want to create the meta files for these that's why we skip here instead of above
		if (!strcmp(fileType.c_str(), "jpg") || !strcmp(fileType.c_str(), "png"))
		{
			continue;
		}

		// Deserialize from meta file and load the asset asynchronously
		if (!dir.is_directory())
		{
			this->AsyncLoadAsset(subFilePathMeta, fileName);

			if (!strcmp(fileType.c_str(), "dds")) // if dds ...
			{
				//this->AsyncLoadAsset(subFilePathMeta, fileName, true);
				std::string filetype = assetPath/* + ".dds"*/;
				TextureManager.AddTexture(assetPath.c_str(), GetAssetGUID(fileName));

			}
			else if (!strcmp(fileType.c_str(), "geom"))
			{
				MeshManager.GetGeomFromFiles(subFilePath, fileName);
			}
			else if (!strcmp(fileType.c_str(), "mp3") || !strcmp(fileType.c_str(), "wav")) {
				AUDIOMANAGER.AddMusic(subFilePath.c_str(), fileName);
				AUDIOMANAGER.AddSFX(subFilePath.c_str(), fileName);
				//AUDIOMANAGER.AddMusic(assetPath.c_str(), fileName);
			}
		}
	}

	MeshManager.Init();
}

// For run time update of files
void AssetManager::Update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
}

void AssetManager::Exit()
{

}

// Multi-threaded loading of assets
void AssetManager::AsyncLoadAsset(const std::string& metaFilePath, const std::string& fileName, bool isDDS)
{
	THREADS.EnqueueTask([this, metaFilePath, fileName, isDDS] { LoadAsset(metaFilePath, fileName, isDDS); });
}

void AssetManager::LoadAsset(const std::string& metaFilePath, const std::string& fileName, bool isDDS)
{
	ACQUIRE_SCOPED_LOCK(Assets);
	DeserializeAssetMeta(metaFilePath, fileName, isDDS);
}

// Multi-threaded unloading of assets
void AssetManager::AsyncUnloadAsset(const std::string& assetGUID)
{
	THREADS.EnqueueTask([this, assetGUID] { UnloadAsset(assetGUID); });
}

void AssetManager::UnloadAsset(const std::string& assetGUID)
{
	//May need to unique lock this
	ACQUIRE_SCOPED_LOCK(Assets);
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
	ACQUIRE_SCOPED_LOCK(Assets);

	mTotalAssets.mFilesData[assetGUID].mFileTime = std::filesystem::last_write_time(std::filesystem::directory_entry(assetPath)); // Update the last write time

	std::ifstream inputFile(assetPath.c_str());
	E_ASSERT(inputFile, "Error opening file to update asset in memory!");

	std::vector<char> buff(std::istreambuf_iterator<char>(inputFile), {});
	mTotalAssets.mFilesData[assetGUID].mData = std::move(buff); // Update the data in memory

	std::cout << "Done updating file in memory!" << assetPath << std::endl;

	inputFile.close();
}

// Get a loaded asset
const std::vector<char>& AssetManager::GetAsset(const std::string& fileName)
{
	std::string data{};
	auto func =
	[this, &fileName, &data] // Wait if the asset is not loaded yet
	{
		for (const auto& [Key, Val] : mTotalAssets.mFilesData)
		{
			if (Val.mFileName == fileName)
			{
				data = Key;
				return true;
			}
		}
		return false;
	};
	ACQUIRE_UNIQUE_LOCK
	(
		Assets, func
	);

	return mTotalAssets.mFilesData[data].mData;
}

//// Get a loaded asset GUID
std::string AssetManager::GetAssetGUID(const std::string& fileName)
{
	std::string data{};
	auto func =
		[this, &fileName, &data] // wait if the asset is not loaded yet
	{
		for (const auto& [key, val] : mTotalAssets.mFilesData)
		{
			if (val.mFileName == fileName)
			{
				data = key;
				return true;
			}
		}
		return false;
	};
	ACQUIRE_UNIQUE_LOCK
	(
		Assets, func
	);

	return data;
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
	if (strcmp(fileType.c_str(), ""))
	{
		fileAssetPath += fileType;
	}
	
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

void AssetManager::DeserializeAssetMeta(const std::string& filePath, const std::string& fileName, bool /*isDDS*/)
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
	E_ASSERT(inputFile, "Error opening file to load asset into memory!");

	std::vector<char> buff(std::istreambuf_iterator<char>(inputFile), {});

	std::filesystem::directory_entry path(assetPath);
	std::filesystem::file_time_type pathTime = std::filesystem::last_write_time(path);

	FileInfo tempFI(pathTime, fileName, buff);
	this->mTotalAssets.mFilesData.insert(std::make_pair(GUIDofAsset, tempFI));

	inputFile.close();
}

void AssetManager::FileAddProtocol(const std::string& filePath, const std::string& fileName, const std::string& fileExtension)
{
	ACQUIRE_SCOPED_LOCK(Assets);

	// Remove extension in filePath and add .meta
	std::string filePathMeta = "Assets/" + filePath;
	filePathMeta.erase(filePathMeta.find_last_of('.'), strlen(fileExtension.c_str()) + 1);
	filePathMeta += ".meta";

	// Remove the . in fileExtension
	std::string fileExtensionEdited = fileExtension;
	fileExtensionEdited.erase(fileExtension.find('.'));

	CreateMetaFile(fileName, filePathMeta, fileExtension);

	// Mark meta files as hidden files
	std::wstring wideStr = std::wstring(filePathMeta.begin(), filePathMeta.end());
	const wchar_t* fileLPCWSTR = wideStr.c_str();
	int attribute = GetFileAttributes(fileLPCWSTR);
	if ((attribute & FILE_ATTRIBUTE_HIDDEN) == 0)
	{
		SetFileAttributes(fileLPCWSTR, attribute | FILE_ATTRIBUTE_HIDDEN);
	}

	mTotalAssets.mExtensionFiles["meta"].push_back(fileName); // Meta file
	mTotalAssets.mExtensionFiles[fileExtensionEdited].push_back(fileName); // File name

	if (fileExtension == ".jpg" || fileExtension == ".png")
	{
		return; // Do not load jpg and png files as we want to load the dds version
	}

	// Deserialize from meta file and load the asset asynchronously
	this->AsyncLoadAsset(filePathMeta, fileName);
}

void AssetManager::FileRemoveProtocol(const std::string& filePath, const std::string& fileName, const std::string& fileExtension)
{
	ACQUIRE_SCOPED_LOCK(Assets);

	std::string filePathMeta = "Assets/" + filePath;
	filePathMeta.erase(filePathMeta.find_last_of('.'), strlen(fileExtension.c_str()) + 1);
	filePathMeta += ".meta";

	std::ifstream ifs(filePathMeta);
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	ifs.close();

	rapidjson::Document doc;
	const std::string data(buffer.str());
	doc.Parse(data.c_str());

	std::string assetPath = doc["FileAssetPath"].GetString();
	std::string assetType = doc["FileType"].GetString();
	std::string tempGUID = doc["GUID"].GetString();

	std::filesystem::remove(filePathMeta); // Delete meta file

	mTotalAssets.mExtensionFiles["meta"].erase(std::remove(mTotalAssets.mExtensionFiles["meta"].begin(), mTotalAssets.mExtensionFiles["meta"].end(), fileName), mTotalAssets.mExtensionFiles["meta"].end()); // Meta file removal
	mTotalAssets.mExtensionFiles[assetType].erase(std::remove(mTotalAssets.mExtensionFiles[assetType].begin(), mTotalAssets.mExtensionFiles[assetType].end(), fileName), mTotalAssets.mExtensionFiles[assetType].end()); // File name removal

	this->AsyncUnloadAsset(tempGUID); // Unload asset from memory
}

// Change meta file name (And path to the updated name)
void AssetManager::FileUpdateProtocol(const std::string& filePath, const std::string& fileExtension)
{
	ACQUIRE_SCOPED_LOCK(Assets);

	// Remove extension in filePath and add .meta
	std::string filePathMeta = "Assets/" + filePath;
	filePathMeta.erase(filePathMeta.find_last_of('.'), strlen(fileExtension.c_str()) + 1);
	filePathMeta += ".meta";

	// Get the asset file path and GUID from meta file of the asset
	std::ifstream ifs(filePathMeta);
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	ifs.close();

	rapidjson::Document doc;
	const std::string data(buffer.str());
	doc.Parse(data.c_str());

	std::string assetPath = doc["FileAssetPath"].GetString();
	const std::string tempGUID = doc["GUID"].GetString();
	if (!std::filesystem::is_directory(assetPath) && mTotalAssets.mFilesData[tempGUID].mFileTime != std::filesystem::last_write_time(std::filesystem::path(assetPath)))
	{
		// The asset file associated with this meta file was updated
		mTotalAssets.mFilesData[tempGUID].mData.clear(); // Remove the data in memory
		this->AsyncUpdateAsset(assetPath, tempGUID); // Add the new data into memory
	}
}


void AssetManager::CallbackFileModified(FileModifiedEvent* pEvent)
{
	namespace fs = std::filesystem;
	fs::path filePath{ pEvent->filePath};

	if (filePath.empty())
	{
		PRINT("EMPTY!\n");
	}

	std::string fileName = filePath.filename().generic_string();
	std::string fileExtension = filePath.extension().generic_string();
	std::string filePathEdited = filePath.generic_string();
	std::replace(filePathEdited.begin(), filePathEdited.end(), '\\', '/');


	if (fileExtension == ".fbx") // Call the fbx compiler
	{
		const char* command{ "Compiler.exe" };

		system(command);

		return; // Return as file watcher will detect an addition of geom file and desc
	}
	else if (fileExtension == ".png" || fileExtension == ".jpg") // Call the texture compiler
	{
		const char* command{ "TextureCompiler.exe" };

		system(command);
	}

	if (fileExtension == ".meta" || 
		filePath.string().find("~") != std::string::npos || 
		fileExtension == ".desc" ||
		fileExtension == "")
	{
		return;
	}

	fileName.erase(fileName.find_last_of('.'), strlen(fileExtension.c_str()) + 1);
	
	switch (pEvent->fileState)
	{
		case FileState::CREATED:
		{
			PRINT("CREATED ");
			FileAddProtocol(filePathEdited, fileName, fileExtension);
			break;
		}
		case FileState::DELETED:
		{
			PRINT("DELETED ");
			FileRemoveProtocol(filePathEdited, fileName, fileExtension);
			break;
		}
		case FileState::MODIFIED:
		{
			FileUpdateProtocol(filePathEdited, fileExtension);
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
			break;
		}

	}
	if (filePath.extension() == ".cs")
	{
		FileTypeModifiedEvent<FileType::SCRIPT> scriptModifiedEvent(filePath.stem().c_str(),pEvent->fileState);
		EVENTS.Publish(&scriptModifiedEvent);
	}
	PRINT(filePath.string(), "\n");
}

void AssetManager::CallbackGetAssetGUID(GetAssetEvent* pEvent)
{
	pEvent->guid = GetAssetGUID(pEvent->fileName);
}