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
	//PRINT("Hello", " World!");
	Scene& scene = GetCurrentScene();
	for (Entity& entity : loadedScenes.front().entities)
	{
		DenseIndex index = entity.GetDenseIndex();
		scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(index);
	}

	int j = 0;
	for (MultiComponent<Script>& multiScript : scene.multiComponentsArrays.GetArray<Script>())
	{
		for (Script& script : multiScript)
		{
			PRINT("SCRIPT: ", j, ' ');
			++j;
		}
	}

	//for (Transform& transfrom : loadedScenes.front().singleComponentsArrays.GetArray<Transform>())

	//PRINT('\n');
}

void SceneManager::Exit()
{
	loadedScenes.clear();
}