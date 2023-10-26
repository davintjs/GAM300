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
#include "IOManager/InputHandler.h"
#include "Editor/EditorHeaders.h"
#include "Core/EventsManager.h"
#include "Scene/SceneManager.h"

void EditorCamera::Init()
{
	BaseCamera::Init();

	farClip = 10000.f;
	focalPoint = { 0.f, 2.f, 3.f };
	pitch = 0.7f;

	timer = duration;

	EVENTS.Subscribe(this, &EditorCamera::CallbackPanCamera);
	EVENTS.Subscribe(this, &EditorCamera::CallbackUpdateSceneGeometry);
}

void EditorCamera::Update(float dt)
{
	intersected = FLT_MAX;
	tempIntersect = 0.f;

	// Focus on object using F key
	FocusOnObject(dt);

	EditorWindowEvent e("Scene");
	EVENTS.Publish(&e);

	if(e.isHovered || isFlying)	// Rotating and Flying
		InputControls();

	if (e.isHovered && InputHandler::getMouseScrollState() != 0) // Zooming
		ZoomCamera();

	BaseCamera::Update();
}

void EditorCamera::InputControls()
{
	glm::vec2 mousePos = InputHandler::getMousePos();
	mousePos.y = Application::GetHeight() - mousePos.y;
	glm::vec2 delta = (mousePos - prevMousePos) * 0.003f;
	prevMousePos = mousePos;

	isMoving = true;

	// To Move and rotate the editor camera
	if (InputHandler::isMouseButtonHolding_R())
	{
		isFlying = true;
		isFocusing = false;

		float speedModifer = 0.1f;
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT_SHIFT))
			speedModifer = 1.f;

		//--------------------------------------------------------------
		// Rotating / Panning / Zooming
		//--------------------------------------------------------------

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
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_Q))
		{
			focalPoint.y += speedModifer;
		}
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_E))
		{
			focalPoint.y -= speedModifer;
		}
	}
	else if (!isFlying && InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT_ALT))
	{
		if (InputHandler::isMouseButtonHolding_L())
			OrbitCamera(delta);

		isFocusing = false;
	}
	else if ((isPanning && InputHandler::isMouseButtonHolding_L()) || InputHandler::isMouseButtonHolding_M())
	{
		PanCamera(delta);
		isFocusing = false;
	}
	else
	{
		isFlying = false;
		isMoving = false;
	}
}

void EditorCamera::FocusOnObject(float dt)
{
	if (InputHandler::isKeyButtonPressed(GLFW_KEY_F))
	{
		GetSelectedEntityEvent e;
		EVENTS.Publish(&e);
		if (e.pEntity)
		{
			Transform& t = MySceneManager.GetCurrentScene().Get<Transform>(*e.pEntity);
			targetFP = t.GetTranslation();
			glm::vec3 scale = t.GetScale();
			targetFL = std::max(1.f, 10.f * cbrt((scale.x * scale.y * scale.z) * 0.5f));
			initialFP = focalPoint;
			initialFL = focalLength;
			timer = 0.f;
			isFocusing = true;
		}
	}

	if (timer < duration && isFocusing)
	{
		focalPoint = Interpolate(initialFP, targetFP, timer, duration, EASINGTYPE::BEZIER);
		focalLength = Interpolate(initialFL, targetFL, timer, duration, EASINGTYPE::BEZIER);
		timer += dt;
	}
}

glm::vec2 EditorCamera::GetMouseInNDC()
{
	glm::vec2 mousePosition = InputHandler::getMousePos();
	mousePosition.y -= Application::GetHeight() - scenePosition.y - dimension.y;
	glm::vec2 mouseScenePosition = { mousePosition.x - scenePosition.x, mousePosition.y};
	glm::vec2 mouseToNDC = mouseScenePosition / dimension;
	glm::vec2 mouseTo1600By900 = mouseToNDC * glm::vec2(Application::GetWidth(), Application::GetHeight());
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
	pitch += _delta.y * GetRotationSpeed();
	yaw += _delta.x * GetRotationSpeed();
}

void EditorCamera::PanCamera(const glm::vec2& _delta)
{
	//std::cout << "Panning\n";
	glm::vec2 panSpeed = GetPanSpeed();
	focalPoint += -GetRightVec() * _delta.x * panSpeed.x * GetFocalLength();
	focalPoint += GetUpVec() * _delta.y * panSpeed.y * GetFocalLength();
}

void EditorCamera::ZoomCamera()
{
	focalLength -= InputHandler::getMouseScrollState() * GetZoomSpeed();
	if (focalLength < 0.1f)
	{
		focalPoint += GetForwardVec();
		focalLength = 0.15f;
	}
}

float EditorCamera::GetZoomSpeed()
{
	float distance = focalLength * 0.2f;
	distance = std::max(distance, 0.f);
	float speed = distance * distance;
	speed = std::min(speed, 100.0f); // max speed = 100
	return speed * speedModifier * 0.1f;
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

	float x = (2.0f * (float)mousePos.x) / Application::GetWidth() - 1.0f;
	float y = (2.0f * (float)mousePos.y) / Application::GetHeight() - 1.0f;

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
	
	float x = (2.0f * (float)xpos) / Application::GetWidth() - 1.0f;
	float y = (2.0f * (float)ypos) / Application::GetHeight() - 1.0f;

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