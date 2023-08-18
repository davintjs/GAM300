#pragma once

#include "BehaviorTree.h"

class Enemy
{
public:
	Enemy(BehaviorTree* tree) : mTree(tree) {}

	void Init();
	void Update(float dt);
	void Exit();

private:
	BehaviorTree* mTree;
};
