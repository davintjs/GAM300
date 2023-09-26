#include "Precompiled.h"

#include "MoveToPosition.h"

void MoveToPosition::Enter()
{
	onLeafEnter();
}

void MoveToPosition::Tick(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	std::cout << "Leaf node running..." << std::endl;
	onSuccess();
}