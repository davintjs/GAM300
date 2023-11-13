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

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "YAMLUtils.h"
#include "Properties.h"
#include "Scene/Scene.h"
#include <filesystem>

#include "Graphics/GraphicsHeaders.h"

// Bean: May be added in the future for modularity of serialization
//enum CLASSID
//{
//    OBJECT,
//    GAMEOBJECT,
//    COMPONENT
//};

// Deserializing components helper
struct DeComHelper
{
    YAML::Node* node;
    Scene* scene;
    bool linker;
};

// Serialize the scene
bool SerializeScene(Scene& _scene);

// Serialize the settings in the scene
bool SerializeSettings(YAML::Emitter& out, Scene& _scene);

// Serialize the entities in the specific scene
bool SerializeEntity(YAML::Emitter& out, Entity& _entity, Scene& _scene);

bool SerializePrefab(Entity& _entity, Scene& _scene);

// When serializing ids/pointers
template <typename T>
bool SerializeReferenceField(YAML::Emitter& out, T& _component);

// Serialize the components in the specific scene, the id parameter checks for serialization of components
// for the GameObject or for mainly the Component
template <typename T>
bool SerializeComponent(YAML::Emitter& out, T& _component);

// Serialization specifically for scripts
void SerializeScript(YAML::Emitter& out, Script& _component);

void Serialize(Material_instance& material, std::string directory = "Assets");

void Deserialize(Material_instance& material, const fs::path& path);

template <typename T>
void Serialize(YAML::Emitter& out, T& object)
{
    int containerIndex = 0;
    int containerSize = 0;
    out << YAML::BeginMap;
    property::SerializeEnum(object, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type Flags)
    {
        if (!Flags.m_isDontSave)
        {
            auto entry = property::entry { PropertyName, Data };
            fs::path name{ entry.first };
            std::visit([&](auto& Value)
                {
                    using T1 = std::decay_t<decltype(Value)>;
                    namespace fs = std::filesystem;
                    std::string keyName = name.string();
                    //Has a header
                    if (name.parent_path().empty())
                    {
                        //Deserialize again somehow get the type
                        //T1 object;
                    }
                    //Has no header
                    else
                    {
                        keyName.erase(keyName.begin(), keyName.begin() + keyName.find_last_of('/') + 1);
                    }
                    // Store Component value
                    if (keyName.ends_with("[]"))
                    {
                        if constexpr (std::is_same_v<T1, int>)
                        {
                            keyName.resize(keyName.size() - 2);
                            out << YAML::Key << keyName << YAML::Value << YAML::BeginMap;
                            containerSize = Value;
                        }
                    }
                    else if (keyName.ends_with(']'))
                    {
                        keyName = "data";
                        if constexpr (std::is_base_of_v<Engine::HexID,T1>)
                        {
                            out << YAML::Key << keyName << YAML::Value;
                            out << YAML::Flow << YAML::BeginMap;
                            out << YAML::Key << "guid" << YAML::Value << Value << YAML::EndMap;
                        }
                        else
                        {
                            out << YAML::Key << keyName << YAML::Value << Value;
                        }
                        ++containerIndex;
                        if (containerSize == containerIndex)
                        {
                            out << YAML::EndMap;
                            containerSize = 0;
                            containerIndex = 0;
                        }
                    }
                    else
                    {
                        out << YAML::Key << keyName << YAML::Value << Value ;
                    }
                }
            , entry.second);
        }
    });
    out << YAML::EndMap;
}

template <typename T>
void Serialize(const std::filesystem::path& path, T& object)
{
    YAML::Emitter out;

    Serialize(out, object);

    std::ofstream outFile{ path };
    outFile << out.c_str();
    outFile.close();
}

template <typename T>
bool Deserialize(const std::filesystem::path& path, T& object)
{
    namespace fs = std::filesystem;

    int containerIndex = 0;
    int containerSize = 0;
    if (!std::filesystem::exists(path))
        return false;
    YAML::Node node = YAML::LoadFile(path.string());




    // Assign to the component
    property::DisplayEnum(object, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type Flags)
    {
        auto entry = property::entry { PropertyName, Data };
        fs::path name{ entry.first };
        //PRINT(name, '\n');
        std::visit([&](auto& Value)
            {
                using T1 = std::decay_t<decltype(Value)>;
                namespace fs = std::filesystem;
                std::string keyName = name.string();
                //Has a header
                if (name.parent_path().empty())
                {
                    //Deserialize again somehow get the type
                    //T1 object;
                }
                //Has no header
                else
                {
                    keyName.erase(keyName.begin(), keyName.begin() + keyName.find_last_of('/') + 1);
                }

                // Extract Component value
                if (node[keyName])
                {
                    if constexpr (std::is_same<char*, T1>()) {
                        std::string buf = node[keyName].as<std::string>();
                        property::set(object, entry.first.c_str(), buf);

                    }
                    else
                        property::set(object, entry.first.c_str(), node[keyName].as<T1>());
                }
                else if (keyName.ends_with("[]"))
                {
                    if constexpr (std::is_same_v<T1, int>)
                    {
                        //keyName.resize(keyName.size() - 2);
                        //for (YAML::Node& childNode : node[keyName])
                        //{
                        //}
                        //containerSize = node[keyName].size();
                    }
                }
            }
        , entry.second);
    });


    if constexpr (std::is_same<T, ModelImporter>())
    {
        if (node["meshes"])
        {
            YAML::Node meshes = node["meshes"];
            for (YAML::const_iterator it = meshes.begin(); it != meshes.end(); ++it)
            {
                YAML::Node data = it->second;
                if (data["guid"])
                {
                    Engine::HexID id = data["guid"].as<Engine::HexID>();
                    object.meshes.emplace_back(id);
                }
            }
        }

        if (node["animations"])
        {
            YAML::Node animations = node["animations"];
            for (YAML::const_iterator it = animations.begin(); it != animations.end(); ++it)
            {
                YAML::Node data = it->second;
                if (data["guid"])
                {
                    object.animations.emplace_back(data["guid"].as<Engine::HexID>());
                }
            }
        }

    }

    //property::pack Pack;
//property::Pack(object, Pack);
//float LookUps = -1;         // The very first one is actually not a lookup
//{
//    int iPath = 0;
//    for (const auto& E : Pack.m_lEntry)
//    {
//        // Mark how many pops we are making

//        for (int i = 0; i < E.m_nPaths; ++i, ++iPath)
//        {
//            // Are we dealing with an array type?
//            if (Pack.m_lPath[iPath].m_Index != property::lists_iterator_ends_v)
//            {
//                // Arrays/Lists we get them for free
//                PRINT(Pack.m_lPath[iPath].m_Key,'\n');

//                LookUps -= 1.0f;
//            }
//            else
//            {
//                PRINT(Pack.m_lPath[iPath].m_Key,'\n');
//            }
//        }

//        // Keep track of lookups
//        LookUps += E.m_nPaths;

//        std::visit([&](auto&& Value)
//            {
//                using T = std::decay_t<decltype(Value)>;

//                //PRINT(typeid(T).name());
//            }
//        , E.m_Data);


//        // Make sure everything is align
//        //while ((c++) != 70) printf(" ");
//        //// next line
//        //printf("\n");
//    }
//}
    return true;
}

// Deserialize the scene
bool DeserializeScene(Scene& _scene);

// Deserialize the scene setttings
bool DeserializeSettings(YAML::Node& _node, Scene& _scene);

// Deserialize the entities in the specific scene
void DeserializeEntity(YAML::Node& _node, Scene& _scene);

// Deserialize the components in the specific scene, the id parameter checks for deserialization of components
// for the GameObject or for mainly the Component
template <typename T>
void DeserializeComponent(const DeComHelper& _helper);

// After deserializing the scene, perform this function for linkage
void DeserializeLinker(Scene& _scene, const std::vector<YAML::Node>& _data);

// Helper function for serializing the script component
template <typename T, typename... Ts>
void SerializeScriptHelper(Field& rhs, YAML::Emitter& out);

// Helper function for serializing the script component
template <typename T, typename... Ts>
void SerializeScriptHelper(Field& rhs, YAML::Emitter& out, TemplatePack<T, Ts...>);

// Helper function for deserializing the script component
template <typename T, typename... Ts>
void DeserializeScriptHelper(Field& rhs, YAML::Node& node);

// Helper function for deserializing the script component
template <typename T, typename... Ts>
void DeserializeScriptHelper(Field& rhs, YAML::Node& node, TemplatePack<T, Ts...>);

// This struct contains functions which serializes components without using a switch case
template<typename T, typename... Ts>
struct SerializeComponentsStruct
{
public:
    // Default constructors
    constexpr SerializeComponentsStruct(TemplatePack<T, Ts...> pack) {}
    SerializeComponentsStruct() = default;
    
    // Serializing all components in the scene
    template<bool SerializeReference>
    bool SerializeComponents(YAML::Emitter& out, Entity& _entity, Scene& _scene)
    {
        return SerializeNext<SerializeReference,T, Ts...>(out, _entity, _scene);
    }
private:
    // Next component to serialize
    template<bool SerializeReference,typename T1, typename... T1s>
    bool SerializeNext(YAML::Emitter& out, Entity& _entity, Scene& _scene)
    {
        if constexpr (SingleComponentTypes::Has<T1>())
        {
            if (_scene.Has<T1>(_entity))
            {
                auto& component = _scene.Get<T1>(_entity);
                //skip deleted components 
                if (component.state == DELETED) return true;
                if constexpr (!std::is_same<T1, Tag>())
                {
                    if constexpr (SerializeReference)
                    {
                        if (!SerializeReferenceField(out, component))
                            return false;
                    }
                    else
                    {
                        if (!SerializeComponent(out, component))
                            return false;
                    }
                }
            }
        }
        else if constexpr (MultiComponentTypes::Has<T1>())
        {
            auto components = _scene.GetMulti<T1>(_entity);
            for (T1* component : components)
            {
                //skip deleted components 
                if (component->state == DELETED)  return true;
                if constexpr (SerializeReference)
                {
                    if (!SerializeReferenceField(out, *component))
                        return false;
                }
                else
                {
                    if (!SerializeComponent(out, *component))
                        return false;
                }
            }
        }

        if constexpr (sizeof...(T1s) != 0)
        {
            if (!SerializeNext<SerializeReference,T1s...>(out, _entity, _scene))
                return false;
        }

        return true;
    }
};

using ComponentsSerializer = decltype(SerializeComponentsStruct(AllComponentTypes()));

#endif // !SERIALIZER_H