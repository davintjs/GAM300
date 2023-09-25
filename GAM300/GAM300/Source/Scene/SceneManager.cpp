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
	EVENTS.Subscribe(this, &SceneManager::CallbackLoadScene);
	EVENTS.Subscribe(this, &SceneManager::CallbackSaveScene);
	EVENTS.Subscribe(this, &SceneManager::CallbackIsNewScene);
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

	loadedScenes.emplace_front(filePath);
}

void SceneManager::LoadScene(const std::string& _filePath)
{
	// Bean: Next time check if the scene has already been loaded


	loadedScenes.emplace_front(_filePath);
	Scene& scene = GetCurrentScene();


	if (!DeserializeScene(scene))
	{
		std::cout << "Error loading scene!\n";
		return;
	}/**/
	Entity& tit = scene.AddEntity().Get();
	Transform& tit_trans = scene.GetComponent<Transform>(tit);
	tit_trans.scale = vec3(20.f, 20.f, 20.f);
	MeshRenderer& tit_render = scene.GetComponent<MeshRenderer>(tit);
	std::cout << "Scene \"" << scene.sceneName << "\" has been loaded.\n";
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

void SceneManager::ChangeScene(Scene& _newScene)
{
	// Bean: Prompt to save current scene (save for now)
	//SaveScene(GetCurrentScene().filePath.string());

	LoadScene(_newScene.filePath.string().c_str());
}

bool SceneManager::DuplicateScene()
{
	return false;
}

void SceneManager::Update(float dt)
{
	Scene& scene = GetCurrentScene();
	scene.ClearBuffer();
}

void SceneManager::CallbackCreateScene(CreateSceneEvent* pEvent)
{
	// Bean: prompt to save current scene first

	ClearEntitiesEvent clearEntitiesEvent;
	EVENTS.Publish(&clearEntitiesEvent);

	CreateScene();
	pEvent->scene = &GetCurrentScene();
}

void SceneManager::CallbackLoadScene(LoadSceneEvent* pEvent)
{
	// Bean: prompt to save current scene first if havent done so

	ClearEntitiesEvent clearEntitiesEvent;
	EVENTS.Publish(&clearEntitiesEvent);

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


void SceneManager::Exit()
{
	loadedScenes.clear();
}
