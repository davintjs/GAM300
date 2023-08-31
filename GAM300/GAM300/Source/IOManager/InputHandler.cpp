#include "Precompiled.h"

#include "InputHandler.h"
#include "../Graphics/Editor_Camera.h"


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

	prev_mouse_Button_states[0] = mouse_Button_states[0];
	prev_mouse_Button_states[1] = mouse_Button_states[1];
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
void InputHandler::mouseReset()
{
	mouseScrollState = 0;

	mouse_Button_states[0] = 0;
	mouse_Button_states[1] = 0;
}


int InputHandler::getMouseScrollState()
{
	return mouseScrollState;
}

void InputHandler::setFullscreen(bool state)
{
	fullscreen = state;
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	if (fullscreen)// going into fullscreen mode
	{

		std::cout << "going into fullscreen mode\n";
		glfwSetWindowMonitor(GLFW_Handler::ptr_window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
		glViewport(0, 0, mode->width, mode->height);
		EditorCam.setViewportSize(mode->width, mode->height);

	}
	else
	{

		std::cout << "going into windowed mode\n";
		glfwSetWindowMonitor(GLFW_Handler::ptr_window, nullptr, 0, 30, GLFW_Handler::width, GLFW_Handler::height,
			mode->refreshRate);
		glfwSetWindowAttrib(GLFW_Handler::ptr_window, GLFW_DECORATED, GLFW_TRUE);
		glViewport(0, 0, GLFW_Handler::width, GLFW_Handler::height);
		EditorCam.setViewportSize(GLFW_Handler::width, GLFW_Handler::height);

	}


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


bool InputHandler::isFullscreen()
{
	return fullscreen;
}





//void InputHandler::resizeViewport(int width, int height)
//{
//	glViewport(0, 0, width, height);
//
//}
