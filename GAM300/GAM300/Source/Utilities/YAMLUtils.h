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

struct Transform;

template <typename T>
void SerializeBasic(const T& _data, YAML::Emitter& out, const std::string& _key);

template<>
void SerializeBasic<bool>(const bool& _data, YAML::Emitter& out, const std::string& _key);

YAML::Emitter& operator<<(YAML::Emitter& out, const std::vector<Transform*>& _data);

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v);

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v);

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v);

#endif // !YAMLUTILS_H