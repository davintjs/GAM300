/*!***************************************************************************************
\file			EditorCamera.cpp
\project
\author         Euan Lim

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definitions of the editor camera and it's related functionalities
	that includes:
	1. Ray3D Class for ray casting ( Mouse Picking) and collision with OBB
	2. Editor Camera and it's functionalities

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "EditorCamera.h"
#include "Editor/EditorHeaders.h"
#include "Core/EventsManager.h"

void EditorCamera::Init()
{
	BaseCamera::Init();

	EVENTS.Subscribe(this, &EditorCamera::CallbackPanCamera);
	EVENTS.Subscribe(this, &EditorCamera::CallbackUpdateSceneGeometry);
}

void EditorCamera::Update(float dt)
{
	intersected = FLT_MAX;
	tempIntersect = 0.f;

	EditorWindowEvent e("Scene");
	EVENTS.Publish(&e);

	if(e.isHovered || isMoving)	// Rotating and Flying
		InputControls();

	if (e.isHovered && InputHandler::getMouseScrollState() != 0) // Zooming
		ZoomCamera();

	BaseCamera::Update();

	canMove = true;// The false check happens in editorscene, incase guizmo is being used
}

void EditorCamera::InputControls()
{
	glm::vec2 delta = (InputHandler::getMousePos() - prevMousePos) * 0.003f;
	prevMousePos = InputHandler::getMousePos();

	// To Move and rotate the editor camera
	isMoving = true;
	if (InputHandler::isMouseButtonHolding_R())
	{
		float speedModifer = 5.f;
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT_SHIFT))
			speedModifer = 20.f;

		//--------------------------------------------------------------
		// Rotating / Panning / Zooming
		//--------------------------------------------------------------

		if (canMove)
			RotateCamera(delta);

		//--------------------------------------------------------------
		// Moving the Editor Camera
		//--------------------------------------------------------------
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_W))
		{
			focalPoint += GetForwardVec() * speedModifer;
		}
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_A))
		{
			focalPoint += -GetRightVec() * speedModifer;
		}
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_S))
		{
			focalPoint += -GetForwardVec() * speedModifer;
		}
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_D))
		{
			focalPoint += GetRightVec() * speedModifer;
		}
	}
	else if (InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT_ALT))
	{
		if (InputHandler::isMouseButtonHolding_L())
			OrbitCamera(delta);
	}
	else if (isPanning && InputHandler::isMouseButtonHolding_L())
	{
		PanCamera(delta);
	}
	else
	{
		isMoving = false;
	}
}

glm::vec2 EditorCamera::GetMouseInNDC()
{
	glm::vec2 mousePosition = InputHandler::getMousePos();
	mousePosition.y -= GLFW_Handler::height - scenePosition.y - dimension.y;
	glm::vec2 mouseScenePosition = { mousePosition.x - scenePosition.x, mousePosition.y};
	glm::vec2 mouseToNDC = mouseScenePosition / dimension;

	glm::vec2 mouseTo1600By900 = mouseToNDC * glm::vec2(GLFW_Handler::width, GLFW_Handler::height);
	//mouseTo1600By900.y = GLFW_Handler::height - mouseTo1600By900.y;
	return mouseTo1600By900;
}

void EditorCamera::RotateCamera(const glm::vec2& _delta)
{
	OrbitCamera(_delta);
	focalPoint = GetFocalPoint();	
}

void EditorCamera::OrbitCamera(const glm::vec2& _delta)
{
	pitch += _delta.x * GetRotationSpeed();
	yaw -= _delta.y * GetRotationSpeed();
}

void EditorCamera::PanCamera(const glm::vec2& _delta)
{
	//std::cout << "Panning\n";
	glm::vec2 panSpeed = GetPanSpeed();
	focalPoint += -GetRightVec() * _delta.x * panSpeed.x * GetFocalLength();
	focalPoint += -GetUpVec() * _delta.y * panSpeed.y * GetFocalLength();
}

void EditorCamera::ZoomCamera()
{
	focalLength += -InputHandler::getMouseScrollState() * GetZoomSpeed();
	if (focalLength < 1.f)
	{
		focalLength = 1.1f;
	}
}

float EditorCamera::GetZoomSpeed()
{
	float distance = focalLength * 0.2f;
	distance = std::max(distance, 0.0f);
	float speed = distance * distance;
	speed = std::min(speed, 100.0f); // max speed = 100
	return speed * speedModifier;
}

glm::vec2 EditorCamera::GetPanSpeed() //  Copied from Cherno no cappo
{
	float x = std::min(dimension.x / 1000.f, 1.f);
	float xFactor = 0.22f * (x * x) - 0.1778f * x + 0.44f;

	float y = std::min(dimension.y / 1000.f, 1.f);
	float yFactor = 0.22f * (y * y) - 0.1778f * y + 0.44f;

	return { xFactor, yFactor };
}

Ray3D EditorCamera::Raycasting()
{
	glm::vec2 mousePos = GetMouseInNDC();
	EditorDebugger::Instance().AddLog("Mouse Position: %f %f\n", mousePos.x, mousePos.y);

	float x = (2.0f * (float)mousePos.x) / 1600.f - 1.0f;
	float y = (2.0f * (float)mousePos.y) / 900.f - 1.0f;

	glm::vec4 rayClip(x, y, -1.0f, 1.0f);
	glm::vec4 rayEye = glm::inverse(projMatrix) * rayClip;
	rayEye.z = -1.0f;
	rayEye.w = 0.0f;
	glm::vec4 inverseRayWorld = glm::inverse(viewMatrix) * rayEye;
	glm::vec3 rayWorld = glm::vec3(inverseRayWorld);
	rayWorld = glm::normalize(rayWorld);

	Ray3D temp;
	temp.origin = cameraPosition;
	temp.direction = rayWorld;

	return temp;
}

Ray3D EditorCamera::Raycasting(double xpos, double ypos, glm::mat4 proj, glm::mat4 view, glm::vec3 eye)
{
	EditorDebugger::Instance().AddLog("Mouse Position: %f %f\n", xpos, ypos);
	
	float x = (2.0f * (float)xpos) / 1600.f - 1.0f;
	float y = (2.0f * (float)ypos) / 900.f - 1.0f;

	glm::vec4 rayClip(x, y, -1.0f, 1.0f);
	glm::vec4 rayEye = glm::inverse(proj) * rayClip;
	rayEye.z = -1.0f;
	rayEye.w = 0.0f;
	glm::vec4 inverseRayWorld = glm::inverse(view) * rayEye;
	glm::vec3 rayWorld = glm::vec3(inverseRayWorld);
	rayWorld = glm::normalize(rayWorld);

	Ray3D temp;
	temp.origin = eye;
	temp.direction = rayWorld;

	return temp;
}

void EditorCamera::CallbackPanCamera(EditorPanCameraEvent* pEvent)
{
	isPanning = pEvent->isPanning;
}

void EditorCamera::CallbackUpdateSceneGeometry(EditorUpdateSceneGeometryEvent* pEvent)
{
	dimension = pEvent->dimension;
	scenePosition = pEvent->position;
}