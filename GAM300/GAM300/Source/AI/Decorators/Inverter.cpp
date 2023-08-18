#include "Precompiled.h"

#include "Inverter.h"

BehaviorStatus Inverter::Run()
{
	std::cout << "Decorator node running..." << std::endl;
	BehaviorStatus status = mChild->Run();

	if (status == BehaviorStatus::SUCCESS)
	{
		return BehaviorStatus::FAILURE;
	}
	else if (status == BehaviorStatus::FAILURE)
	{
		return BehaviorStatus::SUCCESS;
	}

	return status;
}