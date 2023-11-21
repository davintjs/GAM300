/*!***************************************************************************************
\file			BaseAnimator.cpp
\project
\author         Theophelia Tan

\par			Course: GAM300
\date           23/10/2023

\brief
    This file contains the definitions of the following:
    1.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "BaseAnimator.h"
#include "AnimationManager.h"

BaseAnimator::BaseAnimator()
{
    m_CurrentTime = 0.0f;
    startTime = 0.0f;
    endTime = 0.0f;
    m_AnimationIdx = -1;
    m_FinalBoneMatIdx = -1; 
    currentState = nextState = defaultState = nullptr;
    playing = false;

    m_FinalBoneMatrices.reserve(100);

    for (int i = 0; i < 100; i++)
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

void BaseAnimator::UpdateAnimation(float dt, glm::mat4& pTransform)
{
    Animation& m_CurrentAnimation = AnimationManager.GetAnimCopy(m_AnimationIdx);

    m_CurrentTime += (m_CurrentAnimation.GetTicksPerSecond() * dt) - startTime;

    // Change state if the current time passes the end time
    if (m_CurrentTime >= endTime - startTime)
        ChangeState();

    // crash prevention
    endTime = (endTime > m_CurrentAnimation.GetDuration() || endTime == 0.f) ? m_CurrentAnimation.GetDuration() : endTime;
    startTime = (startTime > endTime) ? endTime - 1.f : startTime;

    //m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation.GetDuration());
    m_CurrentTime = fmod(m_CurrentTime, endTime - startTime);
    m_CurrentTime += startTime; // wrap within the time range then offset by the start time 

    CalculateBoneTransform(&m_CurrentAnimation.GetRootNode(), glm::mat4(1.f));
}

void BaseAnimator::PlayAnimation(Animation* pAnimation)
{
    Animation& m_CurrentAnimation = AnimationManager.GetAnimCopy(m_AnimationIdx);
    m_CurrentAnimation = *pAnimation;
    m_CurrentTime = 0.0f;
}

void BaseAnimator::ChangeState()
{
    if (currentState != nextState)
        m_CurrentTime = 0.f;

    currentState = nextState;

    if (!currentState) // If no next state, use default state
        currentState = defaultState; 

    // Check that the current state exists
    if (currentState)
    {
        startTime = currentState->minMax.x;
        endTime = currentState->minMax.y;
        stateName = currentState->label;
        stateNextName = "None";
        playing = true;
    }
    else
    {
        startTime = endTime = 0.f;
        stateName = "None";
        stateNextName = "None";
        playing = false;
    }

    nextState = nullptr;
}

void BaseAnimator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    //Bone* Bone = m_CurrentAnimation.FindBone(nodeName);
    Animation& m_CurrentAnimation = AnimationManager.GetAnimCopy(m_AnimationIdx);

    Bone* Bone = m_CurrentAnimation.FindBone(nodeName);

    if (Bone)
    {
        Bone->Update(m_CurrentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = m_CurrentAnimation.GetBoneInfoMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        m_FinalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(&node->children[i], globalTransformation);
}

void BaseAnimator::SetDefaultState(const std::string& _defaultState)
{
    Animation& animation = AnimationManager.GetAnimCopy(m_AnimationIdx);
    defaultState = animation.GetAnimationState(_defaultState);
}

void BaseAnimator::SetNextState(const std::string& _nextState)
{
    // Is not the same as next state
    if (nextState == nullptr || nextState->label.compare(_nextState))
    {
        Animation& animation = AnimationManager.GetAnimCopy(m_AnimationIdx);
        nextState = animation.GetAnimationState(_nextState);
        stateNextName = _nextState;
    }
}

void BaseAnimator::SetState(const std::string& _state)
{
    SetNextState(_state);
    ChangeState();
}

bool BaseAnimator::AnimationAttached() {
    // Check if m_CurrentAnimation is not nullptr (i.e., it's attached)
    return m_AnimationIdx != -1 && animID != 0;
}
