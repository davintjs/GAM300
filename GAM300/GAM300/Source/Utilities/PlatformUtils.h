/*!***************************************************************************************
\file			Platform_Utils.h
\project
\author         Joseph Ho

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the declarations for FileDialogs for the loading/saving of data
	from the engine into data files.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <string>


class FileDialogs {
public:
	static std::string OpenFile(const char* filter);
	static std::string SaveFile(const char* filter);
};


#endif PLATFORM_UTILS_H