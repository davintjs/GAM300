/*!***************************************************************************************
\file			Sequencer.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations of the following:
	1. Sequencer class
		a. Ticks the behavior tree in sequence from left to right

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#include "AI/BehaviorTree.h"

// Must have > 2 children regardless of a decorator or a leaf node
class Sequencer : public BehaviorNode
{
private:
	virtual void Enter() override;
	virtual void Tick(float dt) override;
};