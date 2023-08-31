/*!***************************************************************************************
\file			editor-consolelog.h
\project
\author			Shawn Tanary

\par			Course: GAM250
\par			Section:
\date			20/09/2022

\brief
	This file contins functions that create the logger using IMGUI for the editor.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#pragma once

#ifndef CONSOLELOG_H

#define CONSOLELOG_H

#include <imgui.h>

namespace Window
{
	namespace EditorConsole
	{
		inline bool isConsoleLogOpen;


		struct EditorConsoleLog
		{
			ImGuiTextBuffer     Buf; //Storage for all entries
			ImGuiTextFilter     Search; //Used to search for specific words
			ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
			bool                keepScrolling;  // Keep scrolling if scrollbar is already at the bottom.
			std::string		    performanceText;

			/***************************************************************************/
			/*!
			\brief
				Constructor, Clears the buffer and set keepScrolling to true
			*/
			/**************************************************************************/
			EditorConsoleLog();

			/***************************************************************************/
			/*!
			\brief
				Clears the buffer
			*/
			/**************************************************************************/
			void Clear();

			/***************************************************************************/
			/*!
			\brief
				Adds a new entry to the editor log. It follows the same format as C printf.
				Ex:
					AddLog("%d. Hello World",1);
					AddLog("Characters: %c %c \n", 'b', 66);
			\param fmt
				This follows the printf style formatting
			*/
			/**************************************************************************/
			void    AddLog(const char* fmt, ...) IM_FMTARGS(2);

			/***************************************************************************/
			/*!
			\brief
				Adds a new entry to the editor log. Note that an endline will be appended
				to the back of your string
			\param str
				the message that you wish to add
			*/
			/**************************************************************************/
			void add_logEntry(std::string str);

			/***************************************************************************/
			/*!
			\brief
				Sets the performance text for the performance viewer
			\param str
				the message that you wish to add
			*/
			/**************************************************************************/
			void set_performancetext(std::string str);

			/***************************************************************************/
			/*!
			\brief
				gets the performance text from the performance viewer
			\return
				the performance text
			*/
			/**************************************************************************/
			std::string get_performancetext();

			/***************************************************************************/
			/*!
			\brief
				sets the console to be in focus and bring it to the front
			\return
				nothing
			*/
			/**************************************************************************/
			void bring_to_front();
		};


		static EditorConsoleLog editorLog;
		
		
		

		/***************************************************************************/
		/*!
		\brief
			Initialize function called at the start of program to reset fields
		*/
		/**************************************************************************/
		void init();

		/***************************************************************************/
		/*!
		\brief
			Update function called every frame for ImGui window
		*/
		/**************************************************************************/
		void update();
	}
}

#endif // !CONSOLELOG_H