#include "Utilities/UUID.h"
#include "Properties.h"

#ifndef OBJECT_H
#define OBJECT_H

struct Object : property::base
{	
	Object(Engine::UUID _euid = Engine::CreateUUID(), Engine::UUID _uuid = Engine::CreateUUID());
	Object(const Object& rhs) = default;
	Engine::UUID EUID() const{ return euid; }
	Engine::UUID UUID() const { return uuid; }
	property_vtable();

protected:
	Engine::UUID uuid;
	Engine::UUID euid;
	friend struct Scene;
};


property_begin_name(Object, "Object") {
	property_var(euid).Name("EUID"),
		property_var(uuid).Name("UUID"),
}property_vend_h(Object)

#endif // !OBJECT_H