/*!***************************************************************************************
\file			InputHandler.cpp
\project
\author         Euan Lim

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations of the Input Handler
	1. Checks for keyboard inputs ( Press / Hold )
	2. Check for Mouse inputs ( Press / Hold / Double Click )
	3. Toggling on and off Fullscreen


All content ? 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"

#include "InputHandler.h"
#include "Editor/EditorCamera.h"


// ---------------------------------------------------------------------------------------
// KEYBOARD 
// ---------------------------------------------------------------------------------------

bool InputHandler::isKeyButtonHolding(int key)
{
	//return glfwGetKey(Application::GetWindow(), key);
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

	prev_mouse_Button_states[0] = mouse_Button_states[0];
	prev_mouse_Button_states[1] = mouse_Button_states[1];
	prev_mouse_Button_states[2] = mouse_Button_states[2];

	prevMousePos = getMousePos();
	//std::cout << "PREV STATE VAL : " << prev_mouse_Button_states[1] << "\n";
}


// ---------------------------------------------------------------------------------------
// MOUSE 
// ---------------------------------------------------------------------------------------

bool InputHandler::isMouseButtonPressed_L()
{
	if (InputHandler::isMouseButtonHolding_L())
	{
		if (InputHandler::get_Prev_MouseButtonState(0) == 0)
		{
			return true;
		}
	}

	return false;

}


bool InputHandler::isMouseButtonPressed_R()
{
	//std::cout << "entered\n";
	if (InputHandler::isMouseButtonHolding_R())
	{
		InputHandler::setMouseButtonState(1, 1);
		if (InputHandler::get_Prev_MouseButtonState(1) == 0)
		{
			return true;
		}
		
		//std::cout << "here\n";

	}
	return false;
}

bool InputHandler::isMouseButtonPressed_M()
{
	if (InputHandler::isMouseButtonHolding_M())
	{
		InputHandler::setMouseButtonState(2, 1);
		if (InputHandler::get_Prev_MouseButtonState(2) == 0)
		{
			return true;
		}
	
	}

	return false;
}

bool InputHandler::isMouseButtonHolding_L()
{
	return glfwGetMouseButton(Application::GetWindow(), GLFW_MOUSE_BUTTON_LEFT);
}


bool InputHandler::isMouseButtonHolding_R()
{
	return glfwGetMouseButton(Application::GetWindow(), GLFW_MOUSE_BUTTON_RIGHT);
}

bool InputHandler::isMouseButtonHolding_M()
{
	return glfwGetMouseButton(Application::GetWindow(), GLFW_MOUSE_BUTTON_MIDDLE);
}

glm::vec2 InputHandler::getMousePos()
{
	double x, y;
	glfwGetCursorPos(Application::GetWindow(), &x, &y);
	y = Application::GetHeight() - y;

	glm::vec2 X_Y(x, y);
	return X_Y;
}

double InputHandler::getMouseX()
{
	return getMousePos().x;
}

double InputHandler::getMouseY()
{
	return getMousePos().y;
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
void InputHandler::mouseReset()
{
	mouseScrollState = 0;

	mouse_Button_states[0] = 0;
	mouse_Button_states[1] = 0;
	mouse_Button_states[2] = 0;
}


int InputHandler::getMouseScrollState()
{
	return mouseScrollState;
}

bool InputHandler::isMouse_L_DoubleClick()
{
	if (InputHandler::doubleclick)
	{
		return true;
	}
	return false;
}

bool InputHandler::isMouse_L_DoubleClickandHold()
{
	if (InputHandler::doubleclickAndHold)
	{
		return true;
	}
	return false;

}


void InputHandler::setMouseButtonState(int index, int state)
{
	InputHandler::mouse_Button_states[index] = state;
}

int InputHandler::getMouseButtonState(int index)
{
	return InputHandler::mouse_Button_states[index];
}

int InputHandler::get_Prev_MouseButtonState(int index)
{
	return InputHandler::prev_mouse_Button_states[index];
}

glm::vec2 InputHandler::mouseDelta()
{
	

	
	return InputHandler::getMousePos() - prevMousePos;

}