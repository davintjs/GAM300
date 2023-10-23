// TextureCompiler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "TextureCompiler.h"


int main(int argc, char* argv[])
{
	std::cout << "Compiling textures..." << std::endl;

	// Initialize COM
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr)) {
		// Handle initialization failure
		return hr;
	}

	if (argc != 2)
		return -1;

	std::filesystem::path path{ argv[1] };

	// Get the file type of the current file
	if (path.extension() != ".png" && path.extension() != ".jpg")
		return -1;

	//std::string subFilePathDesc = subFilePath;
	//std::string textureFilePath = subFilePath;
	
	//Replace extension to be dds
	std::filesystem::path ddsPath{ path };
	ddsPath.replace_extension(".dds");
	std::cout << "Compiling " << ddsPath << "...";

	// Load PNG image into a ScratchImage
	DirectX::ScratchImage image;

	hr = DirectX::LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
	if (FAILED(hr)) {
		// Handle image loading failure
		std::cout << "Loading texture unsuccessful." << std::endl;
		//CoUninitialize(); // Cleanup COM
		//return hr;
		return -1;
	}

	DirectX::ScratchImage bcImage;
	hr = Compress(image.GetImages(), image.GetImageCount(),
		image.GetMetadata(), DXGI_FORMAT_BC1_UNORM,
		DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT,
		bcImage);
	if (FAILED(hr)) {
		// Handle DDS saving failure
		std::cout << "Compression unsuccessful." << std::endl;
		//CoUninitialize(); // Cleanup COM
		//return hr;
		return -1;
	}

	// Save the loaded image as DDS
	hr = DirectX::SaveToDDSFile(bcImage.GetImages(), bcImage.GetImageCount(), bcImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, ddsPath.c_str());
	if (FAILED(hr)) {
		// Handle DDS saving failure
		std::cout << "Creating DDSFile unsuccessful." << std::endl;
		//CoUninitialize(); // Cleanup COM
		//return hr;
		return -1;
	}
	std::cout << " Done!" << std::endl;

	std::cout << "Conversion from PNG / JPG to DDS successful." << std::endl;

	// Cleanup COM
	CoUninitialize();

	std::cout << "Finished compiling all textures!" << std::endl;

	return 0;
}