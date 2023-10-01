/*!***************************************************************************************
\file			FramerateController.cpp
\project
\author			Zacharie Hong
\co-authors

\par			Course: GAM300
\par			Section:
\date			05/09/2023

\brief
	Contains definitions for FrameRateController class, its constructor and member functions.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "FramerateController.h"
#include <GLFW/glfw3.h>

void FrameRateController::Init(float _fixedDt)
{
	fixedDeltaTime	= _fixedDt;
	accumulatedTime = 0.0f;
}

void FrameRateController::Start()
{
	frameStart = glfwGetTime();
}

float FrameRateController::End()
{
	steps = 0;
	frameEnd = glfwGetTime();
	double deltaTime = frameEnd - frameStart;
	//if (deltaTime > 1 / 15.0f) deltaTime = 1 / 15.0f;
	accumulatedTime += deltaTime;
	while (accumulatedTime >= fixedDeltaTime)
	{
		accumulatedTime -= fixedDeltaTime;
		++steps;
	}
	return (float)deltaTime;
}
