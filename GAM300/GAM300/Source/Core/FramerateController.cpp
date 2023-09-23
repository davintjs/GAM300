/*!***************************************************************************************
\file			FramerateController.cpp
\project
\author			Matthew Lau
\co-authors

\par			Course: GAM200
\par			Section:
\date			05/09/2022

\brief
	Contains the definitions for FrameRateController class, its constructor and
	member functions.

All content © 2022 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "FramerateController.h"
#include <GLFW/glfw3.h>
#include "Core/Debug.h"


void FrameRateController::Init(float _fixedDt)
{
	fixedDeltaTime	= _fixedDt;
	accumulatedTime = 0.0f;
}

/*******************************************************************************
/*!
*
\brief
	sets the time that the frame started at.
	Note: this function should be called at the beginning of each game loop

\return
	void
*/
/*******************************************************************************/
void FrameRateController::Start()
{
	frameStart = glfwGetTime();
}

float FrameRateController::End()
{
	steps = 0;
	frameEnd = glfwGetTime();
	float deltaTime = frameEnd - frameStart;
	//if (deltaTime > 1 / 15.0f) deltaTime = 1 / 15.0f;
	accumulatedTime += deltaTime;
	while (accumulatedTime >= fixedDeltaTime)
	{
		accumulatedTime -= fixedDeltaTime;
		++steps;
	}
	return deltaTime;
}