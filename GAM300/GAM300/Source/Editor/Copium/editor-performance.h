/*!***************************************************************************************
\file			editor-performance.h
\project
\author			Matthew Lau

\par			Course: GAM250
\par			Section:
\date			9/03/2023

\brief
	Contains declarations of the EditorPerformance class. It is responsible for the performance viewer window

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef EDITOR_PERFORMANCE_H
#define EDITOR_PERFORMANCE_H
#pragma once

#include <imgui.h>
#include "Windows/windows-utils.h"

namespace Copium
{
	inline bool isPerformanceOpen;

	class EditorPerformance
	{
	public:
		/*******************************************************************************
		/*!
		*
		\brief
			Default constructor for EditorPerformance class

		\return
			void
		*/
		/*******************************************************************************/
		EditorPerformance() : open{ false }{}
		/*******************************************************************************
		/*!
		*
		\brief
			Initialise the EditorPerformance class

		\return 
			void
		*/
		/*******************************************************************************/
		void init();
		/*******************************************************************************
		/*!
		*
		\brief
			Display the window but only draw content when window tab is open to user

		\return
			void
		*/
		/*******************************************************************************/
		void update();
		/*******************************************************************************
		/*!
		*
		\brief
			Exit function for EditorPerformance class. At present, does nothing as no clean up is 
			required for EditorPerformance class

		\return
			void
		*/
		/*******************************************************************************/
		void exit();
	private:
		std::string performanceText;
		bool open;
	};
}

#endif
