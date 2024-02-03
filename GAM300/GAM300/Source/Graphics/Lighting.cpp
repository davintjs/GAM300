/*!***************************************************************************************
\file			Lighting.cpp
\project
\author			Euan Lim

\par			Course: GAM300
\date           11/10/2023

\brief
	This file contains the definitions of Graphics Lighting that includes:
	1. Computation for the light stuffs that will be given to the eventual rendering

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include "Scene/SceneManager.h"

//extern LightProperties spot_light_stuffs;
//extern LightProperties directional_light_stuffs;
//extern LightProperties point_light_stuffs;

void Lighting::Init()
{
	// Initalizing Shadow FBOs
	unsigned int shadowFBO, textureID;

	for (int i = 0; i < MAX_POINT_LIGHT_SHADOW; ++i)
	{
		FRAMEBUFFER.CreatePointLight(shadowFBO, textureID, SHADOW_WIDTH, SHADOW_HEIGHT);
		pointLightFBO.push_back({ shadowFBO, textureID });
	}

	for (int i = 0; i < MAX_SPOT_LIGHT_SHADOW; ++i)
	{
		FRAMEBUFFER.CreateDirectionalAndSpotLight(shadowFBO, textureID, SHADOW_WIDTH, SHADOW_HEIGHT);
		spotLightFBO.push_back({ shadowFBO, textureID });
	}

	for (int i = 0; i < MAX_DIRECTION_LIGHT_SHADOW; ++i)
	{
		FRAMEBUFFER.CreateDirectionalAndSpotLight(shadowFBO, textureID, SHADOW_WIDTH_DIRECTIONAL, SHADOW_HEIGHT_DIRECTIONAL);
		directionalLightFBO.push_back({ shadowFBO, textureID });
	}

	directionLightSources.resize(MAX_DIRECTION_LIGHT);
	pointLightSources.resize(MAX_POINT_LIGHT);
	spotLightSources.resize(MAX_SPOT_LIGHT);
}

void Lighting::Update(float)
{
	pointLightCount = 0;
	directionalLightCount = 0;
	spotLightCount = 0;
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();

	// Temporary Light stuff
	bool haveLight = false;
	for (LightSource& lightSource : currentScene.GetArray<LightSource>())
	{
		if (lightSource.state == DELETED)
			continue;

		
		Entity& entity{ currentScene.Get<Entity>(lightSource) };

		if (!currentScene.IsActive(entity))
			continue;

		haveLight = true;
		Transform& transform = currentScene.Get<Transform>(entity);


		if (lightSource.lightType == POINT_LIGHT)// Point Light
		{
			// Cull
			pointLightSources[pointLightCount].enableShadow = lightSource.enableShadow;
			pointLightSources[pointLightCount].lightpos = transform.GetGlobalTranslation();
			pointLightSources[pointLightCount].lightColor = lightSource.lightingColor;
			pointLightSources[pointLightCount].intensity = lightSource.intensity;

			// Replace the first light if the count is more than the engines max available lights
			pointLightCount = (pointLightCount >= MAX_POINT_LIGHT - 1) ? 0 : pointLightCount + 1;
		}


		else if (lightSource.lightType == DIRECTIONAL_LIGHT)// Directional Light - WIP
		{
			// Cull
			directionLightSources[directionalLightCount].enableShadow = lightSource.enableShadow;
			directionLightSources[directionalLightCount].lightpos = transform.GetGlobalTranslation();
			directionLightSources[directionalLightCount].lightColor = lightSource.lightingColor;
			directionLightSources[directionalLightCount].intensity = lightSource.intensity;

			glm::vec3 direction = glm::vec3(0.f, -1.f, 0.f);
			glm::vec3 rotation = transform.GetGlobalRotation();
			glm::mat4 rot = glm::toMat4(glm::quat(vec3(rotation)));

			rot *= glm::translate(glm::mat4(1.f), direction);

			directionLightSources[directionalLightCount].direction = glm::normalize(rot[3]);

			// Replace the first light if the count is more than the engines max available lights
			directionalLightCount = (directionalLightCount >= MAX_DIRECTION_LIGHT - 1) ? 0 : directionalLightCount + 1;

		}

		else if (lightSource.lightType == SPOT_LIGHT)
		{
			// Cull
			spotLightSources[spotLightCount].enableShadow = lightSource.enableShadow;
			spotLightSources[spotLightCount].lightpos = transform.GetGlobalTranslation();
			spotLightSources[spotLightCount].lightColor = lightSource.lightingColor;
			spotLightSources[spotLightCount].intensity = lightSource.intensity;


			glm::vec3 direction = glm::vec3(0.f, -1.f, 0.f);
			glm::vec3 rotation = transform.GetGlobalRotation();
			glm::mat4 rot = glm::toMat4(glm::quat(vec3(rotation)));

			rot *= glm::translate(glm::mat4(1.f),direction);

			spotLightSources[spotLightCount].direction = glm::normalize(rot[3]);

			spotLightSources[spotLightCount].inner_CutOff = glm::cos(glm::radians(lightSource.inner_CutOff));
			spotLightSources[spotLightCount].outer_CutOff = glm::cos(glm::radians(lightSource.outer_CutOff));

			// Replace the first light if the count is more than the engines max available lights
			spotLightCount = (spotLightCount >= MAX_SPOT_LIGHT - 1) ? 0 : spotLightCount + 1;
		}
		//std::cout << spotLightCount << "\n";

		if (currentScene.Has<MeshRenderer>(entity))
		{
			MeshRenderer& mesh_component = currentScene.Get<MeshRenderer>(entity);
			if (mesh_component.state == DELETED) continue;
		}
	}


	if (!haveLight)
	{
		lightingSource.lightColor = glm::vec3(0.f, 0.f, 0.f);
	}
}

void Lighting::Exit()
{

}