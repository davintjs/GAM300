/*!***************************************************************************************
\file			Inverter.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations of the following:
	1. Inverter class
		a. Inverts the result of the child node

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#include "AI/BehaviorTree.h"

// Must have only 1 child. Can also be a standalone node without the need for composite node
class Inverter : public BehaviorNode
{
private:
	virtual void Enter() override;
	virtual void Tick(float dt) override;
};