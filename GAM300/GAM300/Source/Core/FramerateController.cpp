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


void FrameRateController::Init(double _maxFPS)
{
	frameCount = 0;
	maxFrameRate = _maxFPS;
	frameRate = _maxFPS;
	fixedDeltaTime = 1 / _maxFPS;
	deltaTime = 0.0;
	accumulatedTime = 0.0;
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

void FrameRateController::End()
{
	steps = 0;
	frameEnd = glfwGetTime();
	deltaTime = frameEnd - frameStart;
	while (deltaTime < fixedDeltaTime)
	{
		frameEnd = glfwGetTime();
		deltaTime = frameEnd - frameStart;
	}
	accumulatedTime += deltaTime;
	while (accumulatedTime > fixedDeltaTime)
	{
		accumulatedTime -= fixedDeltaTime;
		++steps;
	}
	frameRate = 1 / deltaTime;
}


