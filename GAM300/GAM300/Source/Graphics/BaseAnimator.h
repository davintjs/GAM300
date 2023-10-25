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

//#include <glm/glm.hpp>
//#include <glm/gtx/quaternion.hpp>
//#include "Framebuffer.h"
#include "AnimationManager.h"
#include "Properties.h"




struct BaseAnimator /*: property::base*/
{
public:

	void Init();

	void SetAnimation(Animation* animation);

	void UpdateAnimation(float dt);

	void PlayAnimation(Animation* pAnimation);

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	std::vector<glm::mat4> GetFinalBoneMatrices();

	bool AnimationAttached();

	//property_vtable();

protected:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;

};

//property_begin_name(BaseAnimator, "BaseAnimator") {
//	property_var(m_CurrentAnimation).Name("CurrentAnimation"),
//
//}property_vend_h(BaseAnimator)

#endif // !BASEANIMATOR_H