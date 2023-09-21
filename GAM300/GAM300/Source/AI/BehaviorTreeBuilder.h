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
    BehaviorTree* GetBehaviorTree(std::string treeName);

    void Init();
    void Update(float dt);
    void Exit();
private:
    const std::string BHTFiles = "GAM300/Source/AI/Trees";
    std::vector<BehaviorTree*> mBehaviorTrees;

    BehaviorNode* BuildNode(std::string nodeType, std::string nodeName);
    BehaviorNode* DeserializeControlFlow(std::string nodeName);
    BehaviorNode* DeserializeDecorator(std::string nodeName);
    BehaviorNode* DeserializeLeafNode(std::string nodeName);

    void BuildTrees();
    void BuildChildren(BehaviorNode* mNode, const rapidjson::Value& val);
};