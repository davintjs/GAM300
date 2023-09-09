#include "Core/SystemInterface.h"
#include "Core/Events.h"
#include "Scene.h"

#define MySceneManager SceneManager::Instance()

#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

ENGINE_SYSTEM(SceneManager)
{
public:
	void Init();
	void Update(float dt);
	void Exit();

	void CreateScene();

	void LoadScene(const char* path = "");

	bool SaveScene();

	bool DuplicateScene();

	Scene& GetCurrentScene() { return loadedScenes.front(); }
	
	void CallbackSaveScene(SaveSceneEvent* pEvent);

private:
	std::list<Scene> loadedScenes;
};

#endif // !SCENE_MANAGER_H
