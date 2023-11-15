/*!***************************************************************************************
\file			AnimationManager.cpp
\project
\author         Euphrasia Theophelia Tan Ee Mun

\par			Course: GAM300
\date           10/10/2023

\brief
    This file contains the Animation Manager and the declarations of its related functions.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "AnimationManager.h"
#include "BaseAnimator.h"

#include "AssetManager/ModelCompiler.h"
#include "Core/EventsManager.h"
#include "Scene/SceneManager.h"

void Animation_Manager::Init()
{  
    EVENTS.Subscribe(this, &Animation_Manager::CallbackAnimationAssetLoaded);
}

void Animation_Manager::Update(float dt)
{
    Scene& currentScene = MySceneManager.GetCurrentScene();
    for (Animator& animator : currentScene.GetArray<Animator>()) // temp,  move to subsys later
    {
#ifndef _BUILD
        if (animator.state == DELETED) continue;
#endif
        if (!currentScene.IsActive(animator)) continue;

        if (animator.animID != animator.prevAnimID) // This check should be in the animator itself
        {
            PRINT("Animator ID: ", animator.UUID(), "\n");
            animator.prevAnimID = animator.animID;
            animator.m_CurrentTime = 0.f;
            if (animator.animID == 0)
                animator.m_AnimationIdx = -1;
            else
                animator.m_AnimationIdx = AddAnimCopy(animator.animID); // Bean: Should only do once
        }

        if (animator.playing && animator.AnimationAttached())
        {
            glm::mat4 translate = glm::mat4(1.f);
            //glm::mat4 translate = glm::translate(glm::mat4(1.f), currentScene.Get<Transform>(animator).GetTranslation());
            animator.UpdateAnimation(dt, translate);
        }
    }
}

void Animation_Manager::Exit()
{
}

void Animation_Manager::AddAnimation(const AnimationAsset& _animationAsset, const Engine::GUID<AnimationAsset>& _guid)
{
    Animation animation;
    animation.GetBoneCount() = _animationAsset.boneCounter;
    animation.GetBoneInfoMap() = _animationAsset.boneInfoMap;
    animation.GetBones() = _animationAsset.bones;
    animation.GetDuration() = _animationAsset.duration;
    animation.GetTicksPerSecond() = _animationAsset.ticksPerSecond;
    animation.GetRootNode() = _animationAsset.rootNode;
    animation.GetAnimationRange() = _animationAsset.animationRange;

    mAnimationContainer[_guid] = animation;
}

void Animation_Manager::CallbackAnimationAssetLoaded(AssetLoadedEvent<AnimationAsset>* pEvent)
{
    AddAnimation(pEvent->asset, pEvent->guid);
}