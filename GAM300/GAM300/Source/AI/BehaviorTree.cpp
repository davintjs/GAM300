#include "Precompiled.h"

#include "BehaviorTree.h"

BehaviorStatus BehaviorTree::RunTree()
{
	return mRootNode->Run();
}

std::string BehaviorTree::GetTreeName() const
{
	return mTreeName;
}
