/*!***************************************************************************************
\file			InputSystem.cpp
\project
\author         Euan Lim

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definitions of the Input System

All content ? 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "InputSystem.h"
#include "InputHandler.h"


extern glm::vec2 windowPos;
extern glm::vec2 windowDimension;


void InputSystem::Init()
{
	
}

void InputSystem::Update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	InputHandler::copyingCurrStatetoLast();

	InputHandler::mouseReset();



	if (lockCursor)
	{
		// Full Application 
		//glm::vec2 minPos(0.f);
		//glm::vec2 dimensions(Application::GetWidth(), Application::GetHeight());

		glm::vec2 minPos = windowPos;
		glm::vec2 dimensions = windowDimension;

		CenterCursor(minPos, dimensions); // Bug - it will map to where the game screen is even when no game screen is there.
	
	
	}
	
	glfwPollEvents();

	/*if (InputHandler::isKeyButtonPressed(GLFW_KEY_M))
	{
		toggleCursorLock();
	}*/
	//std::cout << lockCursor << "\n";

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
	
	InputHandler::setDimensions(windowDimension);

	//std::cout << "mouse difference " << InputHandler::mouseDelta().x <<
	//	" , " << InputHandler::mouseDelta().y << "\n";



	/*if (InputHandler::isMouseButtonPressed_M())
	{
		std::cout << "middle mouse clicked\n";
	}
	if (InputHandler::isMouseButtonHolding_M())
	{
		std::cout << "middle mouse Holding\n";
	}
	if (InputHandler::isMouseButtonPressed_R())
	{
		std::cout << "right mouse clicked\n";
	}
	if (InputHandler::isMouseButtonHolding_R())
	{
		std::cout << "right mouse Holding\n";
	}*/
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

void InputSystem::CenterCursor(glm::vec2 position, glm::vec2 dimension)
{
	position += dimension / 2.f;
	glfwSetCursorPos(Application::GetWindow(), position.x, position.y);

}

void InputSystem::toggleCursorLock()
{
	lockCursor = !lockCursor;
}

