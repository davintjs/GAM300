/*!***************************************************************************************
\file			Enemy.cpp
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definitions of the following:
	1. Enemy class
		a. Enemy component which causes AI agent to act accordingly to its behavior tree

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"

#include "Enemy.h"
#include "BehaviorTree.h"

void Enemy::Init()
{

}

void Enemy::Update(float dt)
{
	mTree->RunTree(dt);
}

void Enemy::Exit()
{

}
