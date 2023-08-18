#pragma once

#include <iostream>
#include <string>
#include <vector>

enum class BehaviorStatus
{
	SUCCESS,
	FAILURE,
	RUNNING
};

class BehaviorNode
{
public:
	virtual ~BehaviorNode() {};
	virtual BehaviorStatus Run() = 0;
};

class BehaviorTree
{
public:
	BehaviorTree(std::string name, BehaviorNode* rootNode) : mTreeName(name), mRootNode(rootNode) {}

	~BehaviorTree()
	{
		delete mRootNode;
	}

	BehaviorStatus RunTree();
	std::string GetTreeName() const;

private:
	std::string mTreeName;
	BehaviorNode* mRootNode; // Root node of this behavior tree
};
