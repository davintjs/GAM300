/*!***************************************************************************************
\file			YAMLUtils.h
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the declarations of the following:
    1. YAML Integration

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef YAMLUTILS_H
#define YAMLUTILS_H

#include "yaml-cpp/yaml.h"
#include "Properties.h"
#include "UUID.h"

extern class Scene;
extern struct Transform;
extern struct Field;

struct Child
{
    std::vector<Engine::UUID> _transform;
    Scene& _scene;
};

template <typename T>
void SerializeBasic(const T& _data, YAML::Emitter& out, const std::string& _key);

template<>
void SerializeBasic<bool>(const bool& _data, YAML::Emitter& out, const std::string& _key);

YAML::Emitter& operator<<(YAML::Emitter& out, const Field& _data);

YAML::Emitter& operator<<(YAML::Emitter& out, const Child& _data);

YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& v);

YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v);

YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& v);

namespace YAML
{

    template<>
    struct convert<Vector2>
    {
        static Node encode(const Vector2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

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
        static Node encode(const Vector3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

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
        static Node encode(const Vector4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

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
}

#endif // !YAMLUTILS_H