/*!***************************************************************************************
\file			Sequencer.cpp
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definitions of the following:
	1. Sequencer class
		a. Ticks the behavior tree in sequence from left to right

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "Sequencer.h"

void Sequencer::Enter()
{
	for (auto& child : getChildren())
	{
		child->setStatus(BehaviorStatus::READY);
	}
}

void Sequencer::Tick(float dt)
{
	std::cout << "Control flow node running..." << std::endl;
	for (auto& child : getChildren())
	{
		child->Tick(dt);
		if (child->getResult() != BehaviorResult::SUCCESS)
		{
			onFailure();
		}
	}

	onSuccess();
}