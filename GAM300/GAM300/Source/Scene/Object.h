#include "Utilities/UUID.h"
#include "Properties.h"

#ifndef OBJECT_H
#define OBJECT_H

struct Object : property::base
{	
	Object(Engine::UUID _euid = Engine::CreateUUID(), Engine::UUID _uuid = Engine::CreateUUID());
	const Engine::UUID euid;	// Entity unique id
	const Engine::UUID uuid;	// Unique identifier for each component
	property_vtable();
};

property_begin_name(Object, "Object") {
	property_var(euid).Name("EUID"),
		property_var(uuid).Name("UUID"),
}property_vend_h(Object)

#endif // !OBJECT_H