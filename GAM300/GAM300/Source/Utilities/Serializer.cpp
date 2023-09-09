/*!***************************************************************************************
\file			Serializer.cpp
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the definitions of the following:
    1. Serialization

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "Serializer.h"

bool SceneSerializer(Scene& _scene)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << _scene.sceneName;
    out << YAML::EndMap;

    for (Entity& entity : _scene.entities)
    {
        bool serialized = SerializeEntity(out, entity);
        assert(serialized, "Unable To Serialize Entity!\n");
    }

    if(!out.good())
        std::cout << "Emitter error: " << out.GetLastError() << "\n";

    std::ofstream fout(_scene.filePath.string());
    fout << out.c_str();

    if (fout.fail())
    {
        fout.close();
        return false;
    }

    fout.close();
    return true;
}

// Bean: Encapsulation for all different types of serializing like Scene, Prefab, NavMesh Data etc
void Serialize(const std::string& _filepath)
{

}

void SerializeRuntime(const std::string& _filepath)
{

}

bool SerializeEntity(YAML::Emitter& out, Entity& _entity)
{
    out << YAML::BeginMap;
    out << YAML::Key << "GameObject" << YAML::Value;
    out << YAML::BeginMap;
    out << YAML::Key << "m_UUID" << YAML::Key << _entity.uuid;
    out << YAML::Key << "m_Index" << YAML::Value << _entity.denseIndex;
    out << YAML::EndMap;
    out << YAML::EndMap;

    // Bean: Include components here
    return true;
}

void Deserialize(const std::string& _filepath)
{

}

void DeserializeRuntime(const std::string& _filepath)
{

}