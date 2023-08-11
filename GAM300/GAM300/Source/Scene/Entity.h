#include "Utilities/UUID.h"
#include "Utilities/ObjectsList.h"
#include <vector>

#ifndef ENTITY_H
#define ENTITY_H

constexpr size_t MAX_ENTITIES{ 100000 };

struct Scene;
using EntityIndex = size_t;

struct Entity
{
	Entity(UUID _uuid = CreateUUID());
	const UUID uuid;
	Scene* pScene = nullptr;
	//std::bitset<MAX_COMPONENTS> componentsBitset{};
};

using EntitiesArray = ObjectsList<Entity, MAX_ENTITIES>;
using EntitiesPtrArray = std::vector<Entity*>;

#endif // !ENTITY_H
