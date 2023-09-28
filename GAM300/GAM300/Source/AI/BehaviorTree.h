/*!***************************************************************************************
\file			BehaviorTree.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations of the following:
	1. Enum class BehaviorStatus
		a. Behavior tree node's status to determine course of actions
	2. Enum class BehaviorResult
		a. Behavior tree node's returning result after execution
	3. BehaviorNode class
		a. Base leaf node class for inheritance by the control flow, decorators, 
		   and leaf nodes
		b. Setter and getter functions
		c. Addition of child nodes
	4. BehaviorTree class
		a. Behavior tree of AI agents

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

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
	BehaviorNode() = default;
	virtual ~BehaviorNode() 
	{
		for (auto& child : mChildren)
		{
			delete child;
		}
	};
	
	// BehaviorNode initialization upon entering state
	virtual void Enter() = 0;

	// Tick this node
	virtual void Tick(float dt) = 0;

	// Set the status of this node
	void setStatus(BehaviorStatus status);

	// Set the result of this node after execution
	void setResult(BehaviorResult result);

	// Get the status of this node
	const BehaviorStatus getStatus();

	// Get the result of this node's execution
	const BehaviorResult getResult();

	// Add child to this BehaviorNode
	void addChild(BehaviorNode* child);

	// Get the children of this BehaviorNode
	std::vector<BehaviorNode*> getChildren();

protected:

	// Helper function for initialization of the node on enter from a parent node
	void onLeafEnter();

	// Helper function for successful execution of the node
	void onSuccess();

	// Helper function for failed execution of the node
	void onFailure();

private:
	BehaviorStatus mStatus;
	BehaviorResult mResult;

	std::vector<BehaviorNode*> mChildren;
};

class BehaviorTree
{
public:
	BehaviorTree(std::string name, BehaviorNode* rootNode) : mTreeName(name), mRootNode(rootNode) {}
	~BehaviorTree()
	{
		delete mRootNode;
	}

	// Run the behavior tree on the AI agent
	void RunTree(float dt);
	
	// Getter function of the behavior tree name
	std::string GetTreeName() const;

private:
	std::string mTreeName;
	BehaviorNode* mRootNode; // Root node of this behavior tree
};
