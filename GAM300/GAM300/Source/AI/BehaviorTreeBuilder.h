/*!***************************************************************************************
\file			BehaviorTreeBuilder.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations of the following:
	1. BehaviorTreeBuilder singleton class
        a. Builds the behavior trees in the Trees folder to store into memory
        b. Helper functions for deserialization of RapidJSON format

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#include "BehaviorTree.h"
#include "Core/SystemInterface.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#define BEHAVIORTREEBUILDER BehaviorTreeBuilder::Instance()

// This class builds the behavior trees from the trees folder upon start up
// and store all of it into the vector of behavior trees upon 
SINGLETON(BehaviorTreeBuilder)
{
public:
    // Getter function to return all behavior trees built
    BehaviorTree* GetBehaviorTree(std::string treeName);

    // BehaviorTreeBuilder initialization
    void Init();

    // BehaviorTreeBuilder update loop
    void Update(float dt);

    // BehaviorTreeBuilder exit
    void Exit();
private:
    const std::string BHTFiles = "GAM300/Source/AI/Trees";
    std::unordered_map<std::string, BehaviorTree*> mBehaviorTrees;

    // Builds a node of the tree
    BehaviorNode* BuildNode(std::string nodeType, std::string nodeName);

    // Deserializes a control flow node according to the name
    BehaviorNode* DeserializeControlFlow(std::string nodeName);

    // Deserializes a decorator node according to the name
    BehaviorNode* DeserializeDecorator(std::string nodeName);

    // Deserializes a leaf node according to the name
    BehaviorNode* DeserializeLeafNode(std::string nodeName);

    // Build all trees in the Trees folder
    void BuildTrees();

    // Build children nodes
    void BuildChildren(BehaviorNode* mNode, const rapidjson::Value& val);
};