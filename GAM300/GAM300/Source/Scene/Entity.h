#include "Utilities/UUID.h"
#include "Utilities/ObjectsList.h"
#include <vector>

#ifndef ENTITY_H
#define ENTITY_H

constexpr size_t MAX_ENTITIES{ 5 };

struct Scene;
struct Entity;

using EntitiesList = ObjectsList<Entity, MAX_ENTITIES>;

struct Entity
{
public:
	Entity(UUID _uuid = CreateUUID());
	const UUID uuid;
	//Which array does this object belong in?
	EntitiesList* pList = nullptr;
	DenseIndex GetDenseIndex()
	{
		if (pList == nullptr)
			ASSERT(true,"Entity does not belong to any array yet");
		return pList->GetDenseIndex(*this);
	}
};

using EntitiesPtrArray = std::vector<Entity*>;

#endif // !ENTITY_H
