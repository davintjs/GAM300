/*!***************************************************************************************
\file			Entity.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			10/03/2023

\brief
	This file declares the entity for ECS

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#include "Utilities/UUID.h"
#include <bitset>
#include "Components.h"
#include "Object.h"

#ifndef ENTITY_H
#define ENTITY_H

struct Entity : Object
{
	std::bitset<AllComponentTypes::Size()> hasComponentsBitset;
	property_vtable();
};

property_begin_name(Entity, "Entity") {
}property_vend_h(Entity);

#endif // !ENTITY_H
