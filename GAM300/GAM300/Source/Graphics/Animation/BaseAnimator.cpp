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
    currBlendState = notblending;
    blendedBones = 0;
    blendDuration = 5.f;

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
    {
        if (!nextState)
        {
            nextState = defaultState;
        }

        if (!currentState)
        {
            currentState = defaultState;

            m_CurrentTime = startTime = currentState->minMax.x;
            endTime = currentState->minMax.y;
        }

        if (currentState != nextState)
        {
            currBlendState = blending;
            endTime += blendDuration;
        }
        else
            currBlendState = notblending;
    }

    // crash prevention
    //endTime = (endTime > m_CurrentAnimation.GetDuration() || endTime == 0.f) ? m_CurrentAnimation.GetDuration() : endTime;
    //startTime = (startTime > endTime) ? endTime - 1.f : startTime;

    m_CurrentTime = fmod(m_CurrentTime, endTime - startTime);
    m_CurrentTime += startTime; // wrap within the time range then offset by the start time 

    if (currBlendState == blending)/*if (nextState)*/
    {
        blendedBones = 0;
        CalculateBlendedBoneTransform(&m_CurrentAnimation.GetRootNode(), glm::mat4(1.f));
        
        if (blendedBones == m_CurrentAnimation.GetBoneCount())
        {
            currBlendState = blended;
            currentState = nextState;

            m_CurrentTime = startTime = currentState->minMax.x;
            endTime = currentState->minMax.y;
            stateName = currentState->label;
            playing = true;
        }
    }
    else
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

void BaseAnimator::CalculateBlendedBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
    if (nextState)
    {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Animation& m_CurrentAnimation = AnimationManager.GetAnimCopy(m_AnimationIdx);
        Animation& m_NextAnimation = AnimationManager.GetAnimCopy(m_AnimationIdx);

        Bone* NextBone = m_NextAnimation.FindBone(nodeName);
        Bone* Bone = m_CurrentAnimation.FindBone(nodeName);

        if (Bone && NextBone)
        {
            // get anim1 xform
             int p0Index = Bone->GetPositionIndex(currentState->minMax.y);
            // get anim2 xform
            int p1Index = NextBone->GetPositionIndex(nextState->minMax.x);

            // blend factor
            float blendFactor = Bone->GetBlendFactor(Bone->GetTimeStamp(p0Index),
                blendDuration, m_CurrentTime);

            if (blendFactor >= 1.f)
            {
                ++blendedBones;
            }

            // blend them, 
            glm::vec3 finalPosition = glm::mix(Bone->m_Positions[p0Index].position,
                NextBone->m_Positions[p1Index].position, blendFactor);
            glm::quat finalRotation = glm::slerp(Bone->m_Rotations[p0Index].orientation,
                NextBone->m_Rotations[p1Index].orientation, blendFactor);
            glm::vec3 finalScale = glm::mix(Bone->m_Scales[p0Index].scale,
                NextBone->m_Scales[p1Index].scale, blendFactor);

            finalRotation = glm::normalize(finalRotation);

            glm::mat4 translation = glm::translate(glm::mat4(1.0f), finalPosition);
            glm::mat4 rotation = glm::toMat4(finalRotation);
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), finalScale);

            //set nodexform to blended one
            //nodeTransform = NextBone->GetLocalTransform(); // temp
            nodeTransform = translation * rotation * scale; // temp
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
            CalculateBlendedBoneTransform(&node->children[i], globalTransformation);
    }
    else
    {
        currBlendState = blended;
    }
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
    //ChangeState();
}

bool BaseAnimator::AnimationAttached() {
    // Check if m_CurrentAnimation is not nullptr (i.e., it's attached)
    return m_AnimationIdx != -1 && animID != 0;
}


//void BaseAnimator::CalculateBlendFactor(float transitionDuration)
//{
//    // Calculate the blend factor as the ratio of elapsed time to transition duration
//    blendFactor = glm::clamp( (m_CurrentTime - startTime) / transitionDuration, 0.0f, 1.0f);
//}

//glm::mat4 lerp(const glm::mat4& a, const glm::mat4& b, float t) {
//    t = glm::clamp(t, 0.0f, 1.0f); // Ensure t is in the [0, 1] range
//    return a + (b - a) * t;
//}
//void BaseAnimator::InterpolateAnimations(Animation& firstAnimation, Animation& secondAnimation)
//{
//    // Ensure that both animations have the same number of bones
//    if (firstAnimation.GetBoneCount() != secondAnimation.GetBoneCount())
//    {
//        // Handle error: Animations must have the same number of bones for blending
//        return;
//    }
//
//    // Interpolate between the bone transformations of the two animations based on the blend factor
//    for (int i = 0; i < firstAnimation.GetBoneCount(); ++i)
//    {
//        // Get bone transformations from the two animations
//        glm::mat4 firstTransform = firstAnimation.GetBones()[i].GetLocalTransform();
//        glm::mat4 secondTransform = secondAnimation.GetBones()[i].GetLocalTransform();
//
//        // Interpolate between the two transformations
//        glm::mat4 interpolatedTransform = lerp(firstTransform, secondTransform, blendFactor);
//
//        // change existing finalbone mat instead of making new anim!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//        // Update the blended animation with the interpolated transformation
//        m_BlendedAnimation.GetBones()[i].m_LocalTransform = interpolatedTransform;
//    }
//}