/*!***************************************************************************************
\file			editor-game.h
\project
\author			Sean Ngo

\par			Course: GAM250
\par			Section:
\date			21/11/2022

\brief
	This file holds the declarations of the functions in the editor game gui

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#ifndef EDITOR_GAME_H
#define EDITOR_GAME_H

#include <glm/vec2.hpp>

namespace Copium
{
	class EditorGame
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
		void resize_game(glm::vec2 _newDimension);

		// Accessing Properties

		// Scene Properties
		const bool& is_window_focused() { return windowFocused; }
		const bool& is_window_hovered() { return windowHovered; }

		const float& get_indent() { return indent; }

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
		float indent = 0.f;
		int sceneWidth = 0, sceneHeight = 0; // The dimension of the viewport
		glm::vec2 sceneDimension{ 0 }; // The dimension of the viewport as vector 2
		glm::vec2 scenePosition{ 0 }; // The position of the viewport
	};
}
#endif // !EDITOR_GAME_H
