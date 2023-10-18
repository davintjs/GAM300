/*!***************************************************************************************
\file			Application.cpp
\project
\author         Everyone

\par			Course: GAM300
\date           28/08/2023

\brief
	This file contains the definitions of the following:
	1. Application

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include <glm/gtx/string_cast.hpp>

#include "Core/EngineCore.h"
#include "Core/EventsManager.h"

#include "Application.h"

#include "IOManager/Handler_GLFW.h"
#include "stb/stb_image.h"
#include <glm/gtc/type_ptr.hpp> // for glm functions

#include "../Source/Graphics/Model3d.h"
#include "Editor/EditorCamera.h"

void Application::Init()
{

	/// <This should be like some sort of configuration file?>
	GLFW_Handler::width = 1600;
	GLFW_Handler::height = 900;
	/// <end>

	//Initializing OpenGL Context with OpenGL 4.5 and creating window
	if (!GLFW_Handler::init(GLFW_Handler::width, GLFW_Handler::height, "Bean Factory")) {
		std::cout << "Unable to create OpenGL context" << std::endl;
		std::exit(EXIT_FAILURE);
	}

}

void Application::Exit()
{
	GLFW_Handler::cleanup();
	
	// Debug
	std::cout << "Cleaned up Properly" << std::endl;
}

Application* Application::CreateApp()
{
	return new Application;
}