#include "Utilities/UUID.h"
#include <bitset>
#include "Components.h"
#include "Object.h"

#ifndef ENTITY_H
#define ENTITY_H

struct Scene;
struct Entity;


struct Entity : Object
{
	Entity(Engine::UUID _uuid) : Object(_uuid) {}
	Scene* pScene;
	//Which array does this object belong in?
	ObjectIndex denseIndex;
	std::bitset<AllComponentTypes::Size()> hasComponentsBitset;
	property_vtable();
};

property_begin_name(Entity, "Entity") {
		property_var(denseIndex),
}property_vend_h(Entity);

void Set_ParentChild(const ObjectIndex& _parent, const ObjectIndex& _child);
void Set_ParentChild(Transform& parent, Transform& child);
void Break_ParentChild(const ObjectIndex& _child);


#endif // !ENTITY_H
