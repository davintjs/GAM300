#include "Precompiled.h"

#include "BehaviorTree.h"

void BehaviorNode::setStatus(BehaviorStatus status)
{
	mStatus = status;
}

BehaviorResult BehaviorTree::RunTree()
{
	return mRootNode->Run();
}

std::string BehaviorTree::GetTreeName() const
{
	return mTreeName;
}
