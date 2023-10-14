/*!***************************************************************************************
\file			Platform_Utils.cpp
\project
\author         Joseph Ho

\par			Course: GAM300
\date           07/09/2023

\brief
	This file contains the defintions for FileDialogs functions for the loading/saving of data
	from the engine into data files.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "PlatformUtils.h"
#include "../IOManager/Handler_GLFW.h"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <commdlg.h>
#include "ThreadPool.h"


	std::string FileDialogs::OpenFile(const char* filter) {
		ACQUIRE_SCOPED_LOCK(Assets);
		OPENFILENAMEA ofn;
		CHAR filesz[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(GLFW_Handler::ptr_window);
		ofn.lpstrFile = filesz;
		ofn.nMaxFile = sizeof(filesz);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;

		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE) {
			return ofn.lpstrFile;
		}

		return std::string();
	}

	std::string FileDialogs::SaveFile(const char* filter) {
		ACQUIRE_SCOPED_LOCK(Assets);
		OPENFILENAMEA ofn;
		CHAR filesz[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(GLFW_Handler::ptr_window);
		ofn.lpstrFile = filesz;
		ofn.nMaxFile = sizeof(filesz);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;

		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE) {
			return ofn.lpstrFile;
		}

		return std::string();
	}
