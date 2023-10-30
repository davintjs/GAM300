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
    out << YAML::BeginSeq << v.w << v.x << v.y << v.z  << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Engine::GUID& v)
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