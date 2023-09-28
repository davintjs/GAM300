/*!***************************************************************************************
\file			MoveToPosition.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations of the following:
	1. MoveToPosition class
		a. Move the AI agent to a position

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#include "AI/BehaviorTree.h"

class MoveToPosition : public BehaviorNode
{
public:
	MoveToPosition() : mNodeName("MoveToPosition") {};
private:
	std::string mNodeName; // Name of this action
	virtual void Enter() override;
	virtual void Tick(float dt) override;
};