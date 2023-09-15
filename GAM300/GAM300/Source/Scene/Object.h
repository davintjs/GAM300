#include "Utilities/UUID.h"

#ifndef OBJECT_H
#define OBJECT_H

struct Object
{
	Object(Engine::UUID _uuid = Engine::CreateUUID());
	const Engine::UUID uuid;
};

#endif // !OBJECT_H