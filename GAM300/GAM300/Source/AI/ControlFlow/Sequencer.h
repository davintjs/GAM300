#pragma once

#include "AI/BehaviorTree.h"

// Must have > 2 children regardless of a decorator or a leaf node
class Sequencer : public BehaviorNode
{
public:
	virtual ~Sequencer()
	{
		for (BehaviorNode* child : mChildren)
		{
			delete child;
		}
	}

	void addChild(BehaviorNode* child);

private:
	virtual void Enter() override;
	virtual void Tick(float dt) override;

	std::vector<BehaviorNode*> mChildren;
};