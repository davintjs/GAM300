/*!***************************************************************************************
\file			AnimationManager.h
\project
\author         Euphrasia Theophelia Tan Ee Mun

\par			Course: GAM300
\date           10/10/2023

\brief
    This file contains the Animation Manager and the declarations of its related functions.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include "Animation.h"

#include "Core/SystemInterface.h"
#include "Core/Events.h"
#include "AssetManager/AssetTypes.h"

#define AnimationManager Animation_Manager::Instance()

SINGLETON(Animation_Manager)
{
public:
    void Init();

    void Update(float dt);

    void Exit();

    int AddAnimCopy(const Engine::GUID& animguid)//;
    {
        int idx = (int)mAnimationSceneContainer.size();
        mAnimationSceneContainer.emplace(idx, mAnimationContainer[animguid]);
        return idx;
    }

    void AddAnimation(const AnimationAsset& _animationAsset, const Engine::GUID& _guid);

    Animation& GetAnimCopy(const int& idx)
    {
        return mAnimationSceneContainer[idx];
    }

    void CallbackAnimationAssetLoaded(AssetLoadedEvent<AnimationAsset>* pEvent);

private:

    //std::unordered_map<std::string, std::pair<char const*, GLuint>> mAnimationContainer; // GUID, <file name, GLuint>
    std::unordered_map<Engine::GUID, Animation> mAnimationContainer; // GUID, Animation
    std::unordered_map<int, Animation> mAnimationSceneContainer; // Index, Animation -> for current anims in scene
};
#endif // !ANIMATIONMANAGER_H