/*!***************************************************************************************
\file			Object.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			10/08/2023

\brief
	This file declares the object type to be inherited by entities and components
	for ECS to store Entity Unique Identifier(EUID) and UUID

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/


#include "Utilities/UUID.h"
#include "Properties.h"

#ifndef OBJECT_H
#define OBJECT_H

struct Handle
{
	Engine::UUID euid;
	Engine::UUID uuid;

	//Compares whether two handles have the same IDs
	bool operator==(const Handle& other) const
	{return (euid == other.euid && uuid == other.uuid);}
};

template <>
struct std::hash<Handle>
{
	//Hashing operator to be used by unordered maps
	size_t operator()(const Handle& k) const
	{
		return k.euid ^ k.uuid;
	}
};

struct Object : property::base
{
	//Constructor that generates IDs if not params are given
	Object(Engine::UUID _euid = Engine::CreateUUID(), Engine::UUID _uuid = Engine::CreateUUID()) 
		:euid{ _euid }, uuid{ _uuid } {}
	//Copy constructor
	Object(const Object& rhs) = default;
	//Get euid of object
	Engine::UUID EUID() const{ return euid; }
	//Get uuid of object
	Engine::UUID UUID () const { return uuid; }
	//Get euid and uuid as a handle
	operator Handle() const { return{ euid, uuid }; }

	int current_layer = 0; //id of the current layer (default = 0)
	int tag = 0;  //id tag of the current object (default = 0)

	property_vtable();
protected:
	//Allows only scene to change ids (DANGEROUS)
	Engine::UUID uuid;
	Engine::UUID euid;
	friend struct Scene;
};

property_begin_name(Object, "Object") {
	property_var(euid).Name("EUID").Flags(property::flags::DONTSHOW),
		property_var(uuid).Name("UUID").Flags(property::flags::DONTSHOW),
		property_var(current_layer).Name("Layer").Flags(property::flags::DONTSHOW),
		property_var(tag).Name("Tag").Flags(property::flags::DONTSHOW),
}property_vend_h(Object)

#endif // !OBJECT_H