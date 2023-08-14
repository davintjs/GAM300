#include "Precompiled.h"

#include "InputHandler.h"

bool InputHandler::isKeyButtonHolding(int key)
{
	//return glfwGetKey(GLFW_Handler::ptr_window, key);
	if (InputHandler::Key_states[key] == 2)
	{
		return true;
	}

	return false;
}



bool InputHandler::isKeyButtonPressed(int key)
{
	/*if (glfwGetKey(GLFW_Handler::ptr_window, key) == GLFW_RELEASE)
	{
		return glfwGetKey(GLFW_Handler::ptr_window, key);
	}*/

	if (InputHandler::Key_states[key] == 1)
	{
		InputHandler::Key_states[key] = 0;
		return true;
	}

	return false;
}


void InputHandler::setKeystate_Press(int key)
{
	if (InputHandler::Key_states[key] == 1)
	{
		InputHandler::Key_states[key] = 2;
	}
	else if (InputHandler::Key_states[key] == 2)
	{

	}
	else
	{
		InputHandler::Key_states[key] = 1;
	}
}

void InputHandler::setKeystate_Hold(int key)
{
	InputHandler::Key_states[key] = 2;
}

void InputHandler::setKeystate_Release(int key)
{
	InputHandler::Key_states[key] = 0;
}