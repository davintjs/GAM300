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

#include "Graphics/TextureManager.h"
#include "Graphics/MeshManager.h"

void SkyboxManager::Init()
{
	CreateSkybox("Assets//Textures//SkyBox//skybox_default_top.dds");
}

void SkyboxManager::CreateSkybox(const fs::path& _name)
{
	skyboxTex = TextureManager.GetTexture(_name);
	skyBoxModel.SkyBoxinit();
	skyBoxModel.shader = SHADER.GetShader(SKYBOX);
}

void SkyboxManager::Draw(BaseCamera& _camera)
{
	glDepthFunc(GL_LEQUAL);
	skyBoxModel.SkyBoxDraw(skyboxTex, _camera);
	glDepthFunc(GL_LESS);
}