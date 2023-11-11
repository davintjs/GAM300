/*!***************************************************************************************
\file			Lighting.cpp
\project
\author			Euan Lim

\par			Course: GAM300
\date           11/10/2023

\brief
	This file contains the definitions of Graphics Lighting that includes:
	1. Computation for the light stuffs that will be given to the eventual rendering

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include "Scene/SceneManager.h"

extern LightProperties spot_light_stuffs;
extern LightProperties directional_light_stuffs;
extern LightProperties point_light_stuffs;


void Lighting::Init()
{



}

void Lighting::Update(float)
{
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();

	// Temporary Light stuff
	bool haveLight = false;

	// Just resetting the light stuffs here
	pointLightSources.clear();
	directionLightSources.clear();
	spotLightSources.clear();
		// This is temporary - a further reset
	point_light_stuffs.inUse = false;
	directional_light_stuffs.inUse = false;
	spot_light_stuffs.inUse = false;


	for (LightSource& lightSource : currentScene.GetArray<LightSource>())
	{
		if (lightSource.state == DELETED) continue;

		haveLight = true;
		Entity& entity{ currentScene.Get<Entity>(lightSource) };
		Transform& transform = currentScene.Get<Transform>(entity);

		LightProperties Temporary;

		Temporary.lightpos = transform.translation;
		Temporary.lightColor = lightSource.lightingColor;
		Temporary.intensity = lightSource.intensity;


		if (lightSource.lightType == POINT_LIGHT)// Point Light
		{
			pointLightSources.push_back(Temporary);
			point_light_stuffs = Temporary;
		}


		else if (lightSource.lightType == DIRECTIONAL_LIGHT)// Directional Light - WIP
		{

			Temporary.direction = lightSource.direction; // CHANGE


			directionLightSources.push_back(Temporary);
			directional_light_stuffs = Temporary;
		}


		else if (lightSource.lightType == SPOT_LIGHT)// SpotLight - WIP
		{
			//Temporary.direction = glm::vec3(0.f,-1.f,0.f); // CHANGE
			glm::vec3 direction = glm::vec3(0.f, -1.f, 0.f);
			glm::vec3 rotation = transform.GetRotation();

			glm::vec3 test(0.f);
			if (test == rotation)
			{
				Temporary.direction = glm::vec3(0.f,-1.f,0.f);
			}
			else
			{
				Temporary.direction = rotation;
				Temporary.direction = glm::radians(glm::normalize(Temporary.direction));
				//Temporary.direction.y = -Temporary.direction.y;
			}
			Temporary.inner_CutOff = glm::cos(glm::radians(lightSource.inner_CutOff));
			Temporary.outer_CutOff = glm::cos(glm::radians(lightSource.outer_CutOff));
			spotLightSources.push_back(Temporary);
			spot_light_stuffs = Temporary;
		}


		if (currentScene.Has<MeshRenderer>(entity))
		{
			MeshRenderer& mesh_component = currentScene.Get<MeshRenderer>(entity);
			if (mesh_component.state == DELETED) continue;

			mesh_component.mr_Albedo = glm::vec4(Temporary.lightColor, 1.f);

			mesh_component.mr_metallic = -1.f;
			mesh_component.mr_roughness = -1.f;
			mesh_component.ao = -1.f;
			mesh_component.ao = -1.f;
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