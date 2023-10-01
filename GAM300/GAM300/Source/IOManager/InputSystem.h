/*!***************************************************************************************
\file			InputSystem.h
\project
\author         Euan Lim

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations of the Input System

All content ? 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H
#include "Core/SystemInterface.h"

ENGINE_SYSTEM(InputSystem)
{
public:
	void Init();
	void Update(float dt);
	void Exit();
};
#endif // !INPUT_SYSTEM_H