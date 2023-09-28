/*!***************************************************************************************
\file			Serializer.h
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the declarations of the following:
    1. Scene Serialization
        a. Scene Settings
        b. Entity
        c. Components
    2. Scene Deserialization
        a. Scene Settings
        b. Entity
        c. Components
    3. Clone Helper for De/Serialization which copies fields for the script component

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "YAMLUtils.h"
#include "Properties.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

// Bean: May be added in the future for modularity of serialization
//enum CLASSID
//{
//    OBJECT,
//    GAMEOBJECT,
//    COMPONENT
//};

// Bean: May be added in the future when deserializing components
//struct DeComponent
//{
//    YAML::Node* node;
//    Scene* scene;
//    std::map<Entity*, Engine::UUID>* entityRef;
//    std::map<Entity*, Engine::UUID>* componentRef;
//};

// Encapsulation for all different types of serializing like Scene, Prefab, NavMesh Data etc
void Serialize(const std::string& _filepath);

// Serialization for Prefabs, Navmesh & Scene
void SerializeRuntime(const std::string& _filepath);

// Serialize the scene
bool SerializeScene(Scene& _scene);

// Serialize the entities in the specific scene
bool SerializeEntity(YAML::Emitter& out, Entity& _entity, Scene& _scene);

// Serialize the components in the specific scene, the id parameter checks for serialization of components
// for the GameObject or for mainly the Component
template <typename T>
bool SerializeComponent(YAML::Emitter& out, T& _component, const bool& _id);

// Serialization specifically for scripts
void SerializeScript(YAML::Emitter& out, Script& _component);

// Encapsulation for all different types of deserializing like Scene, Prefab, NavMesh Data etc
void Deserialize(const std::string& _filepath);

// Deserialization for Prefabs, Navmesh & Scene
void DeserializeRuntime(const std::string& _filepath);

// Deserialize the scene
bool DeserializeScene(Scene& _scene);

// Deserialize the entities in the specific scene
void DeserializeEntity(YAML::Node& _node, Scene& _scene, std::map<Entity*, Engine::UUID>& _childEntities);

// Deserialize the components in the specific scene, the id parameter checks for deserialization of components
// for the GameObject or for mainly the Component
template <typename T>
void DeserializeComponent(std::pair<YAML::Node*, Scene*> pair);

// Helper function for serializing the script component
template <typename T, typename... Ts>
void CloneHelper(Field& rhs, YAML::Emitter& out);

// Helper function for serializing the script component
template <typename T, typename... Ts>
void CloneHelper(Field& rhs, YAML::Emitter& out, TemplatePack<T, Ts...>);

// Helper function for deserializing the script component
template <typename T, typename... Ts>
void CloneHelper(Field& rhs, YAML::Node& node);

// Helper function for deserializing the script component
template <typename T, typename... Ts>
void CloneHelper(Field& rhs, YAML::Node& node, TemplatePack<T, Ts...>);

// This struct contains functions which serializes components without using a switch case
template<typename T, typename... Ts>
struct SerializeComponentsStruct
{
public:
    // Default constructors
    constexpr SerializeComponentsStruct(TemplatePack<T, Ts...> pack) {}
    SerializeComponentsStruct() = default;
    
    // Serializing all components in the scene
    bool SerializeComponents(YAML::Emitter& out, Entity& _entity, Scene& _scene, const bool& _id = true)
    {
        return SerializeNext<T, Ts...>(out, _entity, _scene, _id);
    }
private:
    // Next component to serialize
    template<typename T1, typename... T1s>
    bool SerializeNext(YAML::Emitter& out, Entity& _entity, Scene& _scene, const bool& _id)
    {
        if constexpr (SingleComponentTypes::Has<T1>())
        {
            if (_scene.Has<T1>(_entity))
            {
                auto& component = _scene.Get<T1>(_entity);
                if (!SerializeComponent(out, component, _id))
                    return false;
            }
        }
        else if constexpr (MultiComponentTypes::Has<T1>())
        {
            auto components = _scene.GetMulti<T1>(_entity);
            for (T1* component : components)
            {
                if (!SerializeComponent(out, *component, _id))
                    return false;
            }
        }

        if constexpr (sizeof...(T1s) != 0)
        {
            if (!SerializeNext<T1s...>(out, _entity, _scene, _id))
                return false;
        }

        return true;
    }
};
using SerializeAllComponentsStruct = decltype(SerializeComponentsStruct(AllComponentTypes()));

#endif // !SERIALIZER_H