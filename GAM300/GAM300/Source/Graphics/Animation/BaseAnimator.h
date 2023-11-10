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

	void PlayAnimation(Animation* pAnimation);

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	std::vector<glm::mat4> GetFinalBoneMatrices();
	std::vector<glm::mat4>* GetFinalBoneMatricesPointer();

	bool AnimationAttached();

	std::vector<glm::mat4> m_FinalBoneMatrices;
	Engine::GUID animID{ DEFAULT_ANIM };
	Engine::GUID prevAnimID{ DEFAULT_ANIM };
	int m_FinalBoneMatIdx; // to access copy of original anim
	int m_AnimationIdx; // to access copy of original anim
	float m_CurrentTime;
	float startTime;
	float endTime;

	property_vtable();
};

property_begin_name(BaseAnimator, "BaseAnimator") {
	property_var(animID).Name("Animation"),
	property_var(m_CurrentTime).Name("Time"),
	property_var(startTime).Name("StartTime"),
	property_var(endTime).Name("EndTime"),

}property_vend_h(BaseAnimator)

#endif // !BASEANIMATOR_H