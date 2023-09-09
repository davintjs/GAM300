/*!***************************************************************************************
\file			YAMLUtils.cpp
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the definitions of the following:
    1. YAML Integration

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "YAMLUtils.h"

template <typename T>
void SerializeBasic(const T& _data, YAML::Emitter& out, const std::string& _key)
{
    out << YAML::Key << _key << YAML::Value << _data;
}

template<>
void SerializeBasic<bool>(const bool& _data, YAML::Emitter& out, const std::string& _key)
{

}