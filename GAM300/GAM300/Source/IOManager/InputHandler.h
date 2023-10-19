/*!***************************************************************************************
\file			InputHandler.h
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

#pragma once

#include "AppEntry/Application.h"
#include <glm/vec2.hpp>

#define MAX_KEY_COUNT 348

enum ButtonState
{
	Release = 0,
	Press = 1,
	Hold = 2
};


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

	// Getter for Keyboard Button Holding
	static bool isKeyButtonHolding(int key);// I guess u can use this for things like moving around
	
	// Getter for Keyboard Button Pressed
	static bool isKeyButtonPressed(int key);// UNIQUE 1 Time Press -> Pressing Tab or something like that
											
	// Getter for Mouse left button Pressed
	static bool isMouseButtonPressed_L();
	
	// Getter for Mouse right button Pressed
	static bool isMouseButtonPressed_R();
	
	// Getter for Mouse middle button Pressed
	static bool isMouseButtonPressed_M();

	// Getter for Mouse left button holding
	static bool isMouseButtonHolding_L();

	// Getter for Mouse right button Holding
	static bool isMouseButtonHolding_R();

	// Getter for Mouse middle button Holding
	static bool isMouseButtonHolding_M();

	// Getter for Mouse coordinate (screen space)
	static glm::vec2 getMousePos(); // < X , Y >

	// Getter for mouse x coordinate
	static double getMouseX();

	// Getter for mouse y coordinate
	static double getMouseY();

	// Setter for mouse scroll wheel
	static void setMouseScroll(int);

	// Getter for mouse scroll wheel
	static int getMouseScrollState();// 1 -> scroll up / -1 -> scroll down, 0 -> nothing

	// Setter for Keyboard button state - Press
	static void setKeystate_Press(int key);

	// Setter for Keyboard button state - Hold
	static void setKeystate_Hold(int key);

	// Setter for Keyboard button state - Release
	static void setKeystate_Release(int key);

	
	static void copyingCurrStatetoLast();

	static void mouseReset();

	// Getter for mouse left button double click
	static bool isMouse_L_DoubleClick();

	// Getter for mouse left button double click & Hold
	static bool isMouse_L_DoubleClickandHold();

	// Setter for mouse button states
	static void setMouseButtonState(int index, int state);

	// Getter for current mouse button states
	static int getMouseButtonState(int index); // change to button state
	
	// Getter for previous mouse button states
	static int get_Prev_MouseButtonState(int index); // change to button state
	// [0] is left click, [1] is right click
	

	inline static std::chrono::high_resolution_clock::time_point prevMouse_LClick;

	inline static bool doubleclick = false;
	inline static bool doubleclickAndHold = false;

private:
	inline static int curr_Key_states[MAX_KEY_COUNT]; // 0 (Release) , 1 (Pressed), 2 Hold
	inline static int last_Key_states[MAX_KEY_COUNT]; // Used for the difference between press and hold
	
	
	inline static int mouseScrollState = 0;
	inline static int mouse_Button_states[3]{ 0 };// 0 (Release) , 1 (Pressed), 2 Hold
	inline static int prev_mouse_Button_states[3]{ 0 };// 0 (Release) , 1 (Pressed), 2 Hold
};