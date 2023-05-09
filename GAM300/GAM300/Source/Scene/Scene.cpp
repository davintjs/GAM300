
#include "Precompiled.h"
#include "Scene/Scene.h"


Scene::Scene(const std::string& _filepath)
{
	//If a filepath was given
	if (!_filepath.empty())
	{
		//Deserialize
	}
}

bool Scene::EntityIsActive(EntityIndex index)
{
	return entities.GetActive(index);
}

void Scene::EntitySetActive(EntityIndex index, bool value)
{
	entities.SetActive(index,value);
}