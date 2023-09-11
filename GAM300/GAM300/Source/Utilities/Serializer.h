/*!***************************************************************************************
\file			Serializer.h
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the declarations of the following:
    1. Serialization

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "YAMLUtils.h"
#include "Properties.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

enum CLASSID
{
    OBJECT,
    GAMEOBJECT,
    COMPONENT
};


void Serialize(const std::string& _filepath);
void SerializeRuntime(const std::string& _filepath);

bool SerializeScene(Scene& _scene);
bool SerializeEntity(YAML::Emitter& out, Entity& _entity, Scene& _scene);

void Deserialize(const std::string& _filepath);
void DeserializeRuntime(const std::string& _filepath);

bool DeserializeScene(Scene& _scene);
bool DeserializeEntity(YAML::Node& _node, Entity& _entity, Scene& _scene);

#endif // !SERIALIZER_H