/*!***************************************************************************************
\file			Serializer.h
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the declarations of the following:
    1. Serialization

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "YAMLUtils.h"
#include "Properties.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

enum CLASSID
{
    OBJECT,
    GAMEOBJECT,
    COMPONENT
};


void Serialize(const std::string& _filepath);
void SerializeRuntime(const std::string& _filepath);

bool SerializeScene(Scene& _scene);
bool SerializeEntity(YAML::Emitter& out, Entity& _entity, Scene& _scene);

template <typename T>
bool SerializeComponent(YAML::Emitter& out, T& _component, const bool& _id);

void SerializeScript(YAML::Emitter& out, Script& _component);

void Deserialize(const std::string& _filepath);
void DeserializeRuntime(const std::string& _filepath);

bool DeserializeScene(Scene& _scene);
void DeserializeGameObject(YAML::Node& _node, Scene& _scene, std::map<Entity*, Engine::UUID>& _childEntities);

template <typename T>
void DeserializeComponent(std::pair<YAML::Node*, Scene*> pair);

template <typename T, typename... Ts>
void CloneHelper(Field& rhs, YAML::Emitter& out);

template <typename T, typename... Ts>
void CloneHelper(Field& rhs, YAML::Emitter& out, TemplatePack<T, Ts...>);

template<typename T, typename... Ts>
struct SerializeComponentsStruct
{
public:
    constexpr SerializeComponentsStruct(TemplatePack<T, Ts...> pack) {}
    SerializeComponentsStruct() = default;
    bool SerializeComponents(YAML::Emitter& out, Entity& _entity, Scene& _scene, const bool& _id = true)
    {
        return SerializeNext<T, Ts...>(out, _entity, _scene, _id);
    }
private:
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