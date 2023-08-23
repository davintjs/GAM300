#include "Precompiled.h"
#include "SceneManager.h"

void SceneManager::Init()
{
	if (loadedScenes.empty())
	{
		//Create empty scene
		LoadScene("");
		loadedScenes.front().AddEntity();
	}
}

void SceneManager::LoadScene(const char* path)
{
	loadedScenes.emplace_front(path);
}

void SceneManager::Update()
{
	//PRINT("Hello", " World!");
}

void SceneManager::Exit()
{
	loadedScenes.clear();
}