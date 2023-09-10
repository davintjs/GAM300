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

void Emittion()
{
    YAML::Emitter out;
    //out << "Hello World!";

    //std::cout << "Here's the output YAML:\n" << out.c_str();
}

void CreateYAMLMap(YAML::Emitter& out)
{
    out << YAML::BeginMap;
}

void EndYAMLMap(YAML::Emitter& out, const std::string& filepath)
{
    out << YAML::EndMap;

    std::ofstream fout(filepath);
    fout << out.c_str();
}