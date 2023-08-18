#include "Precompiled.h"

#include "Sequencer.h"

void Sequencer::addChild(BehaviorNode* child)
{
	mChildren.push_back(child);
}

BehaviorStatus Sequencer::Run()
{
	std::cout << "Control flow node running..." << std::endl;
	for (auto& child : mChildren)
	{
		BehaviorStatus status = child->Run();
		if (status != BehaviorStatus::SUCCESS) // This is a sequence control flow node
		{
			return status;
		}
	}

	return BehaviorStatus::SUCCESS;
}