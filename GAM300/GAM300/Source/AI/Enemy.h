/*!***************************************************************************************
\file			Enemy.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations of the following:
	1. Enemy class
		a. Enemy component which causes AI agent to act accordingly to its behavior tree

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

class BehaviorTree;

class Enemy
{
public:
	Enemy(BehaviorTree* tree) : mTree(tree) {}

	// Enemy initialization
	void Init();

	// Enemy update loop
	void Update(float dt);

	// Enemy exit
	void Exit();

private:
	BehaviorTree* mTree;
};
