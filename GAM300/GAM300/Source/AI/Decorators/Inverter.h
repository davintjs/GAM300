#pragma once

#include "AI/BehaviorTree.h"

// Must have only 1 child. Can also be a standalone node without the need for composite node
class Inverter : public BehaviorNode
{
public:
	Inverter(BehaviorNode* child) : mChild(child) {}

	virtual ~Inverter()
	{
		delete mChild;
	}

private:
	virtual void Enter() override;
	virtual void Tick(float dt) override;

	BehaviorNode* mChild;
};