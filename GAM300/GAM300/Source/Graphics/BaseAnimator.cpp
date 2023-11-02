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

void BaseAnimator::Init()
{
    m_CurrentTime = 0.0f;
    //m_CurrentAnimation = animation;
    hasAnimation = false;

    m_FinalBoneMatrices.reserve(100);

    for (int i = 0; i < 100; i++)
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

void BaseAnimator::SetAnimation(Animation* animation)
{
    m_CurrentAnimation = *animation;

    // Need to set to current entity's transform for either the bones or assimpnodedata
    // CalculateBoneTransform(&m_CurrentAnimation.GetRootNode(), glm::mat4(1.f));
    hasAnimation = true;
}

void BaseAnimator::UpdateAnimation(float dt)
{
    m_DeltaTime = dt;
    if (hasAnimation)
    {
        m_CurrentTime += m_CurrentAnimation.GetTicksPerSecond() * dt;
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation.GetDuration());
        CalculateBoneTransform(&m_CurrentAnimation.GetRootNode(), glm::mat4(1.f));
    }
}

void BaseAnimator::PlayAnimation(Animation* pAnimation)
{
    m_CurrentAnimation = *pAnimation;
    m_CurrentTime = 0.0f;
}

void BaseAnimator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone* Bone = m_CurrentAnimation.FindBone(nodeName);

    if (Bone)
    {
        Bone->Update(m_CurrentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = m_CurrentAnimation.GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        m_FinalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(&node->children[i], globalTransformation);
}

std::vector<glm::mat4> BaseAnimator::GetFinalBoneMatrices()
{
    return m_FinalBoneMatrices;
}

std::vector<glm::mat4>* BaseAnimator::GetFinalBoneMatricesPointer()
{
    return &m_FinalBoneMatrices;
}

bool BaseAnimator::AnimationAttached() {
    // Check if m_CurrentAnimation is not nullptr (i.e., it's attached)
    return hasAnimation;
}
