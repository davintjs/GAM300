#include "Utilities/UUID.h"
#include "Utilities/ObjectsArray.h"
#include <vector>

#ifndef ENTITY_H
#define ENTITY_H

#define MAX_ENTITIES 100000

struct Scene;
using EntityIndex = size_t;

struct Entity
{
	Entity(UUID _uuid = CreateUUID());
	const UUID uuid;
	Scene* pScene = nullptr;
};

using EntitiesArray = ObjectsArray<Entity, MAX_ENTITIES>;
using EntitiesPtrArray = std::vector<Entity*>;

#endif // !ENTITY_H
