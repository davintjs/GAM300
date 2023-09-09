#include "Precompiled.h"
#include "SceneManager.h"
#include "Utilities/Serializer.h"
#include "Core/EventsManager.h"

void SceneManager::Init()
{
	EVENT.Subscribe(this, &SceneManager::CallbackSaveScene);

	if (loadedScenes.empty())
	{
		CreateScene();
		Scene& scene = GetCurrentScene();
		scene.AddEntity();
	}
}

void SceneManager::CreateScene()
{
	const std::string filePath("New Scene");

	// Check Duplicate Scene
	if (DuplicateScene())
	{
		// bean: Should create another new scene with different name
		std::cout << "Warning Duplicate Scene!\n";
		return;
	}

	loadedScenes.emplace_front(filePath);
}

void SceneManager::LoadScene(const char* path)
{
	loadedScenes.emplace_front(path);
	Scene& scene = GetCurrentScene();
	
	Entity& titty = scene.AddEntity();
	scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(titty.denseIndex).translation = Vector3(0.f, 100.f, 0.f);
	scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(titty.denseIndex).scale = Vector3(100.f, 10.f, 10.f);
}

bool SceneManager::SaveScene()
{
	const std::string sceneFolder = "Assets/Scene/";

	// Check if there is a loaded scene
	if (loadedScenes.empty())
	{
		std::cout << "No Scene Loaded!\n";
		return false;
	}

	// Get the current scene and save it
	std::cout << "Saving Scene...\n";
	Scene& currentScene = GetCurrentScene();
	std::string filePath = currentScene.filePath.stem().string() + ".scene";

	// Check for extension
	if (filePath.find(".scene") == std::string::npos)
		filePath += ".scene";

	// Place scene in default scene folder
	filePath = sceneFolder + filePath;
	currentScene.filePath = filePath;

	if (!SceneSerializer(currentScene))
	{
		std::cout << "Error saving current scene!\n";
		return false;
	}

	std::cout << "Scene \"" << currentScene.sceneName << "\" has been saved.\n";
	return true;
}

bool SceneManager::DuplicateScene()
{
	return false;
}

void SceneManager::Update(float dt)
{
	Scene& scene = GetCurrentScene();
	//for (auto it = scene.entities.begin(); it != scene.entities.end(); ++it)
	//{
	//	if (!it.IsActive())
	//		continue;
	//	Entity& entity = *it;
	//	PRINT("Entity: ", scene.entities.GetDenseIndex(entity), '\n');
	//}

	//auto& scriptArr = scene.GetComponentsArray<Script>();
	//for (auto it = scriptArr.begin(); it != scriptArr.end(); ++it)
	//{
	//	//if (!it.IsActive())
	//	//	continue;
	//	Script& script = *it;
	//	PRINT("Script: ", scriptArr.GetDenseIndex(script), '\n');
	//}
	//PRINT('\n');

	//Issue with this, iterating over empty multicomponents
	//for (Script& script : scene.multiComponentsArrays.GetArray<Script>())
	//{
	//	PRINT("Script: ", scene.multiComponentsArrays.GetArray<Script>().GetDenseIndex(script), '\n');
	//}
	scene.ClearBuffer();

	//for (Transform& transfrom : loadedScenes.front().singleComponentsArrays.GetArray<Transform>())

	//PRINT('\n');
}


void SceneManager::CallbackSaveScene(SaveSceneEvent* pEvent)
{
	SaveScene();
}

void SceneManager::Exit()
{
	loadedScenes.clear();
}
