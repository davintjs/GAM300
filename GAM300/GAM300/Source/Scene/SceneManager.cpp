#include "../PCH/Precompiled.h"
#include "SceneManager.h"

void SceneManager::Init()
{
	if (loadedScenes.empty())
	{
		LoadScene("");
	}
}

void SceneManager::LoadScene(const char* path)
{
	loadedScenes.emplace_front(path);
}

void SceneManager::Update()
{
	PRINT("Hello", " World!");
}

void SceneManager::Exit()
{

}