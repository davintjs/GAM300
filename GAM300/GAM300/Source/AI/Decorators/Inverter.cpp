#include "Precompiled.h"

#include "Inverter.h"

void Inverter::Enter()
{
	getChildren()[0]->setStatus(BehaviorStatus::READY);
}

void Inverter::Tick(float dt)
{
	std::cout << "Decorator node running..." << std::endl;
	getChildren()[0]->Tick(dt);
	if (getChildren()[0]->getResult() == BehaviorResult::SUCCESS)
	{
		onFailure();
	}
	else if (getChildren()[0]->getResult() == BehaviorResult::FAILURE)
	{
		onSuccess();
	}
}