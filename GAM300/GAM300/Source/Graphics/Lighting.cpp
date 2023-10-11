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


void Lighting::Init()
{

}

void Lighting::Update(float)
{
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();

	// Temporary Light stuff
	bool haveLight = false;
	for (LightSource& lightSource : currentScene.GetArray<LightSource>())
	{
		haveLight = true;
		Entity& entity{ currentScene.Get<Entity>(lightSource) };
		Transform& transform = currentScene.Get<Transform>(entity);

		lightingSource.lightpos = transform.translation;
		lightingSource.lightColor = lightSource.lightingColor;

		if (currentScene.Has<MeshRenderer>(entity))
		{
			MeshRenderer& mesh_component = currentScene.Get<MeshRenderer>(entity);
			mesh_component.mr_Albedo = glm::vec4(lightingSource.lightColor, 1.f);

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