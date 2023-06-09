#include "Core/SystemInterface.h"
#include "Scene.h"

#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

ENGINE_SYSTEM(SceneManager)
{
public:
	void Init();
	void Update();
	void Exit();

	void LoadScene(const char* path = "");

	Scene& GetCurrentScene() { return loadedScenes.front(); }
	Entity& AddEntity();
	template <typename Component>
	Component& AddComponent();
private:
	std::list<Scene> loadedScenes;
};

#endif // !SCENE_MANAGER_H
