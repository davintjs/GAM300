/*!***************************************************************************************
\file			TextureCompiler.cpp
\project
\author         Euphrasia Theophelia Tan Ee Mun

\par			Course: GAM300
\date           08/12/2023

\brief
	This file contains the definition of Texture Compiler that includes:
	1.Loading of textures and converting it into dds file format

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"

#include "TextureCompiler.h"
#include <DirectXTex.h>

void LoadTexture(const std::filesystem::path& _filePath)
{
	PRINT("Compiling textures...\n");

	// Initialize COM
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr)) {
		// Handle initialization failure
		return;
	}

	//std::string subFilePathDesc = subFilePath;
	//std::string textureFilePath = subFilePath;

	//Replace extension to be dds
	std::filesystem::path ddsPath{ _filePath };
	ddsPath.replace_extension(".dds");
	PRINT("Compiling ", ddsPath, "...\n");

	// Load PNG image into a ScratchImage
	DirectX::ScratchImage image;

	hr = DirectX::LoadFromWICFile(_filePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
	if (FAILED(hr)) {
		// Handle image loading failure
		PRINT("Loading texture unsuccessful.\n");
		//CoUninitialize(); // Cleanup COM
		//return hr;
		return;
	}

	DirectX::ScratchImage bcImage;
	hr = Compress(image.GetImages(), image.GetImageCount(),
		image.GetMetadata(), DXGI_FORMAT_BC1_UNORM,
		DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT,
		bcImage);
	if (FAILED(hr)) {
		// Handle DDS saving failure
		PRINT("Compression unsuccessful.\n");
		//CoUninitialize(); // Cleanup COM
		//return hr;
		return;
	}

	// Save the loaded image as DDS
	hr = DirectX::SaveToDDSFile(bcImage.GetImages(), bcImage.GetImageCount(), bcImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, ddsPath.c_str());
	if (FAILED(hr)) {
		// Handle DDS saving failure
		PRINT("Creating DDSFile unsuccessful.\n");
		//CoUninitialize(); // Cleanup COM
		//return hr;
		return;
	}
	PRINT("Done!\n");

	PRINT("Conversion from PNG / JPG to DDS successful.\n");

	// Cleanup COM
	CoUninitialize();

	PRINT("Finished compiling all textures!\n");
}