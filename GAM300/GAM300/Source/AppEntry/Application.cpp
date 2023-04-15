#include "Precompiled.h"
#include "Application.h"

Application::Application() {}
Application::~Application() {}

void Application::Init()
{

}

void Application::Run()
{
	std::cout << "Hello wOrld" << std::endl;
}

void Application::Exit()
{

}

Application* Application::CreateApp()
{
	return new Application;
}