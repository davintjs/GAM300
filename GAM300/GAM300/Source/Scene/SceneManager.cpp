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
	for (Entity& entity : scene.entities)
	{
		scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(entity.denseIndex);
	}

	//Issue with this, iterating over empty multicomponents
	//for (Script& script : scene.multiComponentsArrays.GetArray<Script>())
	//{

	//}

	//for (Transform& transfrom : loadedScenes.front().singleComponentsArrays.GetArray<Transform>())

	//PRINT('\n');
}

void SceneManager::Exit()
{
	loadedScenes.clear();
}