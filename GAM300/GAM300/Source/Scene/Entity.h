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
	Scene* pScene;
	//Which array does this object belong in?
	ObjectIndex denseIndex;
	std::bitset<AllComponentTypes::Size()> hasComponentsBitset;
};

void Set_ParentChild(const ObjectIndex& _parent, const ObjectIndex& _child);
void Set_ParentChild(Transform& parent, Transform& child);
void Break_ParentChild(const ObjectIndex& _child);


#endif // !ENTITY_H
