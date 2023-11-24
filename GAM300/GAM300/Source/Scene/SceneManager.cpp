/*!***************************************************************************************
\file			SceneManager.cpp
\project
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
	This file contains the definitions of the following:
	1. Scene Manager System
		a. For loading, saving and creating new scene
		b. Getters for checking loadedScenes
		c. Event callbacks from other systems

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "SceneManager.h"
#include "Utilities/Serializer.h"
#include "Core/EventsManager.h"
#include "IOManager/InputSystem.h"

namespace
{
	const std::string defaultName = "New Scene";
	const std::string defaultPath = "Assets/Scene/";
}

void SceneManager::Init()
{
	EVENTS.Subscribe(this, &SceneManager::CallbackCreateScene);
	EVENTS.Subscribe(this, &SceneManager::CallbackLoadScene);
	EVENTS.Subscribe(this, &SceneManager::CallbackSaveScene);
	EVENTS.Subscribe(this, &SceneManager::CallbackIsNewScene);
}

void SceneManager::CreateScene()
{
	const std::string filePath(defaultPath + defaultName);

	// Check Duplicate Scene
	//if (DuplicateScene())
	//{
	//	// bean: Should create another new scene with different name
	//	std::cout << "Warning Duplicate Scene!\n";
	//	return;
	//}
	SelectedEntityEvent selected{ nullptr };
	EVENTS.Publish(&selected);
	loadedScenes.emplace_front(filePath);
}

void SceneManager::LoadScene(const std::string& _filePath)
{
	// Bean: Next time check if the scene has already been loaded
	SelectedEntityEvent selected{ nullptr };
	EVENTS.Publish(&selected);
	loadedScenes.emplace_front(_filePath);
	
	Scene& scene = GetCurrentScene();
	SceneChangingEvent e{ scene };
	EVENTS.Publish(&e);
	E_ASSERT(DeserializeScene(scene), "Error loading scene!");
	SelectedEntityEvent sE{ nullptr };
	EVENTS.Publish(&sE);

	PRINT("Scene \"" + scene.sceneName + "\" has been loaded.\n");
}

bool SceneManager::SaveScene(const std::string& _filePath)
{
	// Check if there is a loaded scene
	if (loadedScenes.empty())
	{
		std::cout << "No Scene Loaded!\n";
		return false;
	}

	// Get the current scene and save it
	std::cout << "Saving Scene...\n";
	Scene& currentScene = GetCurrentScene();
	std::string filePath;

	// If there is a specific file path
	if (!_filePath.empty())
	{
		filePath = _filePath;

		size_t pos = filePath.find_last_of('\\') + 1;
		size_t pos2 = filePath.find_last_of('.');
		currentScene.sceneName = filePath.substr(pos, pos2 - pos);

		// Check for extension
		if (filePath.find(".scene") == std::string::npos)
			filePath += ".scene";

		currentScene.filePath = filePath;
	}

	if (!SerializeScene(currentScene))
	{
		std::cout << "Error saving current scene!\n";
		return false;
	}

	std::cout << "Scene \"" << currentScene.sceneName << "\" has been saved.\n";
	return true;
}

void SceneManager::Update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	Scene& scene = GetCurrentScene();
	scene.ClearBuffer();


	if (sceneToLoad != "")
	{
		StopScene();
		LoadScene(sceneToLoad);
		StartScene();
		sceneToLoad = "";
		++sceneCount;
	}
}


void SceneManager::StartScene()
{
	if (HasScene())
	{
		SceneChangingEvent e{ GetCurrentScene() };
		EVENTS.Publish(&e);
	}
#ifndef _BUILD

	loadedScenes.emplace_front(GetCurrentScene().filePath.string());
	// Publish scene change
	// 
	GetCurrentScene() = GetPreviousScene();
	//Herr
	GetCurrentScene().sceneName += " [PREVIEW]";
#endif _BUILD

	PRINT("SCENE START\n");
	// Publish navmesh build event
	NavMeshBuildEvent e;
	EVENTS.Publish(&e);

	InputSystem::Instance().LockCursor(true);

	sceneCount++;

	SceneStartEvent startEvent{};
	EVENTS.Publish(&startEvent);
}

void SceneManager::CallbackCreateScene(CreateSceneEvent* pEvent)
{
	// Bean: prompt to save current scene first

	CreateScene();
	pEvent->scene = &GetCurrentScene();
}

void SceneManager::CallbackLoadScene(LoadSceneEvent* pEvent)
{
	// Bean: prompt to save current scene first if havent done so

	LoadScene(pEvent->filePath);
}

void SceneManager::CallbackSaveScene(SaveSceneEvent* pEvent)
{
	SaveScene(pEvent->filePath);
}

void SceneManager::CallbackIsNewScene(IsNewSceneEvent* pEvent)
{
	if (loadedScenes.empty())
	{
		pEvent->data = true;
	}
	else if (GetCurrentScene().sceneName == defaultName)
	{
		pEvent->data = true;
	}
}

void SceneManager::StopScene()
{
	Engine::UUID sceneID = GetCurrentScene().uuid;
	for (int i = 0; i < sceneCount; i++)
	{
		loadedScenes.pop_front();
	}

	sceneCount = 0;

	InputSystem::Instance().LockCursor(false);
	SceneStopEvent stopEvent{ sceneID };
	EVENTS.Publish(&stopEvent);
	PRINT("SCENE STOP\n");
}

void SceneManager::Exit()
{
	loadedScenes.clear();
}
