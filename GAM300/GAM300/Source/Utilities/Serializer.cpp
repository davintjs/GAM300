/*!***************************************************************************************
\file			Serializer.cpp
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the definitions of the following:
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
#include "Precompiled.h"
#include "Serializer.h"
#include "Editor/EditorHeaders.h"
#include "Scene/SceneManager.h"
#include "Utilities/ThreadPool.h"
#include "Scene/Identifiers.h"

namespace
{
    const std::string versionID = "1.1";
}

GENERIC_RECURSIVE(void, DeserializeComponent, DeserializeComponent<T>(*((DeComHelper*)pObject)));


bool SerializeScene(Scene& _scene)
{
    YAML::Emitter out;
    std::string versionStr = "%Serializer Version " + versionID;
    out << versionStr;
    // Serialize Scene Settings
    E_ASSERT(SerializeSettings(out, _scene), "Unable To Serialize Entity!\n");

    // Serialize Entities & Components
    for (Engine::UUID euid : _scene.layer)
    {
        //if entity is deleted, dont serialize
        if (_scene.Get<Entity>(euid).state == DELETED) continue;

        bool serialized = SerializeEntity(out, _scene.Get<Entity>(euid), _scene);
        E_ASSERT(serialized, "Unable To Serialize Entity!\n");
    }

    if(!out.good())
        PRINT("Emitter error: ", out.GetLastError(), "\n");

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

bool SerializeSettings(YAML::Emitter& out, Scene& _scene)
{
    out << YAML::BeginMap;
    out << YAML::Key << "Scene Settings" << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "Name" << YAML::Value << _scene.sceneName;
    out << YAML::Key << "m_OcclusionCullingSettings" << YAML::Value << "None";
    out << YAML::Key << "m_RenderSettings" << YAML::Value << "None";
    out << YAML::Key << "m_LightMapSettings" << YAML::Value << "None";
    out << YAML::Key << "m_NavMeshSettings" << YAML::Value << "None";
    out << YAML::EndMap << YAML::EndMap << YAML::Comment("All settings above are just examples");

    return true;
}

bool SerializePrefab(Entity& _entity, Scene& _scene)
{
    YAML::Emitter out;
    std::string name{ _scene.Get<Tag>(_entity).name };
    name = "Assets/" + name;
    name += ".prefab";
    if (fs::exists(name))
    {
        PRINT("FAILED TO SERIALIZE PREFAB AS IT ALREADY EXISTS");
        return false;
    }
    //Success
    if (SerializeEntity(out, _entity, _scene))
    {
        std::ofstream fout(name, std::ios::out);
        fout << out.c_str();

        if (fout.fail())
        {
            fout.close();
            return false;
        }
        fout.close();
    }
}



bool SerializeEntity(YAML::Emitter& out, Entity& _entity, Scene& _scene)
{
    out << YAML::BeginMap;
    out << YAML::Key << "ID" << YAML::Value << _entity.EUID();
    out << YAML::Key << "GameObject" << YAML::Value;
    out << YAML::BeginMap;
    out << YAML::Key << "m_IsActive" << YAML::Value << _scene.IsActive(_entity);

    // Bean: Components are placed in different conditions, maybe implement using templates?
    // Bean: Components should have its own category like Entities, and just loop thru
    auto& tag = _scene.Get<Tag>(_entity);
    out << YAML::Key << "m_Name" << YAML::Value << tag.name;
    out << YAML::Key << "m_TagString" << YAML::Value << IDENTIFIERS.GetTagString(tag.tagName);
    out << YAML::Key << "m_Layer" << YAML::Value << tag.physicsLayerIndex;

    // Serialize components
    out << YAML::Key << "m_Component" << YAML::Value;
    out << YAML::BeginSeq;

    ComponentsSerializer componentSerializer{};
    // Serialize the id of the component
    bool hasSerialized = componentSerializer.SerializeComponents<true>(out, _entity, _scene);

    out << YAML::EndSeq;
    out << YAML::EndMap;
    out << YAML::EndMap;


    // Serialize the actual component after
    hasSerialized = componentSerializer.SerializeComponents<false>(out, _entity, _scene);

    return hasSerialized;
}


template <typename T>
bool SerializeReferenceField(YAML::Emitter& out, T& _component)
{
    // Store UUID for each component
    std::string componentName = GetType::Name<T>();
    out << YAML::BeginMap;
    out << YAML::Key << "component" << YAML::Key << YAML::Flow << YAML::BeginMap;
    out << YAML::Key << "fileID" << YAML::Value << _component.UUID() << YAML::EndMap << YAML::Comment(componentName);
    out << YAML::EndMap;
    return true;
}

template <typename T>
bool SerializeComponent(YAML::Emitter& out, T& _component)
{

    std::string componentName = GetType::Name<T>();

    out << YAML::BeginMap;
    out << YAML::Key << "ID" << YAML::Value << _component.UUID();
    out << YAML::Key << componentName << YAML::Value << YAML::BeginMap;

    out << YAML::Key << "m_GameObject" << YAML::Key << YAML::Flow << YAML::BeginMap;
    out << YAML::Key << "fileID" << YAML::Value << _component.EUID() << YAML::EndMap;
    property::SerializeEnum(_component, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type Flags)
        {
            auto entry = property::entry { PropertyName, Data };
            std::string Name = entry.first;
            if (!Flags.m_isDontSave)
            {
                std::visit([&](auto& Value)
                    {
                        using T1 = std::decay_t<decltype(Value)>;

                        // Edit name
                        auto it = Name.begin() + Name.find_last_of("/");
                        Name.erase(Name.begin(), ++it);
                        // Store Component value
                        if (Flags.m_isReference)
                        {
                            out << YAML::Key << Name << YAML::Key << YAML::Flow << YAML::BeginMap;
                            out << YAML::Key << "fileID" << YAML::Value << Value << YAML::EndMap;
                        }
                        else
                        {
                            if constexpr (std::is_base_of< Engine::HexID, T1>())
                            {
                                out << YAML::Key << Name << YAML::Key << YAML::Flow << YAML::BeginMap;
                                out << YAML::Key << "guid" << YAML::Value << Value << YAML::EndMap;
                            }
                            else
                            {
                                out << YAML::Key << Name << YAML::Value << Value;
                            }
                        }
                    }
                , entry.second);
            }
        });

    if constexpr (std::is_same<T, Transform>())
    {
        out << YAML::Key << "m_Children" << YAML::Value << Child{ _component.child, MySceneManager.GetCurrentScene()};
    }

    if constexpr (std::is_same<T, Script>())
        SerializeScript(out, _component);

    out << YAML::EndMap << YAML::EndMap;

    return true;
}

void Serialize(Material_instance& material, std::string directory)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Material" << YAML::Value;
    out << YAML::BeginMap;
    property::SerializeEnum(material, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type Flags)
    {
        auto entry = property::entry { PropertyName, Data };
        std::string Name = entry.first;
        if (!Flags.m_isDontSave || !Flags.m_isShowReadOnly)
        {
            std::visit([&](auto& Value)
                {
                    using T = std::decay_t<decltype(Value)>;

                    // Edit name
                    auto it = Name.begin() + Name.find_last_of("/");
                    Name.erase(Name.begin(), ++it);
                    if constexpr (std::is_same<T, Engine::GUID<TextureAsset>>())
                    {
                        out << YAML::Key << Name << YAML::Key << YAML::Flow << YAML::BeginMap;
                        out << YAML::Key << "guid" << YAML::Value << Value << YAML::EndMap;
                    }
                    else
                    {
                        out << YAML::Key << Name << YAML::Value << Value;
                    }
                }
            , entry.second);
        }
    });
    out << YAML::EndMap << YAML::EndMap;
    directory += "/";
    std::ofstream fout(directory + material.name + ".material", std::ios::out);
    fout << out.c_str();
}

void Deserialize(Material_instance& material,const fs::path& path)
{
    std::vector<YAML::Node> data = YAML::LoadAllFromFile(path.string());
    //PRINT(path);
    YAML::Node node = data[0]["Material"];
    property::SerializeEnum(material, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type Flags)
        {
            auto entry = property::entry { PropertyName, Data };
            std::visit([&](auto& Value)
            {
                using T1 = std::decay_t<decltype(Value)>;
                std::string name = entry.first;
                // Edit name
                auto it = name.begin() + name.find_last_of("/");
                name.erase(name.begin(), ++it);
                // Edit name
                if (node[name])
                {
                    if constexpr (std::is_same<char*, T1>()) {
                        std::string buf = node[name].as<std::string>();
                        property::set(material, entry.first.c_str(), buf);

                    }
                    else
                    {
                        if constexpr (std::is_same<T1, Engine::GUID<TextureAsset>>())
                        {
                            property::set(material, entry.first.c_str(), node[name]["guid"].as<T1>());
                        }
                        else
                        {
                            property::set(material, entry.first.c_str(), node[name].as<T1>());
                        }
                    }
                }
            }
            , entry.second);
        });
}

void SerializeScript(YAML::Emitter& out, Script& _component)
{
    ScriptGetFieldNamesEvent fieldNamesEvent{_component};
    EVENTS.Publish(&fieldNamesEvent);
    for (size_t i = 0; i < fieldNamesEvent.count; ++i)
    {
        static char buffer[2048]{};
        Field field{ AllComponentTypes::Size(),2048,buffer};
        const char* name{ fieldNamesEvent.pStart[i] };
        ScriptGetFieldEvent getFieldEvent{_component,name,field};
        EVENTS.Publish(&getFieldEvent);
        out << YAML::Key << name;
        SerializeScriptHelper(field, out, AllFieldTypes());
    }
}

void DeserializePrefab(const fs::path& path,Scene& _scene)
{
    std::vector<YAML::Node> data = YAML::LoadAllFromFile(path.string());
    for (size_t i = 2; i < data.size(); i++)
    {
        YAML::Node node = data[i];
        if (node["GameObject"]) // Deserialize Gameobject
        {
            DeserializeEntity(node, _scene);
        }
        else // Deserialize component
        {
            // Check which component this node is

            YAML::detail::iterator_value kv = *(++node.begin());
            std::string name = kv.first.as<std::string>();
            //PRINT("Loading " + name + " component... \n");
            DeComHelper helper{ &node, &_scene, false };
            DeserializeComponent(ComponentTypes[name], &helper);
        }
    }
    DeserializeLinker(_scene, data);
}


bool DeserializeScene(Scene& _scene)
{
    std::vector<YAML::Node> data;

    // Load the scene again for linkages
    try
    {
        data = YAML::LoadAllFromFile(_scene.filePath.string());
    }
    catch (YAML::ParserException e)
    {
        std::string exception = "Failed to load .scene file \"" + _scene.filePath.string() + "\" due to: " + e.what() + "\n";
        E_ASSERT(false, exception);
    }
    catch (YAML::BadFile e)
    {
        std::string exception = "Failed to load .scene file \"" + _scene.filePath.string() + "\" due to: " + e.what() + "\n";
        E_ASSERT(false, exception);
    }

    // Deserialize Scene Settings
    if (data[0])
    {
        std::string versioning = data[0].as<std::string>();
        if (versioning.find(versionID) == std::string::npos)
            PRINT("Warning! Serializer version ", versioning, " is different from ", versionID, ", update the serializer!\n");
    }

    E_ASSERT(DeserializeSettings(data[1], _scene), "Error Deserializing scene!\n");

    // Start from 1 since 0 is the versioning and 1 is for scene settings
    for (size_t i = 2; i < data.size(); i++)
    {
        YAML::Node node = data[i];
        if (node["GameObject"]) // Deserialize Gameobject
        {
            DeserializeEntity(node, _scene);
        }
        else // Deserialize component
        {
            // Check which component this node is
            
            YAML::detail::iterator_value kv = *(++node.begin());
            std::string name = kv.first.as<std::string>();
            //PRINT("Loading " + name + " component... \n");
            DeComHelper helper{ &node, &_scene, false };
            DeserializeComponent(ComponentTypes[name], &helper);
        }
    }

    DeserializeLinker(_scene, data);
    return true;
}

bool DeserializeSettings(YAML::Node& _node, Scene& _scene)
{
    if (!_node["Scene Settings"])
    {
        PRINT("Not a .scene file!\n");
        return false;
    }

    _scene.sceneName = _node["Scene Settings"]["Name"].as<std::string>();
    PRINT("Deserializing scene \"" + _scene.sceneName + "\"\n");

    return true;
}

void DeserializeEntity(YAML::Node& _node, Scene& _scene)
{
    YAML::Node object = _node["GameObject"];
    
    Engine::UUID euid = _node["ID"].as<Engine::UUID>();
    Entity& refEntity = *_scene.Add<Entity>(euid);

    _scene.SetActive(refEntity, object["m_IsActive"].as<bool>());

    _scene.Get<Tag>(refEntity).name = object["m_Name"].as<std::string>(); // Name
    _scene.Get<Tag>(refEntity).tagName = IDENTIFIERS.GetTags()[object["m_TagString"].as<std::string>()]; // Name

    if(object["m_Layer"])
        _scene.Get<Tag>(refEntity).physicsLayerIndex = object["m_Layer"].as<size_t>(); // Layer
}

template <typename T>
void DeserializeComponent(const DeComHelper& _helper)
{
    Engine::UUID uuid = (*_helper.node)["ID"].as<Engine::UUID>();
    T component{};
    component.UUID(uuid);
    //if constexpr (std::is_same<T, AudioSource>())
    //    PRINT("HELLO");
    YAML::Node node = (*_helper.node)[GetType::Name<T>()];
    Scene& _scene = *_helper.scene;
    Engine::UUID euid = node["m_GameObject"]["fileID"].as<Engine::UUID>();
    component.EUID(euid);
    // Assign to the component
    property::SerializeEnum(component, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type Flags)
        {
            auto entry = property::entry { PropertyName, Data };
            std::string name = entry.first;
            std::visit([&](auto& Value)
                {
                    using T1 = std::decay_t<decltype(Value)>;

                    // Edit name
                    auto it = name.begin() + name.find_last_of("/");
                    name.erase(name.begin(), ++it);

                    // Extract Component value
                    if (Flags.m_isReference)
                    {
                        if constexpr (!std::is_same<char*, T1>()) {
                            property::set(component, entry.first.c_str(), node[name]["fileID"].as<T1>());

                        }
                    }
                    else
                    {
                        if (node[name])
                        {
                            if constexpr (std::is_same<char*, T1>()) {
                                std::string buf = node[name].as<std::string>();
                                property::set(component, entry.first.c_str(), buf);

                            }
                            else
                            {
                                
                                if constexpr (std::is_base_of< Engine::HexID,T1>())
                                {
                                    if (node[name]["guid"])
                                        property::set(component, entry.first.c_str(), node[name]["guid"].as<T1>());
                                }
                                else
                                {
                                    property::set(component, entry.first.c_str(), node[name].as<T1>());
                                }
                            }
                        }
                    }
                }
            , entry.second);
        });

    if (!_helper.linker)
    {
        // Store in entity

        auto id = component.EUID();
        Entity& entity = _scene.Get<Entity>(id);

        if (&entity)
        {
            // Check for Scripts
            if constexpr (std::is_same<Transform, T>())
            {
                T& transform = _scene.Get<T>(component.EUID());
                transform = component;
            }
            else if constexpr (std::is_same<Tag, T>())
            {

            }
            else if constexpr (std::is_same<T, Script>())
            {
                _scene.Add<T>(component.EUID(),component.UUID(),nullptr,component.scriptId);
            }
            else
            {
                T* pComponent = _scene.Add<T>(component.EUID(), component.UUID());
                *pComponent = component;
            }
        }
        else
            E_ASSERT(false, "Entity does not exist in this scene! Either the EUID provided or the scene is invalid!");
    }
    else
    {
        if constexpr (std::is_same<T, Transform>())
        {
            if (component.parent)
            {
                MySceneManager.GetCurrentScene().Get<Transform>(component.parent).child.push_back(component.EUID());
            }
        }
        if constexpr (std::is_same<T, Script>())
        {
            T& script = _scene.Get<T>(component.EUID(), component.UUID());
            ScriptGetFieldNamesEvent fieldNamesEvent{ script };
            EVENTS.Publish(&fieldNamesEvent);
            // Assigning script values from the loaded scene
            for (size_t i = 0; i < fieldNamesEvent.count; ++i)
            {
                static char buffer[2048]{};
                Field field{ AllComponentTypes::Size(),2048,buffer };
                const char* name{ fieldNamesEvent.pStart[i] };
                YAML::Node varNode = node[name];
                if (!varNode)
                    continue;
                ScriptGetFieldEvent getFieldEvent{ script,name,field };
                EVENTS.Publish(&getFieldEvent);
                DeserializeScriptHelper(field, varNode, AllFieldTypes());
                ScriptSetFieldEvent setFieldEvent{ script,name,field };
                EVENTS.Publish(&setFieldEvent);
            }
        }
        //E_ASSERT(false, "Component does not exist in this scene! Either the EUID/UUID provided or the scene is invalid!");
    }
}

void DeserializeLinker(Scene& _scene, const std::vector<YAML::Node>& _data)
{
    // Start from 1 since 0 is the versioning and 1 is for scene settings
    for (size_t i = 2; i < _data.size(); i++)
    {
        YAML::Node node = _data[i];

        if (!node["GameObject"]) // Deserialize component
        {
            // Check which component this node is
            YAML::detail::iterator_value kv = *(++node.begin());
            std::string name = kv.first.as<std::string>();
            //PRINT("Loading " + name + " component... \n");
            DeComHelper helper{ &node, &_scene, true };
            DeserializeComponent(ComponentTypes[name], &helper);
        }
    }
}

template <typename T, typename... Ts>
void SerializeScriptHelper(Field& rhs, YAML::Emitter& out)
{
    if (rhs.fType == GetFieldType::E<T>())
    {
        if constexpr (AllObjectTypes::Has<T>())
        {
            out << YAML::Flow << YAML::BeginMap;
            out << YAML::Key << "fileID";
            // Storing EUID and UUID of Objects (Gameobject, Components etc)
            T*& object = *reinterpret_cast<T**>(rhs.data);
            if (!object)
            {
                out << YAML::Value << 0 << YAML::EndMap;
                
            }
            else
            {
                if constexpr (std::is_same<T, Entity>())
                    out << YAML::Value << object->EUID() << YAML::EndMap << YAML::Comment("GameObject");
                else
                {
                    if constexpr (std::is_same_v<T, Transform> || std::is_same_v<T, Tag>)
                    {
                        out << YAML::Value << object->EUID() << YAML::EndMap << YAML::Comment("Component");
                    }
                    else
                    {
                        out << YAML::Value << object->UUID() << YAML::EndMap << YAML::Comment("Component");
                    }
                }
            }
        }
        else
        {
            if constexpr (std::is_same_v<char*,T>)
            {
                std::string str = (char*)rhs.data;
                out << YAML::Value << str;
            }
            else
            {
                // Store Basic Types
                T& value = rhs.Get<T>();
                out << YAML::Value << value;
            }
        }

        return;
    }

    if constexpr (sizeof...(Ts) != 0)
    {
        SerializeScriptHelper<Ts...>(rhs, out);
    }
}

template <typename T, typename... Ts>
void SerializeScriptHelper(Field& rhs, YAML::Emitter& out, TemplatePack<T, Ts...>)
{
    SerializeScriptHelper<T, Ts...>(rhs, out);
}

template <typename T, typename... Ts>
void DeserializeScriptHelper(Field& rhs, YAML::Node& node)
{
    //YAML::Emitter out;
    //out << node;
    //PRINT(out.c_str(), '\n');
    if (rhs.fType == GetFieldType::E<T>())
    {
        if constexpr (AllObjectTypes::Has<T>())
        {
            // Storing EUID and UUID of Objects (Gameobject, Components etc)
            Scene& scene{ MySceneManager.GetCurrentScene() };
            T*& object = *reinterpret_cast<T**>(rhs.data);
            if constexpr (std::is_same<T, Entity>())
            {
                Engine::UUID euid = node["fileID"].as<Engine::UUID>();
                if (euid)
                    object = &scene.Get<T>(euid);
            }
            else
            {
                Engine::UUID uuid = node["fileID"].as<Engine::UUID>();
                if (uuid)
                {
                    if constexpr (std::is_same_v<T, Transform> || std::is_same_v<T, Tag>)
                    {
                        object = &scene.Get<T>(uuid);
                    }
                    else
                    {
                        
                        object = &scene.GetByUUID<T>(uuid);
                    }
                }
            }
        }
        else
        {
            if constexpr (std::is_same_v<char*, T>)
            {
                std::string value = (char*)rhs.data;
                value = node.as<std::string>();
                strcpy((char*)rhs.data,value.data());
            }
            else
            {
                // Store Basic Types
                T& value = rhs.Get<T>();
                value = node.as<T>();
            }
        }
        return;
    }

    if constexpr (sizeof...(Ts) != 0)
    {
        DeserializeScriptHelper<Ts...>(rhs, node);
    }
}

template <typename T, typename... Ts>
void DeserializeScriptHelper(Field& rhs, YAML::Node& node, TemplatePack<T, Ts...>)
{
    DeserializeScriptHelper<T, Ts...>(rhs, node);
}