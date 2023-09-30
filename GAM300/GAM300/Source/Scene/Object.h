#include "Utilities/UUID.h"
#include "Properties.h"

#ifndef OBJECT_H
#define OBJECT_H

struct Handle
{
	Engine::UUID euid;
	Engine::UUID uuid;

	bool operator==(const Handle& other) const
	{
		return (euid == other.euid && uuid == other.uuid);
	}
};

template <>
struct std::hash<Handle>
{
	size_t operator()(const Handle& k) const
	{
		return k.euid ^ k.uuid;
	}
};

struct Object : property::base
{	
	Object(Engine::UUID _euid = Engine::CreateUUID(), Engine::UUID _uuid = Engine::CreateUUID());
	Object(const Object& rhs) = default;
	Engine::UUID EUID() const{ return euid; }
	Engine::UUID UUID () const { return uuid; }
	operator Handle() const { return{ euid, uuid }; }
	property_vtable();

protected:
	Engine::UUID uuid;
	Engine::UUID euid;
	friend struct Scene;
};




property_begin_name(Object, "Object") {
	property_var(euid).Name("EUID").Flags(property::flags::DONTSHOW),
		property_var(uuid).Name("UUID").Flags(property::flags::DONTSHOW),
}property_vend_h(Object)

#endif // !OBJECT_H