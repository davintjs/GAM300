/*!***************************************************************************************
\file			YAMLUtils.cpp
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the definitions of the following:
    1. YAML Integration

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "YAMLUtils.h"
#include "Scene/Scene.h"
#include "Scene/Components.h"


template <typename T>
void SerializeBasic(const T& _data, YAML::Emitter& out, const std::string& _key)
{
    out << YAML::Key << _key << YAML::Value << _data;
}

template<>
void SerializeBasic<bool>(const bool& _data, YAML::Emitter& out, const std::string& _key)
{

}

YAML::Emitter& operator<<(YAML::Emitter& out, const Child& _data)
{
    out << YAML::BeginSeq;

    for (auto& data : _data._transform)
    {
        out << YAML::Flow << YAML::BeginMap;
        out << YAML::Flow << YAML::Key << "fileID" << YAML::Value << _data._scene.GetEntity(*data).EUID();
        out << YAML::EndMap;
    }
    
    out << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
    return out;
}