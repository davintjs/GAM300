#pragma once

#include "IOManager/Handler_GLFW.h"

class InputHandler
{
public:
	/*!*****************************************************************************
	\author
		Euan Lim
	\brief
		Check if KEYBOARD button is pressed
	param [in] key
		put in GLFW key code -> ex: GLFW_KEY_A
	\return
		GLFW_PRESS -> if pressed
		GLFW_RELEASE -> if released / not pressed
	*******************************************************************************/
	static bool isKeyButtonHolding(int key);

	static bool isKeyButtonPressed(int key);

	static void setKeystate_Press(int key);

	static void setKeystate_Hold(int key);

	static void setKeystate_Release(int key);

private:
	inline static int Key_states[348]; // 0 (Release) , 1 (Pressed), 2 Hold
};