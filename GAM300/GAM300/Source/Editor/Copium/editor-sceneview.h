/*!***************************************************************************************
\file			editor-sceneview.h
\project
\author			Sean Ngo

\par			Course: GAM250
\par			Section:
\date			16/09/2022

\brief
	This file holds the declaration of functions for editor-sceneview.cpp.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#ifndef EDITOR_SCENEVIEW_H
#define EDITOR_SCENEVIEW_H

#include <glm/vec2.hpp>
#include "SceneManager/scene-manager.h"

namespace Copium
{
	class EditorSceneView
	{
	public:
		/***************************************************************************/
		/*!
		\brief
			Initializes the editor scene view
		*/
		/***************************************************************************/
		void init();

		/***************************************************************************/
		/*!
		\brief
			Updates the editor scene view
		*/
		/***************************************************************************/
		void update();

		/***************************************************************************/
		/*!
		\brief
			Exits the editor scene view
		*/
		/***************************************************************************/
		void exit();

		/***************************************************************************/
		/*!
		\brief
			Resize the scene view
		\param _newDimension
			The new dimensions to resize to
		*/
		/***************************************************************************/
		void resize_sceneview(glm::vec2 _newDimension);

		/***************************************************************************/
		/*!
		\brief
			Updates the gizmos of selected object. Currently allows for scaling,
			rotating and translating of selected object.
		*/
		/***************************************************************************/
		void update_gizmos();
		
		// Accessing Properties

		// Scene Properties
		const bool& is_window_focused() { return windowFocused; }
		const bool& is_window_hovered() { return windowHovered; }

		int const get_width() { return sceneWidth; }
		void const set_width(int _width) { sceneWidth = _width; }

		int const get_height() { return sceneHeight; }
		void const set_height(int _height) { sceneHeight = _height; }

		glm::vec2 const get_position() { return scenePosition; }
		void const set_position(glm::vec2 _position) { scenePosition = _position; }

		glm::vec2 const get_dimension() { return sceneDimension; }
		void const set_dimension(glm::vec2 _dimension) { sceneDimension = _dimension; }

	private:
		/* Scene Data *******************************************************************/
		bool windowFocused = false, windowHovered = false;
		int sceneWidth = 1280, sceneHeight = 720; // The dimension of the viewport
		glm::vec2 sceneDimension{0}; // The dimension of the viewport as vector 2
		glm::vec2 scenePosition{0}; // The position of the viewport
	};

}
#endif // !EDITOR_SCENEVIEW_H
