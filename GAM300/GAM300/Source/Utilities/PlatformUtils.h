#pragma once

/**************************************************************************************/
/*!
//    \file			Platform_Utils.h
//    \author(s) 	Joseph Ho Jun Jie
//
//    \date   	    26 October 2022
//    \brief		This file contains the declaration of class type FileDialogs for the
//					loading/saving of data from the engine into data files.
//
//    \Percentage   Joseph 100%
//
//    Copyright (C) 2022 DigiPen Institute of Technology.
//    Reproduction or disclosure of this file or its contents without the
//    prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /**************************************************************************************/

#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <string>


class FileDialogs {
public:
	static std::string OpenFile(const char* filter);
	static std::string SaveFile(const char* filter);
};


#endif PLATFORM_UTILS_H