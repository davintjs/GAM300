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
        E_ASSERT(serialized, "Unable To Serialize Entity!\n");
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
        auto& component = _scene.Get<Tag>(_entity);
        out << YAML::Key << "m_Name" << YAML::Value << component.name;
    }

    if (_scene.HasComponent<Transform>(_entity))
    {
        auto& component = _scene.Get<Transform>(_entity);
        out << YAML::Key << "m_Position" << YAML::Value << component.translation;
        out << YAML::Key << "m_Rotation" << YAML::Value << component.rotation;
        out << YAML::Key << "m_Scale" << YAML::Value << component.scale;
        out << YAML::Key << "m_Children" << YAML::Value << Child{component.child, _scene};

        if (component.parent)
            out << YAML::Key << "m_Parent" << YAML::Value << _scene.Get<Entity>(*component.parent).EUID();
        else
            out << YAML::Key << "m_Parent" << YAML::Value << 0;
    }

    // Bean: For when components have their own uuid
    out << YAML::Key << "m_Components" << YAML::Value;
    out << YAML::BeginSeq;

    SerializeAllComponentsStruct componentSerializer;
    bool hasSerialized = componentSerializer.SerializeComponents(out, _entity, _scene);

    /*if (_scene.HasComponent<AudioSource>(_entity))
    {
        auto& component = _scene.Get<AudioSource>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_AudioSource" << YAML::Value;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<BoxCollider>(_entity))
    {
        auto& component = _scene.Get<BoxCollider>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_BoxCollider" << YAML::Value;
        out << YAML::BeginMap << YAML::Flow << "X" << component.x << "Y" << component.y << "Z" << component.z << YAML::EndMap;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<SphereCollider>(_entity))
    {
        auto& component = _scene.Get<SphereCollider>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_SphereCollider" << YAML::Value;
        out << YAML::BeginMap << "Radius" << component.radius << YAML::EndMap;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<CapsuleCollider>(_entity))
    {
        auto& component = _scene.Get<CapsuleCollider>(_entity);
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
        auto& component = _scene.Get<Animator>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_Animator" << YAML::Value;
        out << YAML::BeginMap << YAML::EndMap;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<CharacterController>(_entity))
    {
        auto& component = _scene.Get<CharacterController>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_CharacterController" << YAML::Value;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<Script>(_entity))
    {
        auto& component = _scene.Get<Script>(_entity);
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
        auto& component = _scene.Get<MeshRenderer>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_MeshRenderer" << YAML::Value;
        out << YAML::BeginMap << "Name" << component.MeshName << YAML::EndMap;
        out << YAML::EndMap;
    }

    if (_scene.HasComponent<LightSource>(_entity))
    {
        auto& component = _scene.Get<LightSource>(_entity);
        out << YAML::BeginMap;
        out << YAML::Key << "m_LightSource" << YAML::Value;
        out << YAML::BeginMap << "lightingColor" << component.lightingColor << YAML::EndMap;
        out << YAML::EndMap;
    }*/

    out << YAML::EndSeq;
    out << YAML::EndMap;
    out << YAML::EndMap;

    return hasSerialized;
}

template <typename T>
bool SerializeComponent(YAML::Emitter& out, T& _component)
{
    std::string componentName = "m_";
    
    componentName += GetType::Name<T>();

    if constexpr (std::is_same<T, Tag>() || std::is_same<T, Transform>())
        return true;

    out << YAML::BeginMap;
    out << YAML::Key << componentName << YAML::Value << YAML::BeginSeq;

    
    // Store UUID for each component
    // Store reference to gameobject

    property::flags::type flags;
    std::vector<property::entry> List;
    property::SerializeEnum(_component, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, flags)
        {
            // If we are dealing with a scope that is not an array someone may have change the SerializeEnum to a DisplayEnum they only show up there.
            assert(Flags.m_isScope == false || PropertyName.back() == ']');
            List.push_back(property::entry { PropertyName, Data });
        });

    for (auto& [Name, Data] : List)
    {
        std::visit([&](auto& Value)
            {
                using T = std::decay_t<decltype(Value)>;
                
                // Edit name
                auto it = Name.begin() + Name.find_last_of("/");
                Name.erase(Name.begin(), ++it);
                Name[0] = (char)toupper(Name[0]); //Make first letter uppercase
                
                // Store Component value
                out << YAML::BeginMap;
                out << YAML::Key << Name << YAML::Value << Value;
                out << YAML::EndMap;
            }
        , Data);
        property::set(_component, Name.c_str(), Data);
    }

    out << YAML::EndSeq << YAML::EndMap;

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

            Entity& entity = *_scene.Add<Entity>(object["m_EUID"].as<Engine::UUID>());

            // Bean: Dont need cuz when parenting it reorders the object anyways
            _scene.SetActive(entity, object["m_IsActive"].as<bool>());
            
            // Bean: Create a constructor for entity with transform and tag
            Transform& transform = _scene.Get<Transform>(entity);
            _scene.Get<Tag>(entity).name = object["m_Name"].as<std::string>();
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
                    auto& audioSource = *_scene.Add<AudioSource>(entity);
                }

                if (component["m_BoxCollider"])
                {
                    auto node = component["m_BoxCollider"];
                    auto& boxCollider = *_scene.Add<BoxCollider>(entity);
                    boxCollider.x = node["X"].as<float>();
                    boxCollider.y = node["Y"].as<float>();
                    boxCollider.z = node["Z"].as<float>();
                }

                if (component["m_SphereCollider"])
                {
                    auto node = component["m_SphereCollider"];
                    auto& sphereCollider = *_scene.Add<SphereCollider>(entity);
                    sphereCollider.radius = node["Radius"].as<float>();
                }

                if (component["m_CapsuleCollider"])
                {
                    auto node = component["m_CapsuleCollider"];
                    auto& capsuleCollider = *_scene.Add<CapsuleCollider>(entity);
                    capsuleCollider.height = node["Height"].as<float>();
                    capsuleCollider.radius = node["Radius"].as<float>();
                }

                if (component["m_Animator"])
                {
                    auto node = component["m_Animator"];
                    auto& animator = *_scene.Add<Animator>(entity);
                }

                if (component["m_CharacterController"])
                {
                    auto node = component["m_CharacterController"];
                    auto& characterController = *_scene.Add<CharacterController>(entity);
                }

                if (component["m_Script"])
                {
                    auto node = component["m_Script"];
                    auto& script = *_scene.Add<Script>(entity);
                    script.name = node["Name"].as<std::string>();
                }

                if (component["m_MeshRenderer"])
                {
                    auto node = component["m_MeshRenderer"];
                    auto& meshRenderer = *_scene.Add<MeshRenderer>(entity);
                    meshRenderer.MeshName = node["Name"].as<std::string>();
                }

                if (component["m_LightSource"])
                {
                    auto node = component["m_LightSource"];
                    auto& lightSource = *_scene.Add<LightSource>(entity);
                    lightSource.lightingColor = node["lightingColor"].as<glm::vec3>();
                }
            }
        }
    }

    // Link all children with parents
    for (auto& pair : childEntities)
    {
        Transform& transform = _scene.Get<Transform>(*pair.first);
        transform.SetParent(&_scene.Get<Transform>(_scene.Get<Entity>(pair.second)));
    }

    return true;
}

bool DeserializeEntity(YAML::Node& _node, Entity& _entity, Scene& _scene)
{
    return true;
}