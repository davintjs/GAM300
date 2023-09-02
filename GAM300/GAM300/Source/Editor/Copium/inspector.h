/*!***************************************************************************************
\file			inspector.h
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			17/07/2022

\brief
	Declares Window::Inspector namespace to have ImGui functions for displaying and
	interacting with the window.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#pragma once

#ifndef INSPECTOR_H

#define INSPECTOR_H

//INCLUDES

#include <imgui.h>
#include "SceneManager/scene-manager.h"
#include "GameObject/game-object.h"
#include "Audio/sound-system.h"

namespace Copium
{
	class EditorInspector
	{
	public:
		/***************************************************************************/
		/*!
			\brief
				Initializes the inspector
		*/
		/**************************************************************************/
		void init();
		/***************************************************************************/
		/*!
			\brief
				Updates the inspector
		*/
		/**************************************************************************/
		void update();
		/***************************************************************************/
		/*!
			\brief
				Cleaning up of resources used by the inspector
		*/
		/**************************************************************************/
		void exit();

		/***************************************************************************/
		/*!
			\brief
				Gets reference of boolean on whether inspector is focused
			\return
				Reference of bool on whether the window is focused
		*/
		/**************************************************************************/
		bool& getFocused() { return isFocused; }
		/***************************************************************************/
		/*!
			\brief
				Gets reference of boolean on whether inspector is opened
			\return
				Reference of bool on whether the window is opened
		*/
		/**************************************************************************/
		bool& status() { return isInspectorOpen; }
	private:
		bool isInspectorOpen;
		bool isFocused;
		UUID targetGameobjectName;

		/***************************************************************************/
		/*!
			\brief
				Callback when entering preview mode
		*/
		/**************************************************************************/
		void CallbackStartPreview(StartPreviewEvent* pEvent);

		/***************************************************************************/
		/*!
			\brief
				Callback when exiting preview mode
		*/
		/**************************************************************************/
		void CallbackStopPreview(StopPreviewEvent* pEvent);
	};
}


#endif // !INSPECTOR_H