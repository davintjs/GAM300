#include "Precompiled.h"

#include "Inverter.h"

void Inverter::Enter()
{
	mChild->setStatus(BehaviorStatus::READY);
	Run();
}

BehaviorResult Inverter::Run()
{
	std::cout << "Decorator node running..." << std::endl;
	BehaviorResult status = mChild->Run();

	if (status == BehaviorResult::SUCCESS)
	{
		return BehaviorResult::FAILURE;
	}
	else if (status == BehaviorResult::FAILURE)
	{
		return BehaviorResult::SUCCESS;
	}

	return status;
}