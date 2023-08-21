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