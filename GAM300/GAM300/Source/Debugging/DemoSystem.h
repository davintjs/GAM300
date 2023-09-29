/*!***************************************************************************************
\file			DemoSystem.h
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           18/09/2023

\brief
    This file contains the declarations of the following:
    1. Demo system for debugging and testing purposes

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef DEMOSYSTEM_H
#define DEMOSYSTEM_H

#include "Core/SystemInterface.h"
#include "Core/Events.h"

#define MyDemoSystem DemoSystem::Instance()

ENGINE_SYSTEM(DemoSystem) // For debugging and demoing purposes
{
public:
	// Initialize the system
	void Init();

	// Update the system
	void Update(float dt);

	// Exit the system
	void Exit();

private:

};

#endif // !DEMOSYSTEM_H