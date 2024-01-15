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

#define MAX_POINT_LIGHT 10
#define MAX_SPOT_LIGHT 10
#define MAX_DIRECTION_LIGHT 2

//extern LightProperties spot_light_stuffs;
//extern LightProperties directional_light_stuffs;
//extern LightProperties point_light_stuffs;



void Lighting::Init()
{
	for (int i = 0; i < MAX_POINT_LIGHT; ++i)
	{
		LightProperties temp_point;
		FRAMEBUFFER.CreatePointLight(temp_point.shadowFBO, temp_point.shadow, SHADOW_WIDTH, SHADOW_HEIGHT);
		pointLightSources.push_back(temp_point);

		LightProperties temp_spot;
		FRAMEBUFFER.CreateDirectionalAndSpotLight(temp_spot.shadowFBO, temp_spot.shadow, SHADOW_WIDTH, SHADOW_HEIGHT);
		spotLightSources.push_back(temp_spot);
	}
	for (int i = 0; i < MAX_DIRECTION_LIGHT; ++i)
	{
		LightProperties temp_directional;
		FRAMEBUFFER.CreateDirectionalAndSpotLight(temp_directional.shadowFBO, temp_directional.shadow, SHADOW_WIDTH_DIRECTIONAL, SHADOW_HEIGHT_DIRECTIONAL);
		directionLightSources.push_back(temp_directional);

	}

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

			pointLightSources[pointLightCount].enableShadow = lightSource.enableShadow;
			pointLightSources[pointLightCount].lightpos = transform.translation;
			pointLightSources[pointLightCount].lightColor = lightSource.lightingColor;
			pointLightSources[pointLightCount].intensity = lightSource.intensity;
			++pointLightCount;

		}


		else if (lightSource.lightType == DIRECTIONAL_LIGHT)// Directional Light - WIP
		{
			directionLightSources[directionalLightCount].enableShadow = lightSource.enableShadow;
			directionLightSources[directionalLightCount].lightpos = transform.translation;
			directionLightSources[directionalLightCount].lightColor = lightSource.lightingColor;
			directionLightSources[directionalLightCount].intensity = lightSource.intensity;
			directionLightSources[directionalLightCount].direction = lightSource.direction;
			++directionalLightCount;

		}

		else if (lightSource.lightType == SPOT_LIGHT)// SpotLight - WIP
		{

			spotLightSources[spotLightCount].enableShadow = lightSource.enableShadow;
			spotLightSources[spotLightCount].lightpos = transform.translation;
			spotLightSources[spotLightCount].lightColor = lightSource.lightingColor;
			spotLightSources[spotLightCount].intensity = lightSource.intensity;


			glm::vec3 direction = glm::vec3(0.f, 0.f, 1.f);
			glm::vec3 rotation = transform.GetRotation();
			glm::mat4 rot = glm::toMat4(glm::quat(vec3(rotation)));

			rot *= glm::translate(glm::mat4(1.f),direction);
			glm::vec3 testdir = rot[3];

			glm::vec3 test(0.f);
			/*if (test == rotation)
			{
				spotLightSources[spotLightCount].direction = glm::vec3(0.f,-1.f,0.f);
			}
			else
			{*/
				//spotLightSources[spotLightCount].direction = glm::radians(glm::normalize(rotation));
				//spotLightSources[spotLightCount].direction = glm::normalize(glm::degrees(rotation));
				spotLightSources[spotLightCount].direction = glm::normalize(testdir);
				//spotLightSources[spotLightCount].direction = glm::degrees(rotation);
				//spotLightSources[spotLightCount].direction = glm::normalize(rotation);
				//spotLightSources[spotLightCount].direction = rotation;

			//}

			spotLightSources[spotLightCount].inner_CutOff = glm::cos(glm::radians(lightSource.inner_CutOff));
			spotLightSources[spotLightCount].outer_CutOff = glm::cos(glm::radians(lightSource.outer_CutOff));

			++spotLightCount;
		}
		//std::cout << spotLightCount << "\n";

		if (currentScene.Has<MeshRenderer>(entity))
		{
			MeshRenderer& mesh_component = currentScene.Get<MeshRenderer>(entity);
			if (mesh_component.state == DELETED) continue;

			//mesh_component.mr_Albedo = glm::vec4(glm::vec3(lightSource.lightingColor), 1.f);

		//	mesh_component.mr_metallic = -1.f;
		//	mesh_component.mr_roughness = -1.f;
		//	mesh_component.ao = -1.f;
		//	mesh_component.ao = -1.f;
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