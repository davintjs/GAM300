/*!***************************************************************************************
\file			editor-colortheme.h
\project
\author			Shawn Tanary

\par			Course: GAM250
\par			Section:
\date			25/11/2022

\brief
	Contains the function declarations of the editor-colortheme

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#pragma once
#include <imgui.h>
#include <rapidjson/document.h>
#include "Windows/windows-utils.h"
#include "Math/math-library.h"

namespace Copium
{
		inline bool isColorThemeOpen;

		//the default values
		static Copium::Math::Vec3 color_for_text = Copium::Math::Vec3(255.f / 255.f, 233.f / 255.f, 0.f / 255.f);
		static Copium::Math::Vec3 color_for_head = Copium::Math::Vec3(238.f / 255.f, 5.f / 255.f, 43.f / 255.f);
		static Copium::Math::Vec3 color_for_area = Copium::Math::Vec3(238.f / 255.f, 75.f / 255.f, 43.f / 255.f);
		static Copium::Math::Vec3 color_for_body = Copium::Math::Vec3(238.f / 255.f, 155.f / 255.f, 43.f / 255.f);
		static Copium::Math::Vec3 color_for_pops = Copium::Math::Vec3(238.f / 255.f, 105.f / 255.f, 43.f / 255.f);


		class EditorColorTheme 
		{
		public:

			struct ColorPreset
			{
				ImVec4 color_for_text;
				ImVec4 color_for_head;
				ImVec4 color_for_area;
				ImVec4 color_for_body;
				ImVec4 color_for_pops;
			};


			/***************************************************************************/
			/*!
			\brief
				Initializes the editor color theme picker
			*/
			/***************************************************************************/
			void init();

			/***************************************************************************/
			/*!
			\brief
				Updates the editor color theme picker
			*/
			/***************************************************************************/
			void update();

			/*******************************************************************************
				/*!
				*
				\brief
					Serialize this transform component's data to the specified rapidjson Value

				\param color_for_text
					the color to use for the text

				\param color_for_head
					the color to use for the head

				\param color_for_area
					the color to use for the area

				\param color_for_body
					the color to use for the body

				\param color_for_pops
					the color to use for the pop ups

				\return
					void
				*/
				/*******************************************************************************/
			void setTheme(Copium::Math::Vec3 color_for_text, Copium::Math::Vec3 color_for_head,
				Copium::Math::Vec3 color_for_area, Copium::Math::Vec3 color_for_body,
				Copium::Math::Vec3 color_for_pops);

			/*******************************************************************************
				/*!
				*
				\brief
					Serialize the data to the specified rapidjson file path

				\param _filepath
					reference to the  filepath to serialize its data to

				\return
					void
			*/
			/*******************************************************************************/
			void serialize(const std::string& _filepath);

			/*******************************************************************************
				/*!
				*
				\brief
					Deserialize the data to the specified rapidjson file path

				\param _filepath
					reference to the  filepath to deserialize its data from

				\return
					void
			*/
			/*******************************************************************************/
			void deserialize(const std::string& _filepath);
		};
}
