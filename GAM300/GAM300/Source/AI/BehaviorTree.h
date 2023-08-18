#pragma once

#include <iostream>
#include <string>
#include <vector>

enum class BehaviorStatus
{
	READY,
	RUNNING,
	EXITING,
	SUSPENDED
};

enum class BehaviorResult
{
	SUCCESS,
	FAILURE,
	RUNNING
};

class BehaviorNode
{
public:
	virtual ~BehaviorNode() {};
	virtual void Enter() = 0;
	virtual BehaviorResult Run() = 0;

	void setStatus(BehaviorStatus status);

private:
	BehaviorStatus mStatus;
};

class BehaviorTree
{
public:
	BehaviorTree(std::string name, BehaviorNode* rootNode) : mTreeName(name), mRootNode(rootNode) {}

	~BehaviorTree()
	{
		delete mRootNode;
	}

	BehaviorResult RunTree();
	std::string GetTreeName() const;

private:
	std::string mTreeName;
	BehaviorNode* mRootNode; // Root node of this behavior tree
};
