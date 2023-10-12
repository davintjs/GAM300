/*!***************************************************************************************
\file			Lighting.cpp
\project
\author

\par			Course: GAM300
\date           11/10/2023

\brief
	This file contains the definitions of Graphics Lighting that includes:
	1.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include "Scene/SceneManager.h"
#include "Editor_Camera.h"

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

	for (LightSource& lightSource : currentScene.GetArray<LightSource>())
	{
		haveLight = true;
		Entity& entity{ currentScene.Get<Entity>(lightSource) };
		Transform& transform = currentScene.Get<Transform>(entity);

		LightProperties Temporary;

		Temporary.lightpos = transform.translation;
		Temporary.lightColor = lightSource.lightingColor;

		if (lightSource.type == 0)// Point Light
		{
			pointLightSources.push_back(Temporary);
		}
		else if (lightSource.type == 1)// Directional Light - WIP
		{
			Temporary.direction = { -0.2f, -1.0f, -0.3f }; // CHANGE

			directionLightSources.push_back(Temporary);
		}
		else if (lightSource.type == 2)// SpotLight - WIP
		{
			Temporary.direction = EditorCam.GetForwardVec(); // CHANGE
			//Temporary.inner_CutOff = glm::cos(glm::radians(12.5f));
			//Temporary.outer_CutOff = glm::cos(glm::radians(17.5f));
			Temporary.inner_CutOff = glm::cos(glm::radians(1.5f));
			Temporary.outer_CutOff = glm::cos(glm::radians(3.5f));
			spotLightSources.push_back(Temporary);
		}


		if (currentScene.Has<MeshRenderer>(entity))
		{
			MeshRenderer& mesh_component = currentScene.Get<MeshRenderer>(entity);
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