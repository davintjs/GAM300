#pragma once

#include <iostream>
#include <string>

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
	IN_PROGRESS
};

class BehaviorNode
{
public:
	virtual ~BehaviorNode() {};
	virtual void Enter() = 0;
	virtual void Tick(float dt) = 0;

	void setStatus(BehaviorStatus status);
	void setResult(BehaviorResult result);

	const BehaviorStatus getStatus();
	const BehaviorResult getResult();

protected:
	void onLeafEnter();
	void onSuccess();
	void onFailure();

private:
	BehaviorStatus mStatus;
	BehaviorResult mResult;
};

class BehaviorTree
{
public:
	BehaviorTree(std::string name, BehaviorNode* rootNode) : mTreeName(name), mRootNode(rootNode) {}
	~BehaviorTree()
	{
		delete mRootNode;
	}

	void RunTree(float dt);
	std::string GetTreeName() const;

private:
	std::string mTreeName;
	BehaviorNode* mRootNode; // Root node of this behavior tree
};
