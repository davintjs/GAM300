#include "Precompiled.h"
#include "InputSystem.h"
#include "InputHandler.h"
#include "Handler_GLFW.h"


void InputSystem::Init()
{
	//InputHandler:: = new int[55];
}
void InputSystem::Update()
{

	// Bean: This should be in input system (Euan go create)
	glfwPollEvents();
	//int checking = glfwGetKey(GLFW_Handler::ptr_window, GLFW_KEY_A);// Only returns GLFW_PRESS or GLFW_RELEASE

	//if ((checking == GLFW_PRESS))
	//{
	//	std::cout << "tapping A key\n";
	//}

	//InputHandler::isKeyButtonPressed(GLFW_KEY_0)

	if (InputHandler::isKeyButtonPressed(GLFW_KEY_0))
	{
		std::cout << "Pressed 0\n";
	}
	
	if (InputHandler::isKeyButtonHolding(GLFW_KEY_0))
	{
		std::cout << "Holding 0\n";
	}
	if (InputHandler::isKeyButtonPressed(GLFW_KEY_0) == GLFW_RELEASE)
	{
		std::cout << "NOT PRESSING 0\n";
	}

}
void InputSystem::Exit()
{

}