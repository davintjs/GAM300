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
    out << YAML::BeginMap;
    out << YAML::Key << "GameObject" << YAML::Value;
    out << YAML::BeginMap;
    out << YAML::Key << "m_EUID" << YAML::Key << _entity.EUID();
    out << YAML::Key << "m_IsActive" << YAML::Value << _scene.IsActive(_entity);

    // Bean: Components are placed in different conditions, maybe implement using templates?
    // Bean: Components should have its own category like Entities, and just loop thru
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
            out << YAML::Key << "m_Parent" << YAML::Value << _scene.GetEntity(*component.parent).EUID();
        else
            out << YAML::Key << "m_Parent" << YAML::Value << 0;
    }

    // Bean: For when components have their own uuid
    out << YAML::Key << "m_Components" << YAML::Value;
    out << YAML::BeginSeq;

    if (_scene.HasComponent<AudioSource>(_entity))
    {
        auto& component = _scene.GetComponent<AudioSource>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_AudioSource" << YAML::Value;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<BoxCollider>(_entity))
    {
        auto& component = _scene.GetComponent<BoxCollider>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_BoxCollider" << YAML::Value;
        out << YAML::BeginMap << YAML::Flow << "X" << component.x << "Y" << component.y << "Z" << component.z << YAML::EndMap;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<SphereCollider>(_entity))
    {
        auto& component = _scene.GetComponent<SphereCollider>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_SphereCollider" << YAML::Value;
        out << YAML::BeginMap << "Radius" << component.radius << YAML::EndMap;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<CapsuleCollider>(_entity))
    {
        auto& component = _scene.GetComponent<CapsuleCollider>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_CapsuleCollider" << YAML::Value;
        out << YAML::BeginMap;
        out << "Height" << component.height;
        out << "Radius" << component.radius;
        out << YAML::EndMap;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<Animator>(_entity))
    {
        auto& component = _scene.GetComponent<Animator>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_Animator" << YAML::Value;
        out << YAML::BeginMap << YAML::EndMap;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<CharacterController>(_entity))
    {
        auto& component = _scene.GetComponent<CharacterController>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_CharacterController" << YAML::Value;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<Script>(_entity))
    {
        auto& component = _scene.GetComponent<Script>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_Script" << YAML::Value;
        out << YAML::BeginMap;
        out << "Name" << component.name;
        out << "Field" << 0;
        out << YAML::EndMap;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<MeshRenderer>(_entity))
    {
        auto& component = _scene.GetComponent<MeshRenderer>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_MeshRenderer" << YAML::Value;
        out << YAML::BeginMap << "Name" << component.MeshName << YAML::EndMap;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<LightSource>(_entity))
    {
        auto& component = _scene.GetComponent<LightSource>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_LightSource" << YAML::Value;
        out << YAML::BeginMap << "lightingColor" << component.lightingColor << YAML::EndMap;
        out << YAML::EndMap;
    }

    out << YAML::EndSeq;
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
            transform.translation = Vector3(object["m_Position"].as<vec3>());
            transform.rotation = Vector3(object["m_Rotation"].as<vec3>());
            transform.scale = Vector3(object["m_Scale"].as<vec3>());
            
            auto parent = object["m_Parent"];
            if (parent)
            {
                Engine::UUID parentUUID = parent.as<Engine::UUID>();
                if (parentUUID != 0)
                    childEntities[&entity] = parentUUID;
            }

            // Bean: Components will be here temporarily
            for (auto component : object["m_Components"])
            {
                if (component["m_AudioSource"])
                {
                    auto node = component["m_AudioSource"];
                    auto& audioSource = _scene.AddComponent<AudioSource>(entity);
                }

                if (component["m_BoxCollider"])
                {
                    auto node = component["m_BoxCollider"];
                    auto& boxCollider = _scene.AddComponent<BoxCollider>(entity);
                    boxCollider.x = node["X"].as<float>();
                    boxCollider.y = node["Y"].as<float>();
                    boxCollider.z = node["Z"].as<float>();
                }

                if (component["m_SphereCollider"])
                {
                    auto node = component["m_SphereCollider"];
                    auto& sphereCollider = _scene.AddComponent<SphereCollider>(entity);
                    sphereCollider.radius = node["Radius"].as<float>();
                }

                if (component["m_CapsuleCollider"])
                {
                    auto node = component["m_CapsuleCollider"];
                    auto& capsuleCollider = _scene.AddComponent<CapsuleCollider>(entity);
                    capsuleCollider.height = node["Height"].as<float>();
                    capsuleCollider.radius = node["Radius"].as<float>();
                }

                if (component["m_Animator"])
                {
                    auto node = component["m_Animator"];
                    auto& animator = _scene.AddComponent<Animator>(entity);
                }

                if (component["m_CharacterController"])
                {
                    auto node = component["m_CharacterController"];
                    auto& characterController = _scene.AddComponent<CharacterController>(entity);
                }

                if (component["m_Script"])
                {
                    auto node = component["m_Script"];
                    auto& script = _scene.AddComponent<Script>(entity);
                    script.name = node["Name"].as<std::string>();
                }

                if (component["m_MeshRenderer"])
                {
                    auto node = component["m_MeshRenderer"];
                    auto& meshRenderer = _scene.AddComponent<MeshRenderer>(entity);
                    meshRenderer.MeshName = node["Name"].as<std::string>();
                }

                if (component["m_LightSource"])
                {
                    auto node = component["m_LightSource"];
                    auto& lightSource = _scene.AddComponent<LightSource>(entity);
                    lightSource.lightingColor = node["lightingColor"].as<glm::vec3>();
                }
            }
        }
    }

    // Link all children with parents
    for (auto& pair : childEntities)
    {
        Transform& transform = _scene.GetComponent<Transform>(*pair.first);
        transform.SetParent(&_scene.GetComponent<Transform>(_scene.GetEntityByUUID(pair.second)));
    }

    return true;
}

bool DeserializeEntity(YAML::Node& _node, Entity& _entity, Scene& _scene)
{
    return true;
}