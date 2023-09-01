/*!***************************************************************************************
\file			editor-camera.h
\project
\author			Sean Ngo

\par			Course: GAM250
\par			Section:
\date			15/10/2022

\brief
	Contains the function declarations of the editor camera in the scene view.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include "Graphics/base-camera.h"
#include "Messaging/message-system.h"

namespace Copium
{
	// Editor camera which moves around in the scene view
	class EditorCamera : public BaseCamera, public IReceiver
	{
	public:
		/***************************************************************************/
		/*!
		\brief
			Constructs the editor camera
		\param _width
			The width of the screen
		\param _height
			The height of the screen
		\param _orthographic
			Is it orthographic or perspective projection
		*/
		/**************************************************************************/
		void init(float _width, float _height, bool _orthographic = true);
		
		/***************************************************************************/
		/*!
		\brief
			Updates the editor camera
		*/
		/**************************************************************************/
		void update();

		/**************************************************************************/
		/*!
		\brief
			Interface function for MessageSystem to call for IReceivers to handle
			a messageType
		\param _mType
			The type of message to receive
		*/
		/**************************************************************************/
		void handleMessage(MESSAGE_TYPE _mType);

		// Accessing Properties
		float get_zoom() const { return orthographicSize; }

		/***************************************************************************/
		/*!
		\brief
			Gets the orthographic size of the camera
		*/
		/**************************************************************************/
		float get_zoom_speed() const;
		
		/***************************************************************************/
		/*!
		\brief
			Gets the normalize device coordinates of the current mouse position
		\return
			The position in vector 2
		*/
		/**************************************************************************/
		glm::vec2 get_ndc() const;

	private:
		/***************************************************************************/
		/*!
		\brief
			The input from the user to move the camera around in the scene view in
			the editor
		*/
		/**************************************************************************/
		void mouse_controls();

	private:
		/* Camera Data ******************************************************************/
		glm::vec2 mousePosition{0};
		
		bool dynamicClipping = false; // Bean: To be implemented in the future

		float cameraSpeed = 1.f;
	};
}
#endif // !EDITOR_CAMERA_H