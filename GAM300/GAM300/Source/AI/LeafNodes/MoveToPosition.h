#pragma once

#include "AI/BehaviorTree.h"

class MoveToPosition : public BehaviorNode
{
public:
	MoveToPosition(std::string name) : mNodeName(name) {};

private:
	std::string mNodeName; // Name of this action
	virtual BehaviorStatus Run() override;
};