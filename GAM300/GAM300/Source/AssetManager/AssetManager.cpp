#include "Precompiled.h"
#include "AssetManager/AssetManager.h"

// Loads the required resources only for the first level
void AssetManager::Init()
{
	if (!std::filesystem::exists("Assets"))
	{
		std::cout << "Check if proper assets filepath exists!" << std::endl;
		exit(0);
	}
	SceneManager::Instance().GetCurrentScene();
}

void AssetManager::Update()
{

}

void AssetManager::Exit()
{

}