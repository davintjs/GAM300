#include "Precompiled.h"
#include <glm/gtx/string_cast.hpp>
//#include "GL/glew.h"
//#include "../../glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h"

#include "Application.h"

#include "IOManager/Handler_GLFW.h"
#include "stb/stb_image.h"
#include <glm/gtc/type_ptr.hpp> // for glm functions

#include "LapSupGraphics/Compiler.h"
#include "LapSupGraphics/Mesh.h"
#include "../Source/Graphics/Model3d.h"
#include "Graphics/Camera.h"

////Temporary Stuffs
//Model testmodel;
//Camera testCam;

Application::Application() {}
Application::~Application() {}

void Application::Init()
{

	/// <This should be like some sort of configuration file?>
	GLFW_Handler::width = 1600;
	GLFW_Handler::height = 900;
	/// <end>

	//Initializing OpenGL Context with OpenGL 4.5 and creating window
	if (!GLFW_Handler::init(GLFW_Handler::width, GLFW_Handler::height, "Put Team Name Here")) {
		std::cout << "Unable to create OpenGL context" << std::endl;
		std::exit(EXIT_FAILURE);
	}


	//// Temporary Stuffs
	//AssimpLoader assimp("Assets/Models/Skull_textured/Skull_textured.geom.desc", "Assets/Models/Skull_textured/Skull_textured.geom");
	//testmodel.init(&assimp);
	//testCam.Init();
}

void Application::Run()
{
	// Game Loop, MOVE TO ENGINE CORE
	//while (!glfwWindowShouldClose(GLFW_Handler::ptr_window)) 
	//{
	//	//MOVE TO INPUT/WINDOW SYSTEM
	//	glfwPollEvents(); // This at the start

	//	//IMGUI LOGIC - DO NOT PUT IMGUI INTO PCH

	//	//GAME LOGIC

	//	//MOVE TO GRAPHICS SYSTEM UPDATE
	//	//GRAPHICS SYSTEM END
	//	//IMGUI RENDER
	//}


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