#include "Precompiled.h"

#include "InputHandler.h"
// ---------------------------------------------------------------------------------------
// KEYBOARD 
// ---------------------------------------------------------------------------------------

bool InputHandler::isKeyButtonHolding(int key)
{
	//return glfwGetKey(GLFW_Handler::ptr_window, key);
	if ( (InputHandler::curr_Key_states[key] == 2) || (InputHandler::curr_Key_states[key] == 1))
	{
		return true;
	}

	return false;
}

bool InputHandler::isKeyButtonPressed(int key)
{
	if ( (InputHandler::curr_Key_states[key] == 1) && (InputHandler::last_Key_states[key] == 0))
	{
		InputHandler::last_Key_states[key] = 1;
		return true;
	}
	return false;
}


void InputHandler::setKeystate_Press(int key)
{
	if (InputHandler::curr_Key_states[key] == 0)
	{
		InputHandler::curr_Key_states[key] = 1;
		InputHandler::last_Key_states[key] = 0;
	}
	else if (InputHandler::last_Key_states[key] == 1)
	{
		InputHandler::curr_Key_states[key] = 2;
	}
}

void InputHandler::setKeystate_Hold(int key)
{
	InputHandler::curr_Key_states[key] = 2;
}

void InputHandler::setKeystate_Release(int key)
{
	InputHandler::curr_Key_states[key] = 0;
}

void InputHandler::copyingCurrStatetoLast()
{
	for (int i = 0; i < MAX_KEY_COUNT; ++i)
		InputHandler::last_Key_states[i] = InputHandler::curr_Key_states[i];
}


// ---------------------------------------------------------------------------------------
// MOUSE 
// ---------------------------------------------------------------------------------------

bool InputHandler::isMouseButtonPressed_L()
{
	return glfwGetMouseButton(GLFW_Handler::ptr_window, GLFW_MOUSE_BUTTON_LEFT);
}
bool InputHandler::isMouseButtonPressed_R()
{
	return glfwGetMouseButton(GLFW_Handler::ptr_window, GLFW_MOUSE_BUTTON_RIGHT);
}

glm::vec2 InputHandler::getMousePos()
{
	double x, y;
	glfwGetCursorPos(GLFW_Handler::ptr_window, &x, &y);
	y = GLFW_Handler::height - y;

	glm::vec2 X_Y(x, y);
	return X_Y;
}

double InputHandler::getMouseX()
{
	return getMousePos()[0];
}

double InputHandler::getMouseY()
{
	return getMousePos()[1];
}

void InputHandler::setMouseScroll(int state)
{
	if (state == -1)
	{
		mouseScrollState = -1;
	}
	if (state == 1)
	{
		mouseScrollState = 1;
	}
	
}
void InputHandler::reset()
{
	mouseScrollState = 0;
}

int InputHandler::getMouseScrollState()
{
	return mouseScrollState;
}


