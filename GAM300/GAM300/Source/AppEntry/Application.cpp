#include "Precompiled.h"
#include "Application.h"

#include "IOManager/Handler_GLFW.h"

Application::Application() {}
Application::~Application() {}

void Application::Init()
{
	GLFW_Handler::width = 1600;
	GLFW_Handler::height = 900;
	//Initializing OpenGL Context with OpenGL 4.5 and creating window
	if (!GLFW_Handler::init(GLFW_Handler::width, GLFW_Handler::height, "Put Team Name Here")) {
		std::cout << "Unable to create OpenGL context" << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

void Application::Run()
{
	std::cout << "Konichiwa Tomodachis :)" << std::endl;

	// Game Loop
	while (!glfwWindowShouldClose(GLFW_Handler::ptr_window)) 
	{
		// This at the start
		glfwPollEvents();



		// This at the end
		glfwSwapBuffers(GLFW_Handler::ptr_window);
	}
}

void Application::Exit()
{
	GLFW_Handler::cleanup();

}

Application* Application::CreateApp()
{
	return new Application;
}