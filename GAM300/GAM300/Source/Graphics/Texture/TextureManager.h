/*!***************************************************************************************
\file			TextureManager.h
\project
\author         Euphrasia Theophelia Tan Ee Mun

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the Texture Manager and the declarations of its related functions.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#include "Core/SystemInterface.h"
#include "../gli-master/gli/gli.hpp"
#include "Graphics/glslshader.h"
#include "Utilities/GUID.h"
#include <Core/Events.h>
#include <AssetManager/AssetTypes.h>

#define TextureManager Texture_Manager::Instance()

struct BaseTexture 
{
	char const* path;
	GLuint textureID;
	glm::vec2 pixelDimension;
};

SINGLETON(Texture_Manager)
{
public:
	void Init();

	void Update(float dt);

	void Exit();

	// used in asset manager to add dds textures to the texture container
	void AddTexture(char const* Filename, const Engine::GUID<TextureAsset>& GUID);

	// uses GUID to retrieve a texture from the texture container
	GLuint GetTexture(const Engine::GUID<TextureAsset>& GUID);

	BaseTexture* GetBaseTexture(const Engine::GUID<TextureAsset>& GUID);

	GLuint GetTexture(const fs::path& filePath);

	// creates a texture and returns it to be stored in the texture container
	void CreateTexture(BaseTexture& _texture);

	GLuint CreateTexture(char const* Filename);

	// creates a skybox texture and returns it to be stored in the texture container
	GLuint CreateSkyboxTexture(char const* Filename);


	//Handle mesh adding here
	void CallbackTextureAssetLoaded(AssetLoadedEvent<TextureAsset>*pEvent);

	//Handle mesh removal here
	void CallbackTextureAssetUnloaded(AssetUnloadedEvent<TextureAsset>*pEvent);
private:

	std::unordered_map<Engine::GUID<TextureAsset>, BaseTexture> mTextureContainer; // GUID, <file name, GLuint>
	
};

