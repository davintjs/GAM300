#include "Precompiled.h"
#include "Sequencer.h"

void Sequencer::Enter()
{
	for (auto& child : getChildren())
	{
		child->setStatus(BehaviorStatus::READY);
	}
}

void Sequencer::Tick(float dt)
{
	std::cout << "Control flow node running..." << std::endl;
	for (auto& child : getChildren())
	{
		child->Tick(dt);
		if (child->getResult() != BehaviorResult::SUCCESS)
		{
			onFailure();
		}
	}

	onSuccess();
}