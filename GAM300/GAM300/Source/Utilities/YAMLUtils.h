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
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Scene;
struct Transform;

struct Child
{
    std::vector<Transform*> _transform;
    Scene& _scene;
};

template <typename T>
void SerializeBasic(const T& _data, YAML::Emitter& out, const std::string& _key);

template<>
void SerializeBasic<bool>(const bool& _data, YAML::Emitter& out, const std::string& _key);

YAML::Emitter& operator<<(YAML::Emitter& out, const Child& _data);

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v);

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v);

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v);

namespace YAML
{
    template<>
    struct convert<Transform>
    {
        
    };

    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
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
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
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
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
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