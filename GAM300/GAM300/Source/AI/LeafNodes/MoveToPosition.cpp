#include "Precompiled.h"

#include "MoveToPosition.h"

BehaviorStatus MoveToPosition::Run()
{
	std::cout << "Leaf node running..." << std::endl;
	return BehaviorStatus::SUCCESS;
}