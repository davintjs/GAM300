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

namespace
{
    const std::string versionID = "1.1";
}

GENERIC_RECURSIVE(void, DeserializeComponent, DeserializeComponent<T>(*((DeComHelper*)pObject)));


void Serialize(const std::string& _filepath)
{
    (void)_filepath;
}

void SerializeRuntime(const std::string& _filepath)
{
    (void)_filepath;
}

bool SerializeScene(Scene& _scene)
{
    YAML::Emitter out;
    std::string versionStr = "Serializer Version " + versionID;
    out << versionStr;

    // Serialize Scene Settings
    E_ASSERT(SerializeSettings(out, _scene), "Unable To Serialize Entity!\n");

    // Serialize Entities & Components
    for (Engine::UUID euid : _scene.layer)
    {
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

bool SerializeEntity(YAML::Emitter& out, Entity& _entity, Scene& _scene)
{
    out << YAML::BeginMap;
    out << YAML::Key << "GameObject" << YAML::Value;
    out << YAML::BeginMap;
    out << YAML::Key << "m_EUID" << YAML::Value << _entity.EUID();
    out << YAML::Key << "m_IsActive" << YAML::Value << _scene.IsActive(_entity);

    // Bean: Components are placed in different conditions, maybe implement using templates?
    // Bean: Components should have its own category like Entities, and just loop thru
    if (_scene.Has<Tag>(_entity))
    {
        auto& component = _scene.Get<Tag>(_entity);
        out << YAML::Key << "m_Name" << YAML::Value << component.name;
    }

    if (_scene.Has<Transform>(_entity))
    {
        auto& component = _scene.Get<Transform>(_entity);
        out << YAML::Key << "m_Position" << YAML::Value << component.translation;
        out << YAML::Key << "m_Rotation" << YAML::Value << component.rotation;
        out << YAML::Key << "m_Scale" << YAML::Value << component.scale;
        out << YAML::Key << "m_Children" << YAML::Value << Child{component.child, _scene};

        if (component.parent)
            out << YAML::Key << "m_Parent" << YAML::Value << component.parent;
        else
            out << YAML::Key << "m_Parent" << YAML::Value << 0;
    }

    // Serialize components
    out << YAML::Key << "m_Components" << YAML::Value;
    out << YAML::BeginSeq;

    SerializeAllComponentsStruct componentSerializer{};
    // Serialize the id of the component
    bool hasSerialized = componentSerializer.SerializeComponents(out, _entity, _scene);

    out << YAML::EndSeq;
    out << YAML::EndMap;
    out << YAML::EndMap;

    // Serialize the actual component after
    hasSerialized = componentSerializer.SerializeComponents(out, _entity, _scene, false);

    return hasSerialized;
}

template <typename T>
bool SerializeComponent(YAML::Emitter& out, T& _component, const bool& _id)
{
    if (_id)
    {
        // Store UUID for each component
        std::string componentName = GetType::Name<T>();
        out << YAML::BeginMap;
        out << YAML::Key << "component" << YAML::Key << YAML::Flow << YAML::BeginMap;
        out << YAML::Key << "fileID" << YAML::Value << _component.UUID() << YAML::EndMap << YAML::Comment(componentName);
        out << YAML::EndMap;
        return true;
    }

    std::string componentName = GetType::Name<T>();

    out << YAML::BeginMap;
    out << YAML::Key << componentName << YAML::Value << YAML::BeginMap;

    property::SerializeEnum(_component, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type Flags)
        {
            if (!Flags.m_isDontSave)
            {
                auto entry = property::entry { PropertyName, Data };
                std::visit([&](auto& Value)
                    {
                        using T = std::decay_t<decltype(Value)>;

                        // Edit name
                        std::string Name = entry.first;
                        auto it = Name.begin() + Name.find_last_of("/");
                        Name.erase(Name.begin(), ++it);

                        // Store Component value
                        out << YAML::Key << Name << YAML::Value << Value;
                    }
                , entry.second);
            }
        });

    if constexpr (std::is_same<T, Script>())
        SerializeScript(out, _component);

    out << YAML::EndMap << YAML::EndMap;

    return true;
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

void Deserialize(const std::string& _filepath)
{
    (void)_filepath;
}

void DeserializeRuntime(const std::string& _filepath)
{
    (void)_filepath;
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
        versioning.erase(0, 19);
        if (versioning.compare(versionID))
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
            YAML::detail::iterator_value kv = *node.begin();
            std::string name = kv.first.as<std::string>();
            //PRINT("Loading " + name + " component... \n");

            YAML::Node component = node[name];
            DeComHelper helper{ &component, &_scene, false };
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

void DeserializeEntity(YAML::Node& _node, Scene& _scene, bool _linking)
{
    YAML::Node object = _node["GameObject"];

    if (!_linking)
    {
        Entity& refEntity = *_scene.Add<Entity>(object["m_EUID"].as<Engine::UUID>());

        _scene.SetActive(refEntity, object["m_IsActive"].as<bool>());

        // Create a constructor for entity with transform and tag
        Transform& transform = _scene.Get<Transform>(refEntity); // Transform
        transform.translation = object["m_Position"].as<Vector3>();
        transform.rotation = object["m_Rotation"].as<Vector3>();
        transform.scale = object["m_Scale"].as<Vector3>();
        _scene.Get<Tag>(refEntity).name = object["m_Name"].as<std::string>(); // Tag
    }
    else // Linking parent and child gameobjects
    {
        Entity& refEntity = _scene.Get<Entity>(object["m_EUID"].as<Engine::UUID>());
        YAML::Node parent = object["m_Parent"];
        if (parent)
        {
            Engine::UUID parentUUID = parent.as<Engine::UUID>();
            if (parentUUID != 0)
            {
                Transform& transform = _scene.Get<Transform>(refEntity); // Transform
                transform.SetParent(&_scene.Get<Transform>(parentUUID));
            }
        }
        else
            E_ASSERT(false, "No parent node found in gameobject!\n");
    }
}

template <typename T>
void DeserializeComponent(const DeComHelper& _helper)
{
    T component{};
    YAML::Node& node = *_helper.node;
    Scene& _scene = *_helper.scene;

    // Assign to the component
    property::SerializeEnum(component, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type)
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
                    if (node[name])
                    {
                        if constexpr (std::is_same<char*, T1>()) {
                            std::string buf = node[name].as<std::string>();
                            property::set(component, entry.first.c_str(), buf);

                        }
                        else
                            property::set(component, entry.first.c_str(), node[name].as<T1>());
                    }
                    else
                    {
                        // Bean: Rework this into something usable, right now if the property is not in the scene file,
                        //      it will assert, instead it should be ignored
                        //E_ASSERT(false, "Key: ", name, " of type ", typeid(T1).name(), " does not exist within this component!");
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
            if constexpr (std::is_same<T, Script>())
            {
                _scene.Add<T>(component.EUID(),component.UUID(), component.name.c_str());
            }
            else
            {
                T& tempComponent = *_scene.Add<T>(entity);
                tempComponent = component;
            }
        }
        else
            E_ASSERT(false, "Entity does not exist in this scene! Either the EUID provided or the scene is invalid!");
    }
    else
    {
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

        if (node["GameObject"]) // Deserialize Gameobject
        {
            DeserializeEntity(node, _scene, true);
        }
        else // Deserialize component
        {
            // Check which component this node is
            YAML::detail::iterator_value kv = *node.begin();
            std::string name = kv.first.as<std::string>();

            YAML::Node component = node[name];
            DeComHelper helper{ &component, &_scene, true };
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