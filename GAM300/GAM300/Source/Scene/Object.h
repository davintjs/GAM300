#include "Utilities/UUID.h"
#include "Properties.h"

#ifndef OBJECT_H
#define OBJECT_H

struct Object : property::base
{	
	Object(Engine::UUID _uuid = Engine::CreateUUID());
	const Engine::UUID uuid;
	property_vtable();
};

property_begin_name(Object, "Object") {
	//property_var(uuid)
}property_vend_h(Object)

#endif // !OBJECT_H