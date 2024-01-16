/*!***************************************************************************************
\file			TextureCompiler.h
\project
\author         Euphrasia Theophelia Tan Ee Mun

\par			Course: GAM300
\date           08/12/2023

\brief
	This file contains the declarations of the following:
	1. Loading of Texture files
	2. Creating a dds version of the file

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef TEXTURECOMPILER_H
#define TEXTURECOMPILER_H

#include <filesystem>

#include "Core/SystemInterface.h"

void LoadTexture(const std::filesystem::path& _filePath);

#endif // !TEXTURECOMPILER_H
