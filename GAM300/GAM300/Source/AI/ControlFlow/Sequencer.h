#pragma once

#include "AI/BehaviorTree.h"

// Must have > 2 children regardless of a decorator or a leaf node
class Sequencer : public BehaviorNode
{
private:
	virtual void Enter() override;
	virtual void Tick(float dt) override;
};