/*!***************************************************************************************
\file			Serializer.h
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the declarations of the following:
    1. Serialization

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "YAMLUtils.h"
#include "Properties.h"

#include "glm/vec3.hpp"

class Component : public property::base
{
public:
    virtual ~Component() = default;
};

struct TransformComponent : public Component, public property::base
{
protected:
    float position;
    float rotation;
    float scale;

public:
    void DefaultValues(void) noexcept
    {
        position = 1.f;
        rotation = 1.f;
        scale = 1.f;
    }

    void SanityCheck(void) const noexcept
    {
        assert(position == 1.f);
        assert(rotation == 1.f);
        assert(scale == 1.f);
    }

    property_vtable()
};

property_begin_name(TransformComponent, "Transform")
{
    property_var(position),
    property_var(rotation),
    property_var(scale)
}
property_vend_h(TransformComponent)
#endif // !SERIALIZER_H