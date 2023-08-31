#pragma once

#include "IOManager/Handler_GLFW.h"


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
	static bool isKeyButtonHolding(int key);// I guess u can use this for things like moving around

	static bool isKeyButtonPressed(int key);// UNIQUE 1 Time Press -> Pressing Tab or something like that
											// I am sure u know what i mean big boi
	static bool isMouseButtonPressed_L();
	static bool isMouseButtonPressed_R();

	static glm::vec2 getMousePos(); // < X , Y >

	static void setFullscreen(bool);

	static bool isFullscreen();

	//static void resizeViewport(int width, int height);
	static double getMouseX(); 
	static double getMouseY();

	static void setMouseScroll(int);

	static int getMouseScrollState();// 1 -> scroll up / -1 -> scroll down, 0 -> nothing

	static void setKeystate_Press(int key);

	static void setKeystate_Hold(int key);

	static void setKeystate_Release(int key);

	static void copyingCurrStatetoLast();

	static void mouseReset();

	static bool isMouse_L_DoubleClick();

	static bool isMouse_L_DoubleClickandHold();

	static void setMouseButtonState(int index, int state);

	static int getMouseButtonState(int index); // change to button state
	static int get_Prev_MouseButtonState(int index); // change to button state
	// [0] is left click, [1] is right click
	

	inline static std::chrono::high_resolution_clock::time_point prevMouse_LClick;

	inline static bool doubleclick = false;
	inline static bool doubleclickAndHold = false;

private:
	inline static int curr_Key_states[MAX_KEY_COUNT]; // 0 (Release) , 1 (Pressed), 2 Hold
	inline static int last_Key_states[MAX_KEY_COUNT]; // Used for the difference between press and hold
	
	
	inline static int mouseScrollState = 0;
	inline static int mouse_Button_states[2];// 0 (Release) , 1 (Pressed), 2 Hold
	inline static int prev_mouse_Button_states[2];// 0 (Release) , 1 (Pressed), 2 Hold




	inline static bool fullscreen;

	
};