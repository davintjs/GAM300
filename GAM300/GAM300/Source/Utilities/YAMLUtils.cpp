/*!***************************************************************************************
\file			YAMLUtils.cpp
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the definitions of the following:
    1. YAML Helper Functions
        a. Vector 2
        b. Vector 3
        c. Vector 4
        d. The transform component Child data 
    2. YAML Encoder
        a. Vector 2
        b. Vector 3
        c. Vector 4
    3. YAML Decoder
        a. Vector 2
        b. Vector 3
        c. Vector 4

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "YAMLUtils.h"
#include "Scene/Scene.h"
#include "Scene/Components.h"

template <typename T>
YAML::Emitter& operator<<(YAML::Emitter& out, const std::vector<Engine::GUID<T>>& _data)
{
    out << YAML::BeginMap;
    for (Engine::GUID data : _data)
    {
        out << YAML::Key << "data" << YAML::Key << YAML::Flow << YAML::BeginMap;
        out << YAML::Key << "guid" << YAML::Value << data << YAML::EndMap;
    }
    out << YAML::EndMap;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const ModelAsset& _data)
{
    out << YAML::Key << "meshes" << YAML::Value << _data.meshes;
    out << YAML::Key << "materials" << YAML::Value << _data.materials;
    out << YAML::Key << "animations" << YAML::Value << _data.animations;

    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Child& _data)
{
    out << YAML::BeginSeq;

    for (auto data : _data._transform)
    {
        out << YAML::Flow << YAML::BeginMap;
        out << YAML::Flow << YAML::Key << "fileID" << YAML::Value << data;
        out << YAML::EndMap;
    }
    
    out << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
    return out;
}

template <typename T>
YAML::Emitter& operator<<(YAML::Emitter& out, const Engine::GUID<T>& v)
{
    out << v.ToHexString();
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Engine::HexID& v)
{
    out << v.ToHexString();
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, char*& v)
{
    std::string buf = v;
    out << YAML::Flow;
    out << YAML::BeginSeq << buf << YAML::EndSeq;
    return out;
}