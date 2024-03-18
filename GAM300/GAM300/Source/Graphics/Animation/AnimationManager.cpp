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
#include "Graphics/GraphicsHeaders.h"
#include "Graphics/MeshManager.h"
#include "Physics/PhysicsSystem.h"
#include "Scene/Identifiers.h"

#include "AssetManager/ModelCompiler.h"
#include "Core/EventsManager.h"
#include "Scene/SceneManager.h"

#include "IOManager/InputHandler.h"

void Animation_Manager::Init()
{  
    EVENTS.Subscribe(this, &Animation_Manager::CallbackAnimationAssetLoaded);
    EVENTS.Subscribe(this, &Animation_Manager::CallbackAnimationSceneLoad);
}

void Animation_Manager::Update(float dt)
{
    //std::cout << "Anim DT: " << dt << std::endl;
    if (dt == 0)
        return;
    Scene& currentScene = MySceneManager.GetCurrentScene();

    for (Camera& CurrCam : currentScene.GetArray<Camera>())
    {
        glm::vec3 minBound = { -1.f, -1.f, -1.f };
        glm::vec3 maxBound = { 1.f, 1.f, 1.f };
        for (Animator& animator : currentScene.GetArray<Animator>())
        {
    #ifndef _BUILD
            if (animator.state == DELETED) continue;
    #endif
            if (!currentScene.IsActive(animator)) continue;

            Entity& entity = currentScene.Get<Entity>(animator);
            if (!currentScene.IsActive(entity)) continue;

            Transform& transForm{ currentScene.Get<Transform>(animator) };

            if (animator.animID != animator.prevAnimID)
            {
                //PRINT("Animator ID: ", animator.UUID(), "\n");
                animator.prevAnimID = animator.animID;
                animator.m_CurrentTime = 0.f;
                if (animator.animID == 0) // Replaced to empty
                    animator.m_AnimationIdx = -1;
                else
                    animator.m_AnimationIdx = AddAnimCopy(animator.animID);

                animator.SetDefaultState("Idle");
                //animator.ChangeState();

                animator.CreateRig(&transForm);
            }

            float distance = glm::distance(CurrCam.GetCameraPosition(), transForm.GetGlobalTranslation());

            bool withinFrustum = CurrCam.WithinFrustum(transForm, minBound, maxBound);

            if (distance > 20.f && !withinFrustum) continue;

            if (distance > 20.f && withinFrustum) // Only raycast for those within frustum
            {
                Transform& cameraTran{ currentScene.Get<Transform>(CurrCam) };
                glm::vec3 p = cameraTran.GetGlobalTranslation();
                Transform* parent = transForm.GetParent();
                glm::vec3 d;
                if (parent)
                    d = parent->GetGlobalTranslation() - p;
                else
                    d = transForm.GetGlobalTranslation() - p;

                JPH::RVec3 physicsVec3 = { p.x, p.y, p.z };
                EngineRayCastResult ray = PHYSICS.CastRay(physicsVec3, { d.x, d.y, d.z }, 0.95f);
                std::string tagName = IDENTIFIERS.GetTagString(ray.tag.tagName);
                if (ray.hit && tagName.compare("Enemy") && ray.tag.physicsLayerIndex != 1) // If it hits something
                    continue;
            }

            if (animator.playing && animator.AnimationAttached())
            {
                glm::mat4 translate = glm::mat4(1.f);
                animator.UpdateAnimation(dt, translate);
            }
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

    auto& map = animation.GetBones();
    for (const Bone bone : _animationAsset.bones)
    {
        map[bone.m_Name] = bone;
    }
    animation.GetDuration() = _animationAsset.duration;
    animation.GetTicksPerSecond() = _animationAsset.ticksPerSecond;
    animation.GetRootNode() = _animationAsset.rootNode;
    animation.GetAnimationStates() = _animationAsset.animationStates;

    mAnimationContainer[_guid] = animation;
}

void Animation_Manager::CallbackAnimationAssetLoaded(AssetLoadedEvent<AnimationAsset>* pEvent)
{
    AddAnimation(pEvent->asset, pEvent->asset.importer->guid);
}

void Animation_Manager::CallbackAnimationSceneLoad(SceneStartEvent* pEvent)
{
    Scene& currentScene = MySceneManager.GetCurrentScene();
    for (Animator& animator : currentScene.GetArray<Animator>())
    {
        Transform& t = currentScene.Get<Transform>(animator);
        animator.CreateRig(&t);
    }
}