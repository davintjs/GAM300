#include "Core/SystemInterface.h"
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

	void LoadScene(const char* path = "");

	Scene& GetCurrentScene() { return loadedScenes.front(); }
private:
	std::list<Scene> loadedScenes;
};

#endif // !SCENE_MANAGER_H
