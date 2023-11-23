/*!***************************************************************************************
\file			SceneManager.h
\project
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
	This file contains the declarations of the following:
	1. Scene Manager System
		a. For loading, saving and creating new scene
		b. Getters for checking loadedScenes
		c. Event callbacks from other systems

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Core/SystemInterface.h"
#include "Core/Events.h"
#include "Scene.h"

#define MySceneManager SceneManager::Instance()

#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

ENGINE_SYSTEM(SceneManager)
{
public:
	// Initializing the Scene Manager system
	void Init();

	// Updating of current scene, clear the scene buffer
	void Update(float dt);

	// Exit the system
	void Exit();

	// Create a new scene
	void CreateScene();

	// Load a scene from a file path
	void LoadScene(const std::string& _filePath = "");

	// Save a scene with the file path
	bool SaveScene(const std::string& _filePath = "");

	void StartScene();

	void StopScene();

	// Check if there are scenes loaded
	bool HasScene() { return !loadedScenes.empty(); }

	// Get the previous scene, usually used to reflect objects when starting preview
	Scene& GetPreviousScene() { return *std::next(loadedScenes.begin()); }

	// Get the front of the list as the current scene
	Scene& GetCurrentScene() { return loadedScenes.front(); }
	
	// Callback event when a new scene is created
	void CallbackCreateScene(CreateSceneEvent* pEvent);

	// Callback event when a scene is loaded
	void CallbackLoadScene(LoadSceneEvent* pEvent);

	// Callback event when a scene is saved
	void CallbackSaveScene(SaveSceneEvent* pEvent);

	// Callback event to check if the scene created is a new scene
	void CallbackIsNewScene(IsNewSceneEvent* pEvent);


	std::string sceneToLoad;
private:
	std::list<Scene> loadedScenes;
	int sceneCount = 0; // For checking how many scenes are loaded after starting
};

#endif // !SCENE_MANAGER_H
