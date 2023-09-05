#include "Precompiled.h"
#include "SceneManager.h"

void SceneManager::Init()
{
	if (loadedScenes.empty())
	{
		//Create empty scene
		LoadScene("");
	}
}

void SceneManager::LoadScene(const char* path)
{
	loadedScenes.emplace_front(path);
	
	//Init scene
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

	auto& scriptArr = scene.GetComponentsArray<Script>();
	for (auto it = scriptArr.begin(); it != scriptArr.end(); ++it)
	{
		if (!it.IsActive())
			continue;
		Script& script = *it;
		PRINT("Script: ", scriptArr.GetDenseIndex(script), '\n');
	}
	PRINT('\n');

	//Issue with this, iterating over empty multicomponents
	//for (Script& script : scene.multiComponentsArrays.GetArray<Script>())
	//{
	//	PRINT("Script: ", scene.multiComponentsArrays.GetArray<Script>().GetDenseIndex(script), '\n');
	//}
	scene.ClearBuffer();

	//for (Transform& transfrom : loadedScenes.front().singleComponentsArrays.GetArray<Transform>())

	//PRINT('\n');
}

void SceneManager::Exit()
{
	loadedScenes.clear();
}