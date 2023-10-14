/*!***************************************************************************************
\file			Editor_Camera.cpp
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

#include "Editor_Camera.h"
#include "Editor/EditorHeaders.h"
#include "Core/EventsManager.h"

Ray3D::Ray3D() : origin{}, direction{} {}

Ray3D::Ray3D(const glm::vec3& _origin, const glm::vec3& _direction) : origin{ _origin }, direction{ _direction } {}

bool Ray3D::TestRayOBB(const glm::mat4& _modelMatrix, const glm::vec3& _min, const glm::vec3& _max, float& _intDistance)
{
	// Intersection method from Real-Time Rendering and Essential Mathematics for Games

	direction = glm::normalize(direction);

	float tMin = 0.0f;
	float tMax = 100000.0f;

	glm::vec3 oBBPositionWorldSpace(_modelMatrix[3].x, _modelMatrix[3].y, _modelMatrix[3].z);

	glm::vec3 delta = oBBPositionWorldSpace - origin;

	PlaneParams plane{ delta, _min, _max, tMin, tMax };

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	if (!IBPlanes(_modelMatrix, plane, 0))
		return false;
	
	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	if (!IBPlanes(_modelMatrix, plane, 1))
		return false;

	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	if (!IBPlanes(_modelMatrix, plane, 2))
		return false;

	_intDistance = tMin;
	return true;
}

bool Ray3D::IBPlanes(const glm::mat4& _modelMatrix, PlaneParams& _plane, const int& _i)
{
	glm::vec3 zaxis(_modelMatrix[_i].x, _modelMatrix[_i].y, _modelMatrix[_i].z);
	float e = glm::dot(zaxis, _plane.delta);
	float f = glm::dot(direction, zaxis);

	if (fabs(f) > 0.001f) {

		float t1 = (e + _plane.min[_i]) / f; // Intersection with the "left" plane
		float t2 = (e + _plane.max[_i]) / f; // Intersection with the "right" plane
		// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

		// We want t1 to represent the nearest intersection, 
		// so if it's not the case, invert t1 and t2
		if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

		if (t2 < _plane.tMax) // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			_plane.tMax = t2;
		if (t1 > _plane.tMin) // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			_plane.tMin = t1;

		// And here's the trick :
		// If "far" is closer than "near", then there is NO intersection.
		// See the images in the tutorials for the visual explanation.
		if (_plane.tMin > _plane.tMax)
			return false;

	}
	else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
		if (-e + _plane.min[_i] > 0.0f || -e + _plane.max[_i] < 0.0f)
			return false;
	}

	return true;
}

void Camera::Init()
{
	aspect = 16.f / 9.f;
	fieldOfView = 45.0f;
	nearClip = 0.1f;
	farClip = 100000.f;

	UpdateViewMatrix();
	UpdateProjection();

	framebuffer.set_size((unsigned int) 1600, (unsigned int) 900);
	framebuffer.init();
}

void Camera::UpdateViewMatrix()
{
	cameraPosition = GetCameraPosition();

	glm::quat Orientation = GetOrientation();
	viewMatrix = glm::translate(glm::mat4(1.0f), cameraPosition) * glm::mat4(Orientation);
	viewMatrix = glm::inverse(viewMatrix);
}

void Camera::UpdateProjection()
{
	projMatrix = glm::perspective(glm::radians(fieldOfView), aspect, nearClip, farClip);
}

void Camera::UpdateFrustum()
{

}

bool Camera::WithinFrustum()
{
	return false;
}

glm::vec3 Camera::GetCameraPosition() 
{ 
	return focalPoint - (GetForwardVec() * GetFocalLength()); 
}

glm::vec3 Camera::GetFocalPoint()
{
	return cameraPosition + (GetForwardVec() * GetFocalLength());
}

void EditorCamera::Init()
{
	Camera::Init();

	SetFocalLength(1000.f);

	UpdateViewMatrix();

	EVENTS.Subscribe(this, &EditorCamera::CallbackPanCamera);
	EVENTS.Subscribe(this, &EditorCamera::CallbackUpdateSceneGeometry);
}

void EditorCamera::Update(float dt)
{
	intersected = FLT_MAX;
	tempIntersect = 0.f;

	EditorWindowEvent e;
	EVENTS.Publish(&e);

	if(e.isHovered || isMoving)	// Rotating and Flying
		InputControls();

	if (e.isHovered && InputHandler::getMouseScrollState() != 0) // Zooming
		ZoomCamera();

	UpdateViewMatrix();

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

// Bean: Temporary resize needed for resizing the scene viewport
void EditorCamera::OnResize(const float& _width, const float& _height)
{
	dimension.x = _width;
	dimension.y = _height;
	aspect = dimension.x / dimension.y;

	UpdateProjection();
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