/*!***************************************************************************************
\file			editor-camera.cpp
\project
\author			Sean Ngo

\par			Course: GAM250
\par			Section:
\date			15/10/2022

\brief
	Contains function definitions for the editor camera which draws the view in the
	scene view.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#include "pch.h"

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "Windows/windows-input.h"
#include "Editor/editor-camera.h"
#include "Editor/editor-system.h"
#include "SceneManager/scene-manager.h"

namespace
{
	bool enableCamera = false;
}

namespace Copium
{
	void EditorCamera::init(float _width, float _height, bool _orthographic)
	{
		BaseCamera::init(_width, _height, CameraType::SCENEVIEW, _orthographic);
		MyMessageSystem.subscribe(MESSAGE_TYPE::MT_SCENE_DESERIALIZED, this);
	}

	void EditorCamera::update()
	{
		EditorSceneView* sceneView = EditorSystem::Instance()->get_scene_view();
		if (sceneView->is_window_focused() || sceneView->is_window_hovered())
		{
			mouse_controls();
		}

		static bool debugMode = false;
		if (sceneView->is_window_focused() || sceneView->is_window_hovered())
		{
			if (MyInputSystem.is_key_held(GLFW_KEY_LEFT_SHIFT) && MyInputSystem.is_key_pressed(GLFW_KEY_D))
			{
				debugMode = !debugMode;
			}
		}

		if (debugMode)
			draw.enable(DRAW::DEBUG);
		else if (!debugMode)
			draw.disable(DRAW::DEBUG);

		BaseCamera::update();
	}

	void EditorCamera::handleMessage(MESSAGE_TYPE _mType)
	{
		if (_mType == MESSAGE_TYPE::MT_START_PREVIEW)
		{
			//enableCamera = true;
		}
		else if (_mType == MESSAGE_TYPE::MT_STOP_PREVIEW)
		{
			//enableCamera = false;
		}

		if (_mType == MESSAGE_TYPE::MT_SCENE_DESERIALIZED)
		{
			draw.ResetRenderer();
		}
	}

	float EditorCamera::get_zoom_speed() const
	{
		float tempDistance = orthographicSize * 0.2f;
		tempDistance = std::max(orthographicSize, 0.25f); // Min speed is 0
		float speed = tempDistance * tempDistance;
		speed = std::min(speed, 50.f); // The max speed currently is 50
		return speed;
	}

	glm::vec2 EditorCamera::get_ndc() const
	{
		EditorSceneView* sceneView = EditorSystem::Instance()->get_scene_view();
		glm::vec2 scenePos = sceneView->get_position();
		scenePos.y += 22.f; // Offset from the tab
		glm::vec2 sceneDim = sceneView->get_dimension();
		Math::Vec2 mousePos = MyInputSystem.get_mouseposition();
		//PRINT("Mouse position: " << mousePos.x << " " << mousePos.y);
		glm::vec2 centreOfScene = { scenePos.x + sceneDim.x / 2, scenePos.y + sceneDim.y / 2 };
		glm::vec2 mouseScenePos = { mousePos.x - centreOfScene.x, centreOfScene.y - mousePos.y };
		glm::vec2 mouseToNDC = { mouseScenePos.x / sceneDim.y * 2, mouseScenePos.y / sceneDim.y * 2 };
		mouseToNDC *= orthographicSize;
		glm::vec2 worldNDC = { mouseToNDC.x + viewer.x, mouseToNDC.y + viewer.y };
		return worldNDC;
	}

	void EditorCamera::mouse_controls()
	{
		Math::Vec2 mousePos = MyInputSystem.get_mouseposition();
		//PRINT("Mouse position: " << mousePos.x << " " << mousePos.y);
		//PRINT("NDC: " << get_ndc().x << " " << get_ndc().y);
		glm::vec2 worldNDC = get_ndc() - glm::vec2(viewer.x , viewer.y);
		glm::vec2 delta = (worldNDC - mousePosition) * 4.f;

		mousePosition = worldNDC;

		// Movement using right click and drag
		if (MyInputSystem.is_mousebutton_pressed(1) || MyInputSystem.is_mousebutton_pressed(2))
		{
			ImGui::SetWindowFocus("Scene View");
			glm::vec2 speed = get_pan_speed();
			glm::vec3 point = focalPoint;
			point += -get_up_direction() * delta.y * speed.y;
			point += -get_right_direction() * delta.x * speed.x;

			// Bean: shouldnt be necessary here
			// Clamping camera within boundary
			point.x = std::clamp(point.x, -100.f, 100.f);
			point.y = std::clamp(point.y, -100.f, 100.f);

			viewer = point;
		}

		//PRINT("Camera Pos: " << focalPoint.x << " " << focalPoint.y);
			 
		//if (MyInputSystem.is_key_held(GLFW_KEY_LEFT_CONTROL))
		//{
		//	glm::vec2 speed = get_pan_speed();
		//	// Bean: Zoomlevel should be positive
		//	if (MyInputSystem.is_key_held(GLFW_KEY_W)) // Up
		//	{
		//		focalPoint += get_up_direction() * 0.1f * speed.y * zoomLevel;
		//	}
		//	if (MyInputSystem.is_key_held(GLFW_KEY_A)) // Left
		//	{
		//		focalPoint += -get_right_direction() * 0.1f * speed.x * zoomLevel;
		//	}
		//	if (MyInputSystem.is_key_held(GLFW_KEY_S)) // Down
		//	{
		//		focalPoint += get_up_direction() * -0.1f * speed.y * zoomLevel;
		//	}
		//	if (MyInputSystem.is_key_held(GLFW_KEY_D)) // Right
		//	{
		//		focalPoint += -get_right_direction() * -0.1f * speed.x * zoomLevel;
		//	}
		//}

		// Rotation
		/*if (MyInputSystem.is_key_held(GLFW_KEY_LEFT_ALT))
		{
			glm::vec2 mouse{ MyInputSystem.get_mouseX(), MyInputSystem.get_mouseY() };
			glm::vec2 delta = (mouse - mousePosition) * 0.003f;
			mousePosition = mouse;

			if (MyInputSystem.is_mousebutton_pressed(GLFW_MOUSE_BUTTON_RIGHT))
			{
				float yawSign = (get_up_direction().y < 0.f) ? -1.f : 1.f;
				yaw += yawSign * delta.x * 0.8f;
				pitch += delta.y * 0.8f;
			}
		}*/

		// Zoom In and Out
		int scroll = (int) MyInputSystem.get_mousescroll();
		if (scroll && !enableCamera)
		{
			orthographicSize -= scroll * 0.1f * get_zoom_speed();

			orthographicSize = std::clamp(orthographicSize, 0.5f, 100.f);
			update_ortho_projection();
		}

		//scroll = MyInputSystem.get_mousescroll();
	}
}