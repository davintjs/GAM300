#pragma once

#include "BehaviorTree.h"
#include "Core/SystemInterface.h"

// This class builds the behavior trees from the trees folder upon start up
// and store all of it into the vector of behavior trees upon 
ENGINE_SYSTEM(BehaviorTreeBuilder)
{
public:

    void Init();
    void Update();
    void Exit();

    void BuildTrees();

private:
    std::vector<BehaviorTree> mBehaviorTrees;
};