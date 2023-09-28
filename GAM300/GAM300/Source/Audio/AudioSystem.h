/*!***************************************************************************************
\file			AudioSystem.h
\project
\author         Lian Khai Kiat

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the Declaration of Audio System

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#pragma once

#include "Core/SystemInterface.h"

ENGINE_SYSTEM(AudioSystem) {
public:
	void Init();
	void Update(float dt);
	void Exit();
};