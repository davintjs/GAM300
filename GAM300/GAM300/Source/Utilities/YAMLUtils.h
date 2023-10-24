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

struct Scene;
struct Transform;
struct Field;

struct Child
{
    std::vector<Engine::UUID> _transform;
    Scene& _scene;
};

// Overload for output for the Emitter specifically for child struct
YAML::Emitter& operator<<(YAML::Emitter& out, const Child& _data);

// Overload for output for the Emitter specifically for Vector2
YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& v);

// Overload for output for the Emitter specifically for Vector3
YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v);

// Overload for output for the Emitter specifically for Vector4
YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& v);

// Overload for output for the Emitter specifically for GUID
YAML::Emitter& operator<<(YAML::Emitter& out, const Engine::GUID& v);

// Overload for output for the Emitter specifically for Vector4
YAML::Emitter& operator<<(YAML::Emitter& out, char*& v);

namespace YAML
{

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

    template<>
    struct convert<Engine::GUID>
    {
        // Encoding for Vector4 during deserialization
        static Node encode(const Engine::GUID& rhs)
        {
            Node node;
            node.push_back(rhs.ToHexString());
            return node;
        }

        // Decoding for Vector4 during deserialization
        static bool decode(const Node& node, Engine::GUID& rhs)
        {
            PRINT(node[0].as<std::string>());
            rhs = Engine::GUID(node[0].as<std::string>());
            return true;
        }
    };

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