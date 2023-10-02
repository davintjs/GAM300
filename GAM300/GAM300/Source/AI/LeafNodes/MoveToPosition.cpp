/*!***************************************************************************************
\file			MoveToPosition.cpp
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definitions of the following:
	1. MoveToPosition class
		a. Move the AI agent to a position

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "MoveToPosition.h"

void MoveToPosition::Enter()
{
	onLeafEnter();
}

void MoveToPosition::Tick(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	std::cout << "Leaf node running..." << std::endl;
	onSuccess();
}