/*!***************************************************************************************
\file			FontCompiler.h
\project
\author         Euphrasia Theophelia Tan Ee Mun

\par			Course: GAM300
\date           15/1/2024

\brief
	This file contains the declarations of the following:
	1. Loading of font file and creating a bin version of the file

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef FONTCOMPILER_H
#define FONTCOMPILER_H

#include <filesystem>

#include "Graphics/GraphicsHeaders.h"

void LoadFont(const std::filesystem::path& _filePath);

#endif // !FONTCOMPILER_H
