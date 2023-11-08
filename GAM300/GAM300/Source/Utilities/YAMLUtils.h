/*!***************************************************************************************
\file			YAMLUtils.h
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the declarations of the following:
    1. YAML Helper Functions
        a. Vector 2
        b. Vector 3
        c. Vector 4
        d. The transform component Child data 
    2. YAML Encoder
        a. Vector 2
        b. Vector 3
        c. Vector 4
    3. YAML Decoder
        a. Vector 2
        b. Vector 3
        c. Vector 4

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef YAMLUTILS_H
#define YAMLUTILS_H

#include "yaml-cpp/yaml.h"

#include "Properties.h"
#include "UUID.h"
#include "GUID.h"
#include "AssetManager/ImporterTypes.h"

struct Scene;
struct Transform;
struct Field;

struct Child
{
    std::vector<Engine::UUID> _transform;
    Scene& _scene;
};

// Overload for output for the Emitter specifically for vector of guids
template <typename T>
YAML::Emitter& operator<<(YAML::Emitter& out, const std::vector<Engine::GUID<T>>& _data)
{
    out << YAML::BeginMap;
    for (Engine::GUID data : _data)
    {
        out << YAML::Key << "data" << YAML::Key << YAML::Flow << YAML::BeginMap;
        out << YAML::Key << "guid" << YAML::Value << data << YAML::EndMap;
    }
    out << YAML::EndMap;
    return out;
}

// Overload for output for the Emitter specifically for model importer struct
YAML::Emitter& operator<<(YAML::Emitter& out, const ModelImporter& _data);

// Overload for output for the Emitter specifically for child struct
YAML::Emitter& operator<<(YAML::Emitter& out, const Child& _data);

// Overload for output for the Emitter specifically for Vector2
YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& v);

// Overload for output for the Emitter specifically for Vector3
YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v);

// Overload for output for the Emitter specifically for Vector4
YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& v);

// Overload for output for the Emitter specifically for GUID
template <typename T>
YAML::Emitter& operator<<(YAML::Emitter& out, const Engine::GUID<T>& v)
{
    out << v.ToHexString();
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Engine::HexID& v);

// Overload for output for the Emitter specifically for Vector4
YAML::Emitter& operator<<(YAML::Emitter& out, char*& v);

namespace YAML
{
    template<typename T>
    struct convert<std::vector<Engine::GUID<T>>>
    {
        // Encoding for vector of guid during deserialization
        static Node encode(const std::vector<Engine::GUID<T>>& rhs)
        {
            Node node;
            for (Engine::GUID<T> guid : rhs)
                node.push_back(guid);
            return node;
        }

        // Decoding for vector of guid during deserialization
        static bool decode(const Node& node, std::vector<Engine::GUID<T>>& rhs)
        {
            rhs = node.as<std::vector<Engine::GUID<T>>>();
            return true;
        }
    };

    template<>
    struct convert<ModelImporter>
    {
        // Encoding for model importer during deserialization
        static Node encode(const ModelImporter& rhs)
        {
            Node node;
            node.push_back(rhs.meshes);
            node.push_back(rhs.materials);
            node.push_back(rhs.animations);
            return node;
        }

        // Decoding for model importer during deserialization
        static bool decode(const Node& node, ModelImporter& rhs)
        {
            rhs.meshes = node[0].as<std::vector<Engine::GUID<MeshAsset>>>();
            rhs.materials = node[1].as<std::vector<Engine::GUID<MaterialAsset>>>();
            rhs.animations = node[2].as<std::vector<Engine::GUID<AnimationAsset>>>();
            return true;
        }
    };

    template<>
    struct convert<Vector2>
    {
        // Encoding for Vector2 during deserialization
        static Node encode(const Vector2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        // Decoding for Vector2 during deserialization
        static bool decode(const Node& node, Vector2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
            {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Vector3>
    {
        // Encoding for Vector3 during deserialization
        static Node encode(const Vector3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        // Decoding for Vector3 during deserialization
        static bool decode(const Node& node, Vector3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
            {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Vector4>
    {
        // Encoding for Vector4 during deserialization
        static Node encode(const Vector4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        // Decoding for Vector4 during deserialization
        static bool decode(const Node& node, Vector4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
            {
                return false;
            }
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<typename T>
    struct convert<Engine::GUID<T>>
    {
        // Encoding for Vector4 during deserialization
        static Node encode(const Engine::GUID<T>& rhs)
        {
            Node node;
            node = rhs.ToHexString();
            return node;
        }

        // Decoding for Vector4 during deserialization
        static bool decode(const Node& node, Engine::GUID<T>& rhs)
        {
            rhs = Engine::GUID<T>(node.as<std::string>());
            return true;
        }
    };


    template<>
    struct convert<Engine::HexID>
    {
        // Encoding for Vector4 during deserialization
        static Node encode(const Engine::HexID& rhs)
        {
            Node node;
            node = rhs.ToHexString();
            return node;
        }

        // Decoding for Vector4 during deserialization
        static bool decode(const Node& node, Engine::HexID& rhs)
        {
            rhs = Engine::HexID(node.as<std::string>());
            return true;
        }
    };

    //struct CustomAnchor
    //{
    //    std::string str;
    //};

    //inline Emitter& operator<<(Emitter& emitter, const CustomAnchor& v) {
    //    return emitter.Write(v);
    //}
    //template <>
    //struct convert<char*>
    //{
    //    // Encoding for Vector4 during deserialization
    //    /*static Node encode(const char* rhs)
    //    {
    //        Node node;
    //        node.push_back(std::string(rhs));
    //        return node;
    //    }*/

    //    // Decoding for Vector4 during deserialization
    //    static bool decode(const Node& node, char* rhs)
    //    {
    //        static std::string buf = node[0].as<std::string>();
    //        rhs = buf.data();
    //        return true;
    //    }
    //};
}

#endif // !YAMLUTILS_H