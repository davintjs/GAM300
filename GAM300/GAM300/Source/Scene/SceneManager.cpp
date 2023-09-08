#include "Precompiled.h"
#include "SceneManager.h"

void SceneManager::Init()
{
	if (loadedScenes.empty())
	{
		//Create empty scene
		LoadScene("");
		Scene& scene = GetCurrentScene();
		scene.Scene_name = "Test Scene";
		/*Scene& scene = GetCurrentScene();
		//Add 5 new entity into scene
		scene.AddEntity();
		scene.AddEntity();
		scene.AddEntity();
		scene.AddEntity();
		scene.AddEntity();*/
	}
}

void SceneManager::LoadScene(const char* path)
{
	loadedScenes.emplace_front(path);
	Scene& scene = GetCurrentScene();
	
	Entity& titty = scene.AddEntity();
	scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(titty.denseIndex).translation = Vector3(0.f, 100.f, 0.f);
	scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(titty.denseIndex).scale = Vector3(100.f, 10.f, 10.f);

	/*scene.AddEntity();
	scene.AddEntity();
	scene.AddEntity();
	scene.AddEntity();
	scene.AddEntity();*/

	// test instance rendering
	for (int i = 0; i < 5; ++i) {
		Entity& tempent = scene.AddEntity();
		scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(tempent.denseIndex).translation = Vector3((rand()%1000) - 500.f, (rand() % 1000) - 500.f, (rand() % 1000) - 500.f);
		scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(tempent.denseIndex).scale = Vector3((rand() % 50), (rand() % 50), (rand() % 50));
	}

	/*Entity& titty2 = scene.AddEntity();
	scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(titty2.denseIndex).translation = Vector3(100.f, 100.f, 100.f);
	scene.singleComponentsArrays.GetArray<Transform>().DenseSubscript(titty2.denseIndex).scale = Vector3(40.f, 40.f, 40.f);*/
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

void SceneManager::Exit()
{
	loadedScenes.clear();
}