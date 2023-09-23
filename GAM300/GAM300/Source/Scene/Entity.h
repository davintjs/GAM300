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
	std::bitset<AllComponentTypes::Size()> hasComponentsBitset;
	property_vtable();
	friend struct Scene;
};

property_begin_name(Entity, "Entity") {
}property_vend_h(Entity);

#endif // !ENTITY_H
