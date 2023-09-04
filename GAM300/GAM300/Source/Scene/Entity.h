#include "Utilities/UUID.h"
#include "Utilities/ObjectsList.h"
#include <vector>
#include <Scene/Components.h>
#include <bitset>

#ifndef ENTITY_H
#define ENTITY_H

constexpr size_t MAX_ENTITIES{ 5 };

struct Scene;
struct Entity;

using EntitiesList = ObjectsList<Entity, MAX_ENTITIES>;

struct Entity
{
public:
	Entity(Engine::UUID _uuid = Engine::CreateUUID());
	Entity() = delete;
	const Engine::UUID uuid;
	//Which array does this object belong in?
	Scene* pScene;
	ObjectIndex denseIndex;
	std::bitset<10> hasComponentsBitset;
	
};

using EntitiesPtrArray = std::vector<Entity*>;

#endif // !ENTITY_H
