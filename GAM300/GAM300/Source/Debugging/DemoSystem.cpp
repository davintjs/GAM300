/*!***************************************************************************************
\file			DemoSystem.cpp
\project
\author         Sean Ngo

\par			Course: GAM300
\date           18/09/2023

\brief
    This file contains the definitions of the following:
    1. Demo system for debugging and testing purposes

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "DemoSystem.h"
#include "IOManager/InputHandler.h"
#include "Scene/Scene.h"
#include "Core/Events.h"
#include "Core/EventsManager.h"
#include "Graphics/Texture/TextureManager.h"
#include "AssetManager/AssetManager.h"
#include "Scene/SceneManager.h"
#include "AI/NavMeshBuilder.h"

void DemoSystem::Init()
{

	//MySceneManager.LoadScene("Assets/Scene/LevelPlay.scene");
	//MySceneManager.LoadScene("Assets/Scene/JosScene.scene");
	//MySceneManager.LoadScene("Assets/Scene/EuanTestingArena.scene");
	//MySceneManager.LoadScene("Assets/Scene/MainMenu.scene");
	MySceneManager.LoadScene("Assets/Scene/LevelTutorial.scene");
}

void DemoSystem::Update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (InputHandler::isKeyButtonPressed(GLFW_KEY_B))
	{
		if (!NAVMESHBUILDER.GetNavMesh()) // First build
		{
			NAVMESHBUILDER.BuildNavMesh(); // Build the NavMesh
		}
		else // Rebaking
		{
			NAVMESHBUILDER.Exit(); // Clear current NavMesh
			NAVMESHBUILDER.BuildNavMesh(); // Rebuild NavMesh
		}
	}
}

void DemoSystem::Exit()
{

}