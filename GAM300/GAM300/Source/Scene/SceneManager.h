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

	void LoadScene(const std::string& _filePath = "");

	bool SaveScene(const std::string& _filePath = "");

	void ChangeScene(Scene& _newScene);

	bool DuplicateScene();

	bool HasScene();

	Scene& GetCurrentScene() { return loadedScenes.front(); }
	
	void CallbackCreateScene(CreateSceneEvent* pEvent);

	void CallbackLoadScene(LoadSceneEvent* pEvent);

	void CallbackSaveScene(SaveSceneEvent* pEvent);

	void CallbackIsNewScene(IsNewSceneEvent* pEvent);

	void CallbackSceneStart(SceneStartEvent* pEvent);
	void CallbackSceneStop(SceneStopEvent* pEvent);

private:
	std::list<Scene> loadedScenes;
	//Scene* currentScene;
};

#endif // !SCENE_MANAGER_H
