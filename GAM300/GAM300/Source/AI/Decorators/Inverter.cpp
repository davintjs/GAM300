/*!***************************************************************************************
\file			Inverter.cpp
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definitions of the following:
	1. Inverter class
		a. Inverts the result of the child node

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "Inverter.h"

void Inverter::Enter()
{
	getChildren()[0]->setStatus(BehaviorStatus::READY);
}

void Inverter::Tick(float dt)
{
	std::cout << "Decorator node running..." << std::endl;
	getChildren()[0]->Tick(dt);
	if (getChildren()[0]->getResult() == BehaviorResult::SUCCESS)
	{
		onFailure();
	}
	else if (getChildren()[0]->getResult() == BehaviorResult::FAILURE)
	{
		onSuccess();
	}
}