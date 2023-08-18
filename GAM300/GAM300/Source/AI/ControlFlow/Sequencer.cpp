#include "Precompiled.h"

#include "Sequencer.h"

void Sequencer::addChild(BehaviorNode* child)
{
	mChildren.push_back(child);
}

void Sequencer::Enter()
{
	for (auto& child : mChildren)
	{
		child->setStatus(BehaviorStatus::READY);
	}
}

void Sequencer::Tick(float dt)
{
	std::cout << "Control flow node running..." << std::endl;
	for (auto& child : mChildren)
	{
		child->Tick(dt);
		if (child->getResult() != BehaviorResult::SUCCESS)
		{
			onFailure();
		}
	}

	onSuccess();
}