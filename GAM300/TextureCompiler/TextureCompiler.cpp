// TextureCompiler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "TextureCompiler.h"



int main() 
{
	std::cout << "Compiling textures..." << std::endl;

	// Initialize COM
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr)) {
		// Handle initialization failure
		return hr;
	}

	for (const auto& dir : std::filesystem::recursive_directory_iterator("../Assets/Textures"))
	{
		if (dir.symlink_status().type() == std::filesystem::file_type::directory) // Is a folder (All should be in folder)
		{
			for (const auto& _dir : std::filesystem::recursive_directory_iterator(dir)) // Looping through contents of the texture's folder
			{
				std::string subFilePath = _dir.path().generic_string();
				//std::string subFilePathDesc = subFilePath;
				//std::string textureFilePath = subFilePath;
				std::string fileType{};
				std::string fileName{};

				// Get the file type of the current file in this folder
				for (size_t i = subFilePath.find_last_of('.') + 1; i != strlen(subFilePath.c_str()); ++i)
				{
					fileType += subFilePath[i];
				}

				if (strcmp(fileType.c_str(), "png") == 0 || strcmp(fileType.c_str(), "jpg") == 0)
				{

					// Reaching here means we are at the png / jpg file

					// Get file name
					for (size_t j = subFilePath.find_last_of('/') + 1; j != subFilePath.find_last_of('.'); ++j)
					{
						fileName += subFilePath[j];
					}
					std::cout << "Compiling " << fileName << "...";



					// future optimisation, check if the dds ver of the pg/jpg alr exists


					// Specify the input PNG file and output DDS file
					std::wstring textureFilePath = std::wstring(subFilePath.begin(), subFilePath.end()).c_str();
					const wchar_t* pngFileName = textureFilePath.c_str();   // Replace with your PNG file path

					std::string outputFilePath("../Assets/Resources/" + fileName + ".dds");
					std::wstring outputTextureFilePath = std::wstring(outputFilePath.begin(), outputFilePath.end()).c_str();
					const wchar_t* ddsFileName = outputTextureFilePath.c_str(); // Replace with your desired output DDS file path

					// Load PNG image into a ScratchImage
					DirectX::ScratchImage image;

					hr = DirectX::LoadFromWICFile(pngFileName, DirectX::WIC_FLAGS_NONE, nullptr, image);
					if (FAILED(hr)) {
						// Handle image loading failure
						std::cout << "LoadFromWICFile unsuccessful." << std::endl;
						CoUninitialize(); // Cleanup COM
						return hr;
					}



					// Save the loaded image as DDS
					hr = DirectX::SaveToDDSFile(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::DDS_FLAGS_NONE, ddsFileName);
					if (FAILED(hr)) {
						// Handle DDS saving failure
						std::cout << "SaveToDDSFile unsuccessful." << std::endl;
						CoUninitialize(); // Cleanup COM
						return hr;
					}

					std::cout << " Done!" << std::endl;
				}

				// Skip this file if not png / jpg
				{
					continue;
				}
			}
		}
	}



	std::cout << "Conversion from PNG / JPG to DDS successful." << std::endl;

	// Cleanup COM
	CoUninitialize();

	return 0;
	
}