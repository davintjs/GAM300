#include "Precompiled.h"

#include "Enemy.h"
#include "BehaviorTree.h"

void Enemy::Init()
{

}

void Enemy::Update(float dt)
{
	mTree->RunTree(dt);
}

void Enemy::Exit()
{

}
