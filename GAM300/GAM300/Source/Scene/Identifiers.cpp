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