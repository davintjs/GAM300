/*!***************************************************************************************
\file			BaseAnimator.h
\project
\author         Theophelia Tan

\par			Course: GAM300
\date           23/10/2023

\brief
	This file contains the declarations of the following:
	1. 

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef BASEANIMATOR_H
#define BASEANIMATOR_H

#include <glm/glm.hpp>
#include <Properties.h>
#include "AnimationManager.h"

#define DEFAULT_ANIM DEFAULT_ASSETS["None.anim"]

struct BaseAnimator : property::base
{
public:

	BaseAnimator();

	void UpdateAnimation(float dt, glm::mat4& pTransform);

	void ChangeState();

	void PlayAnimation(Animation* pAnimation);

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	// Sets the default state of the animator
	void SetDefaultState(const std::string& _defaultState);

	// Sets the next state to run after the current state ends
	void SetNextState(const std::string& _nextState);

	// Interrupt the current state and sets another state to run
	void SetState(const std::string& _state);

	// Gets the current animation state progress
	float GetProgress() { return m_CurrentTime / (endTime - startTime); }

	AnimationState* GetCurrentState() { return currentState; }

	std::vector<glm::mat4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; }
	std::vector<glm::mat4>* GetFinalBoneMatricesPointer() { return &m_FinalBoneMatrices; }

	bool AnimationAttached();
	void CalculateBlendFactor(float transitionDuration);
	void CalculateBlendedBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
		//void InterpolateAnimations(Animation& firstAnimation, Animation& secondAnimation);

	std::vector<glm::mat4> m_FinalBoneMatrices;
	Engine::GUID<AnimationAsset> animID{0};
	Engine::GUID<AnimationAsset> prevAnimID{0};
	AnimationState* defaultState; // Bean: Temporary
	AnimationState* currentState;
	AnimationState* nextState;
	std::string stateName;
	std::string stateNextName;
	int m_FinalBoneMatIdx; // to access copy of original anim
	int m_AnimationIdx; // to access copy of original anim
	int m_SecondAnimationIdx;
	float m_CurrentTime;
	float startTime;
	float endTime;
	float blendDuration;

	bool playing;
	int currBlendState{};
	int blendedBones;
	enum blendStates {
		notblending,
		blending,
		blended
	};

	//// temp?
	//Animation m_BlendedAnimation;

	property_vtable();
};

property_begin_name(BaseAnimator, "BaseAnimator") {
	property_var(animID).Name("Animation"),
	property_var(stateName).Name("Current State").Flags(property::flags::SHOW_READONLY),
	property_var(stateNextName).Name("Next State").Flags(property::flags::SHOW_READONLY),
	property_var(m_CurrentTime).Name("Time"),
	property_var(startTime).Name("StartTime"),
	property_var(endTime).Name("EndTime"),
	property_var(playing).Name("Playing"),

}property_vend_h(BaseAnimator)

#endif // !BASEANIMATOR_H