#include "Precompiled.h"
#include "InputSystem.h"
#include "Handler_GLFW.h"


void InputSystem::Init()
{

}
void InputSystem::Update()
{

	// Bean: This should be in input system (Euan go create)
	glfwPollEvents();
	int checking = glfwGetKey(GLFW_Handler::ptr_window, GLFW_KEY_A);// Only returns GLFW_PRESS or GLFW_RELEASE

	if ((checking == GLFW_PRESS) )
	{
		std::cout << "tapping A key\n";
	}
	/*if ((checking == GLFW_REPEAT))
	{
		std::cout << "Holding A key\n";
	}*/
	//glfwSetMouseButtonCallback();
}
void InputSystem::Exit()
{

}