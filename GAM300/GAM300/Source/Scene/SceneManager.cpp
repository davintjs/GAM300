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
	//for (Entity& entity : scene.entities)
	//{
	//	scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(entity.denseIndex);
	//}

	//for (Rigidbody& rb : scene.singleComponentsArrays.GetArray<Rigidbody>())
	//{
	//	PRINT("RB:", scene.singleComponentsArrays.GetArray<Rigidbody>().GetDenseIndex(rb), '\n');
	//}

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