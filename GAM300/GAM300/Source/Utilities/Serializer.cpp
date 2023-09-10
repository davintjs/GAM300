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

Scene* localScene;

bool SceneSerializer(Scene& _scene)
{
    localScene = &_scene;

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

    // Bean: Include components here
    Scene& currScene = *localScene;

    if (currScene.HasComponent<Tag>(_entity))
    {
        auto& component = currScene.GetComponent<Tag>(_entity);
        out << YAML::Key << "m_Name" << YAML::Value << component.name;
    }

    if (currScene.HasComponent<Transform>(_entity))
    {
        auto& component = currScene.GetComponent<Transform>(_entity);
        out << YAML::Key << "m_Position" << YAML::Value << component.translation;
        out << YAML::Key << "m_Rotation" << YAML::Value << component.rotation;
        out << YAML::Key << "m_Scale" << YAML::Value << component.scale;
        out << YAML::Key << "m_Children" << YAML::Value << component.child;
        out << YAML::Key << "m_Parent" << YAML::Value << &component.Parent;
    }

    out << YAML::EndMap;
    out << YAML::EndMap;

    return true;
}

void Deserialize(const std::string& _filepath)
{

}

void DeserializeRuntime(const std::string& _filepath)
{

}