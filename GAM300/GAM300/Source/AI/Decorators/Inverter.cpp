#include "Precompiled.h"

#include "Inverter.h"

void Inverter::Enter()
{
	mChild->setStatus(BehaviorStatus::READY);
}

void Inverter::Tick(float dt)
{
	std::cout << "Decorator node running..." << std::endl;
	mChild->Tick(dt);
	if (mChild->getResult() == BehaviorResult::SUCCESS)
	{
		onFailure();
	}
	else if (mChild->getResult() == BehaviorResult::FAILURE)
	{
		onSuccess();
	}
}