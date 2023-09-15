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
#include "Editor/EditorHeaders.h"

bool SerializeScene(Scene& _scene)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << _scene.sceneName;
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

    for (Entity* entity : EditorHierarchy::Instance().layer)
    {
        bool serialized = SerializeEntity(out, *entity, _scene);
        assert(serialized, "Unable To Serialize Entity!\n");
    }

    out << YAML::EndSeq;
    out << YAML::EndMap;

    if(!out.good())
        std::cout << "Emitter error: " << out.GetLastError() << "\n";

    std::ofstream fout(_scene.filePath.string(), std::ios::out);
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

bool SerializeEntity(YAML::Emitter& out, Entity& _entity, Scene& _scene)
{
    out.SetIndent(0);
    out << YAML::BeginMap << YAML::Block;
    out << YAML::Key << "GameObject" << YAML::Value;
    out << YAML::BeginMap;
    out << YAML::Key << "m_UUID" << YAML::Key << _entity.uuid;
    out << YAML::Key << "m_IsActive" << YAML::Value << _scene.IsActive(_entity);

    // Bean: Components are placed in different conditions, maybe implement using templates?
    if (_scene.HasComponent<Tag>(_entity))
    {
        auto& component = _scene.GetComponent<Tag>(_entity);
        out << YAML::Key << "m_Name" << YAML::Value << component.name;
    }

    if (_scene.HasComponent<Transform>(_entity))
    {
        auto& component = _scene.GetComponent<Transform>(_entity);
        out << YAML::Key << "m_Position" << YAML::Value << component.translation;
        out << YAML::Key << "m_Rotation" << YAML::Value << component.rotation;
        out << YAML::Key << "m_Scale" << YAML::Value << component.scale;
        out << YAML::Key << "m_Children" << YAML::Value << Child{component.child, _scene};

        if (component.parent)
            out << YAML::Key << "m_Parent" << YAML::Value << _scene.GetEntity(*component.parent).uuid;
        else
            out << YAML::Key << "m_Parent" << YAML::Value << 0;
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

bool DeserializeScene(Scene& _scene)
{
    YAML::Node data;

    try
    {
        data = YAML::LoadFile(_scene.filePath.string());
    }
    catch (YAML::ParserException e)
    {
        std::cout << "Failed to load .scene file \"" << _scene.filePath.string() << "\" due to: " << e.what() << "\n";
        return false;
    }
    catch (YAML::BadFile e)
    {
        std::cout << "Failed to load .scene file \"" << _scene.filePath.string() << "\" due to: " << e.what() << "\n";
        return false;
    }

    if (!data["Scene"])
    {
        std::cout << "Not a .scene file!\n";
        return false;
    }

    _scene.sceneName = data["Scene"].as<std::string>();

    std::cout << "Deserializing scene \"" << _scene.sceneName << "\"\n";

    std::map<Entity*, Engine::UUID> childEntities;

    auto entities = data["Entities"];
    if (!entities)
        return false;

    for (auto entity : entities)
    {
        if (entity["GameObject"])
        {
            YAML::Node object = entity["GameObject"];

            Entity& entity = _scene.AddEntity(object["m_UUID"].as<Engine::UUID>());

            // Bean: Dont need cuz when parenting it reorders the object anyways
            //entity.denseIndex = object["m_Index"].as<ObjectIndex>(); 
            _scene.SetActive(entity, object["m_IsActive"].as<bool>());
            
            // Bean: Create a constructor for entity with transform and tag
            Transform& transform = _scene.GetComponent<Transform>(entity);
            _scene.GetComponent<Tag>(entity).name = object["m_Name"].as<std::string>();
            transform.translation = object["m_Position"].as<Vector3>();
            transform.rotation = object["m_Rotation"].as<Vector3>();
            transform.scale = object["m_Scale"].as<Vector3>();
            
            auto parent = object["m_Parent"];
            if (parent)
            {
                Engine::UUID parentUUID = parent.as<Engine::UUID>();
                if (parentUUID != 0)
                    childEntities[&entity] = parentUUID;
            }
        }
    }

    // Link all children with parents
    for (auto pair : childEntities)
    {
        for (auto& entity : _scene.entities)
        {
            if (entity.uuid == pair.second)
            {
                Transform& transform = _scene.GetComponent<Transform>(*pair.first);
                transform.SetParent(&_scene.GetComponent<Transform>(entity));
                break;
            }
        }
    }

    return true;
}

bool DeserializeEntity(YAML::Node& _node, Entity& _entity, Scene& _scene)
{
    return true;
}