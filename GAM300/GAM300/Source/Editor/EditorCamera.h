/*!***************************************************************************************
\file			EditorCamera.h
\project
\author         Euan Lim

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declaration of the editor camera and it's related functionalities
	that includes:
	1. Ray3D Class for ray casting ( Mouse Picking) and collision with OBB
	2. Editor Camera and it's functionalities

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include "Graphics/Ray3D.h"
#include "Graphics/BaseCamera.h"
#include "../Core/SystemInterface.h"
#include "Scene/Entity.h"
#include "Core/Events.h"

#define EditorCam EditorCamera::Instance()

SINGLETON(EditorCamera), public BaseCamera
{
public:
	
	// Initialize the basic editor camera parameters
	void Init();
	
	// Update the editor camera, input controls
	void Update(float dt);

	// Mouse and Keyboard controls for the camera, zoom is separated in Update function
	void InputControls();

	// Move to focused object
	void FocusOnObject(float dt);

	// Get mouse coord in NDC
	glm::vec2 GetMouseInNDC();
	
	// Rotate camera while in FlyMode
	void RotateCamera(const glm::vec2& _delta);

	// Orbit camera around focal point
	void OrbitCamera(const glm::vec2& _delta);
	
	// pan camera
	void PanCamera(const glm::vec2& _delta);
	
	// zoom camera
	void ZoomCamera();

	float GetZoomSpeed();

	glm::vec2 GetPanSpeed();

	// Shoots a Ray from camera
	Ray3D Raycasting();
	Ray3D Raycasting(double xpos, double ypos, glm::mat4 proj, glm::mat4 view, glm::vec3 eye);

	Ray3D& GetRay() { return ray; }

	// Getter and setter for rotation speed
	float GetRotationSpeed() { return rotationSpeed * speedModifier; }
	void SetRotationSpeed(const float& _speed) { rotationSpeed = _speed; }

	float& GetIntersect() { return intersected; }
	float& GetTempIntersect() { return tempIntersect; }

	bool IsPanning() const { return isPanning; }
	bool IsFlying() const { return isFlying; }

	void CallbackPanCamera(EditorPanCameraEvent* pEvent);

	void CallbackUpdateSceneGeometry(EditorUpdateSceneGeometryEvent* pEvent);

private:
	Ray3D ray;
	glm::vec3 targetFP;
	glm::vec3 initialFP;
	glm::vec2 prevMousePos;
	glm::vec2 scenePosition;
	float rotationSpeed = 1.f;			// How fast the camera rotates
	float speedModifier = 1.f;			// How fast all the cameras movements are
	float intersected = 0.f;			// Current ray intersect
	float tempIntersect = 0.f;	
	float initialFL = 0.f;				// Initial Focal Point when focusing on object
	float targetFL = 0.f;				// Target Focal Length when focusing on object
	float timer = 0.f;
	const float duration = 1.f;
	bool isPanning = false;
	bool isFlying = false;				// Hold down right click for fly mode
	bool isFocusing = false;			// Moving to focused object
};

#endif // !EDITOR_CAMERA_H