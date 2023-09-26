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
bool SerializeComponent(YAML::Emitter& out, T& _component);

void Deserialize(const std::string& _filepath);
void DeserializeRuntime(const std::string& _filepath);

bool DeserializeScene(Scene& _scene);
bool DeserializeEntity(YAML::Node& _node, Entity& _entity, Scene& _scene);

template<typename T, typename... Ts>
struct SerializeComponentsStruct
{
public:
    constexpr SerializeComponentsStruct(TemplatePack<T, Ts...> pack) {}
    SerializeComponentsStruct() = default;
    bool SerializeComponents(YAML::Emitter& out, Entity& entity, Scene& _scene)
    {
        return SerializeNext<T, Ts...>(out, entity, _scene);
    }
private:
    template<typename T1, typename... T1s>
    bool SerializeNext(YAML::Emitter& out, Entity& entity, Scene& _scene)
    {
        if constexpr (SingleComponentTypes::Has<T1>())
        {
            if (_scene.HasComponent<T1>(entity))
            {
                //dont Serialize tag component as it is already on top of the inspector
                if constexpr (!std::is_same<T1, Tag>())
                {
                    auto& component = _scene.Get<T1>(entity);
                    if (!SerializeComponent(out, component))
                        return false;
                }
            }
        }
        else if constexpr (MultiComponentTypes::Has<T1>())
        {
            auto components = _scene.GetMulti<T1>(entity);
            for (T1* component : components)
            {
                //SerializeType("Enabled", component->is_enabled); ImGui::SameLine();
                if (!SerializeComponent(out, *component))
                    return false;
            }
        }

        if constexpr (sizeof...(T1s) != 0)
        {
            if (!SerializeNext<T1s...>(out, entity, _scene))
                return false;
        }

        return true;
    }
};
using SerializeAllComponentsStruct = decltype(SerializeComponentsStruct(AllComponentTypes()));

#endif // !SERIALIZER_H