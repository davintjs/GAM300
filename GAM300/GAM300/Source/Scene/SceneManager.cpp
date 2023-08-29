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
	int i = 0;
	for (Entity& entity : loadedScenes.front().entities)
	{
		++i;
		//PRINT(entity.uuid, ' ', i, '\n');
		DenseIndex index = entity.GetDenseIndex();
		loadedScenes.front().singleComponentsArrays.GetArray<Transform>().DenseSubscript(index);
	}

	for (Transform& transfrom : loadedScenes.front().singleComponentsArrays.GetArray<Transform>())
	{

	}

	//PRINT('\n');
}

void SceneManager::Exit()
{
	loadedScenes.clear();
}