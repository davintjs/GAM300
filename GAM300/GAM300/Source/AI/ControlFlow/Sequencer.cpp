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

BehaviorResult Sequencer::Run()
{
	std::cout << "Control flow node running..." << std::endl;
	for (auto& child : mChildren)
	{
		BehaviorResult status = child->Run();
		if (status != BehaviorResult::SUCCESS) // This is a sequence control flow node
		{
			return status;
		}
	}

	return BehaviorResult::SUCCESS;
}