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

void SceneManager::Update()
{
	Scene& scene = GetCurrentScene();
	for (Entity& entity : scene.entities)
	{
		scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(entity.denseIndex);
	}

	//Issue with this, iterating over empty multicomponents
	for (MultiComponent<Script>& multiScript : scene.multiComponentsArrays.GetArray<Script>())
	{
		if (multiScript.size() > 0)
			PRINT("SCRIPT: ", scene.multiComponentsArrays.GetArray<Script>().GetDenseIndex(multiScript), '\n');
		for (Script& script : multiScript)
		{
			
		}
	}

	//for (Transform& transfrom : loadedScenes.front().singleComponentsArrays.GetArray<Transform>())

	//PRINT('\n');
}

void SceneManager::Exit()
{
	loadedScenes.clear();
}