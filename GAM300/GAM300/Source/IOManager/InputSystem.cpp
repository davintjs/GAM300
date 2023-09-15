#include "Precompiled.h"
#include "InputSystem.h"
#include "InputHandler.h"
#include "Handler_GLFW.h"


void InputSystem::Init()
{
	InputHandler::setFullscreen(false);
	//InputHandler:: = new int[55];
}

void InputSystem::Update(float dt)
{
	InputHandler::copyingCurrStatetoLast();

	InputHandler::mouseReset();


	glfwPollEvents();

	if (InputHandler::isKeyButtonPressed(GLFW_KEY_F11))
	{
		std::cout << "toggling between fullscreen and non fullscreen\n";
		InputHandler::setFullscreen(!InputHandler::isFullscreen());
	}

	if (InputHandler::isMouseButtonHolding_L())
	{
		std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
		
		if (InputHandler::get_Prev_MouseButtonState(0) == 2)// Checking if its still holding
		{
			InputHandler::setMouseButtonState(0, 2);
		}
		else if (InputHandler::get_Prev_MouseButtonState(0) == 1 )// Checking if it's a hold
		{
			InputHandler::setMouseButtonState(0, 2);

			if (InputHandler::doubleclick)
			{
				InputHandler::doubleclickAndHold = true;
				InputHandler::doubleclick = false;
			}
		}
		else if (InputHandler::get_Prev_MouseButtonState(0) == 0) // Checking if it's a fresh press
		{
			
			std::chrono::duration<double> time_span = 
				std::chrono::duration_cast<std::chrono::duration<double>>
				(now - InputHandler::prevMouse_LClick);
		
			if (time_span.count() <= 0.5)
			{
				InputHandler::doubleclick = true;
			}

			// Same part
			InputHandler::setMouseButtonState(0, 1);
			InputHandler::prevMouse_LClick = now;

		}

	}
	else
	{
		InputHandler::setMouseButtonState(0, 0);
		InputHandler::doubleclick = false;
		InputHandler::doubleclickAndHold = false;
	}
	
	/*if (InputHandler::isMouse_L_DoubleClick())
	{
		std::cout << "doubleclick ONLY\n";
	}*/

	/*if (InputHandler::isMouse_L_DoubleClickandHold())
	{
		std::cout << "doubleclick & Hold ONLY\n";
	}*/











	//--------------------------------------------------------------
	// KEYBOARD TESTING using button - 0
	//--------------------------------------------------------------
	
	/*if (InputHandler::isKeyButtonPressed(GLFW_KEY_0))
	{
		std::cout << "Pressed 0\n";
	}
	
	if (InputHandler::isKeyButtonHolding(GLFW_KEY_0))
	{
		std::cout << "Holding 0\n";
	}*/

	//if (InputHandler::isMouseButtonPressed_L())
	//{
	//	std::cout << "Pressing LEFT\n";
	//}
	//if (InputHandler::isMouseButtonPressed_R())
	//{
	//	std::cout << "Pressing RIGHT\n";
	//}
	//std::cout << InputHandler::getMouseY() << "\n";
	

}
void InputSystem::Exit()
{

}