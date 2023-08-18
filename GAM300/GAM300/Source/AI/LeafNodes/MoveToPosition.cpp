#include "Precompiled.h"

#include "MoveToPosition.h"

void MoveToPosition::Enter()
{

}

BehaviorResult MoveToPosition::Run()
{
	std::cout << "Leaf node running..." << std::endl;
	return BehaviorResult::SUCCESS;
}