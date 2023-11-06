#include "Precompiled.h"
#include "Identifiers.h"
#include "Core/EventsManager.h"

void IdentifiersManager::CreateTag(const std::string& tagName)
{
	if (tags.find(tagName) != tags.end())
	{
		PRINT("Cant have two tags of the same name");
		//Exists
		return;
	}
	tags.emplace(tagName,Engine::CreateUUID());
}

void IdentifiersManager::DeleteTag(std::string key)
{
	auto it = tags.find(key);
	if (it != tags.end())
	{
		//Error when trying to delete tag
		//tags.erase(tags.find(key)); 
		it->second = Engine::UUID(-1);
	}
}

void IdentifiersManager::CreateLayer(const std::string& layerName)
{
	int i = 0;
	for (auto& ref : physicsLayers) {
		if (ref.name.empty()) break;
		i++;	
	}

	if (i < MAX_PHYSICS_LAYERS) {
		physicsLayers[i] = Layer(layerName);
	}
	else {
		PRINT("Exceed max layer count");
		return;
	}
}

//return all tags in the current project
Tags& IdentifiersManager::GetTags() {
	return tags;
}


std::string IdentifiersManager::GetTagString(Engine::UUID tagID)
{
	for (auto& pair : tags)
	{
		if (pair.second == tagID)
			return pair.first;
	}
	return "";
}