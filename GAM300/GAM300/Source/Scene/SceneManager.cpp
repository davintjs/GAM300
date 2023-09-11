#include "Precompiled.h"
#include "SceneManager.h"
#include "Utilities/Serializer.h"
#include "Core/EventsManager.h"

namespace
{
	const std::string defaultName = "New Scene";
	const std::string defaultPath = "Assets/Scene/";
}

void SceneManager::Init()
{
	EVENTS.Subscribe(this, &SceneManager::CallbackCreateScene);
	EVENTS.Subscribe(this, &SceneManager::CallbackSaveScene);
	EVENTS.Subscribe(this, &SceneManager::CallbackIsNewScene);

	if (loadedScenes.empty())
	{
		CreateScene();
		Scene& scene = GetCurrentScene();
		scene.AddEntity();
	}
}

void SceneManager::CreateScene()
{
	const std::string filePath(defaultPath + defaultName);

	// Check Duplicate Scene
	if (DuplicateScene())
	{
		// bean: Should create another new scene with different name
		std::cout << "Warning Duplicate Scene!\n";
		return;
	}
	EditorHierarchy::Instance().ClearLayer();
	loadedScenes.emplace_front(filePath);
}

void SceneManager::LoadScene(const char* path)
{
	loadedScenes.emplace_front(path);
	Scene& scene = GetCurrentScene();
	EditorHierarchy::Instance().ClearLayer();
	
	Entity& titty = scene.AddEntity();
	scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(titty.denseIndex).translation = Vector3(0.f, 100.f, 0.f);
	scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(titty.denseIndex).scale = Vector3(100.f, 10.f, 10.f);

	// test instance rendering
	for (int i = 0; i < 5; ++i) {
		Entity& tempent = scene.AddEntity();
		scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(tempent.denseIndex).translation = Vector3((rand()%1000) - 500.f, (rand() % 1000) - 500.f, (rand() % 1000) - 500.f);
		scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(tempent.denseIndex).scale = Vector3((rand() % 50), (rand() % 50), (rand() % 50));
	}
	scene.AddComponent<Script>(0).name = "Player";
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

	if (!SceneSerializer(currentScene))
	{
		std::cout << "Error saving current scene!\n";
		return false;
	}

	std::cout << "Scene \"" << currentScene.sceneName << "\" has been saved.\n";
	return true;
}

void SceneManager::ChangeScene(Scene& _newScene)
{
	// Bean: Prompt to save current scene (save for now)
	SaveScene(GetCurrentScene().filePath.string());
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

void SceneManager::CallbackCreateScene(CreateSceneEvent* pEvent)
{
	CreateScene();
	pEvent->scene = &GetCurrentScene();
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


void SceneManager::Exit()
{
	loadedScenes.clear();
}
