/*!***************************************************************************************
\file			logic-system.h
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			30/10/2022

\brief
	This file declares the functions for the Logic System

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#pragma once

#ifndef LOGIC_SYSTEM_H

#define LOGIC_SYSTEM_H

//INCLUDES
#include "Core/SystemInterface.h"
#include "Core/EventsManager.h"
//USING

ENGINE_RUNTIME_SYSTEM(LogicSystem)
{
public:
	/**************************************************************************/
	/*!
		\brief
			Inits logic system
	*/
	/**************************************************************************/
	void Init();
	/**************************************************************************/
	/*!
		\brief
			Calls the update function of UI buttons and Script Components
	*/
	/**************************************************************************/
	void Update(float dt);
	/**************************************************************************/
	/*!
		\brief
			Cleanup of logic system
	*/
	/**************************************************************************/
	void Exit();

	/**************************************************************************/
	/*!
		\brief
			Callback to when the scene is linked, aka finished deserializing
	*/
	/**************************************************************************/
	void CallbackSceneStart(SceneStartEvent* IEvent);
};


#endif // !LOGIC_SYSTEM_H
