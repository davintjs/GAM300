/*!***************************************************************************************
\file			BehaviorTree.cpp
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definitions of the following:
	1. Enum class BehaviorStatus
		a. Behavior tree node's status to determine course of actions
	2. Enum class BehaviorResult
		a. Behavior tree node's returning result after execution
	3. BehaviorNode class
		a. Base leaf node class for inheritance by the control flow, decorators,
		   and leaf nodes
		b. Setter and getter functions
		c. Addition of child nodes
	4. BehaviorTree class
		a. Behavior tree of AI agents

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "BehaviorTree.h"

void BehaviorNode::setResult(BehaviorResult result)
{
	mResult = result;
}

void BehaviorNode::setStatus(BehaviorStatus status)
{
	mStatus = status;
}

const BehaviorStatus BehaviorNode::getStatus()
{
	return mStatus;
}

const BehaviorResult BehaviorNode::getResult()
{
	return mResult;
}

void BehaviorNode::addChild(BehaviorNode* mChild)
{
	mChildren.push_back(mChild);
}

std::vector<BehaviorNode*> BehaviorNode::getChildren()
{
	return mChildren;
}

void BehaviorNode::onLeafEnter()
{
	setStatus(BehaviorStatus::RUNNING);
	setResult(BehaviorResult::IN_PROGRESS);
}

void BehaviorNode::onSuccess()
{
	setStatus(BehaviorStatus::EXITING);
	setResult(BehaviorResult::SUCCESS);
}

void BehaviorNode::onFailure()
{
	setStatus(BehaviorStatus::EXITING);
	setResult(BehaviorResult::FAILURE);
}

void BehaviorTree::RunTree(float dt)
{
	mRootNode->Tick(dt);
}

std::string BehaviorTree::GetTreeName() const
{
	return mTreeName;
}
