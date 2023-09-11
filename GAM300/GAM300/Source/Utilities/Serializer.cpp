/*!***************************************************************************************
\file			Serializer.cpp
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the definitions of the following:
    1. Serialization

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "Serializer.h"
#include "Editor/EditorHeaders.h"

bool SerializeScene(Scene& _scene)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << _scene.sceneName;
    out << YAML::EndMap;

    for (Entity* entity : EditorHierarchy::Instance().layer)
    {
        bool serialized = SerializeEntity(out, *entity, _scene);
        assert(serialized, "Unable To Serialize Entity!\n");
    }

    /*for (Entity& entity : _scene.entities)
    {
        bool serialized = SerializeEntity(out, entity, _scene);
        assert(serialized, "Unable To Serialize Entity!\n");
    }*/

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

bool SerializeEntity(YAML::Emitter& out, Entity& _entity, Scene& _scene)
{
    out << YAML::BeginMap;
    out << YAML::Key << "GameObject" << YAML::Value;
    out << YAML::BeginMap;
    out << YAML::Key << "m_UUID" << YAML::Key << _entity.uuid;
    out << YAML::Key << "m_Index" << YAML::Value << _entity.denseIndex;
    out << YAML::Key << "m_IsActive" << YAML::Value << 1;

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
    YAML::Node sceneNode = YAML::Load(_scene.filePath.string());
    
    _scene.sceneName = sceneNode["Scene"].as<std::string>();

    for (size_t i = 0; i < sceneNode.size(); i++)
    {
        if (sceneNode[i].IsMap()) // Entity
        {
            
        }
    }
    
    return true;
}

bool DeserializeEntity(YAML::Node& _node, Entity& _entity, Scene& _scene)
{
    return true;
}