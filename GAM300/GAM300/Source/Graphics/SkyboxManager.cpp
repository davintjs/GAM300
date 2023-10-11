/*!***************************************************************************************
\file			SkyboxManager.cpp
\project
\author

\par			Course: GAM300
\date           11/10/2023

\brief
	This file contains the definitions of Graphics Skybox that includes:
	1.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include "AssetManager/AssetManager.h"

void SkyboxManager::Init()
{
	skyBoxModel = new SkyBox;
	CreateSkybox("skybox_default_top");
}

void SkyboxManager::Update(float)
{
	
}

void SkyboxManager::CreateSkybox(const std::string& _name)
{
	skyboxTex = TextureManager.GetTexture(AssetManager::Instance().GetAssetGUID(_name));
	skyBoxModel->SkyBoxinit();
	skyBoxModel->shader = SHADER.GetShader(SKYBOX);
}

void SkyboxManager::Draw()
{
	glDepthFunc(GL_LEQUAL);
	skyBoxModel->SkyBoxDraw(skyboxTex);
	glDepthFunc(GL_LESS);
}

void SkyboxManager::Exit()
{
	delete skyBoxModel;
}