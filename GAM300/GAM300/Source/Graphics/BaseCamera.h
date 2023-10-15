/*!***************************************************************************************
\file			BaseCamera.h
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           15/10/2023

\brief
    This file contains the declarations of the following:
    1.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef BASECAMERA_H
#define BASECAMERA_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Framebuffer.h"
#include "Properties.h"

enum CAMERATYPE
{
	SCENE,
	GAME,
	PREVIEW
};

struct BaseCamera : property::base
{
public:

	void Init();

	void Update();

	void UpdateViewMatrix();

	void UpdateProjection();

	void UpdateFrustum();

	void UpdateCamera(const glm::vec3& _position, const glm::vec3& _rotation);

	// Adjust Prespective projection based off viewport
	void OnResize(const float& _width, const float& _height);

	bool WithinFrustum() const;

	void SetCameraRotation(const glm::vec3& _rotation);

	void SetCameraPosition(const glm::vec3& _position);
	glm::vec3 GetCameraPosition();

	glm::vec3 GetFocalPoint();

	// Getting Things relative to Cameras
	glm::vec2& GetViewportSize() { return dimension; }
	void SetViewportSize(const float& _width, const float& _height) { dimension = glm::vec2(_width, _height); }
	void SetViewportSize(const glm::vec2& _dimension) { dimension = _dimension; }

	unsigned int& GetTargetDisplay() { return targetDisplay; }

	glm::quat GetOrientation() { return glm::quat(glm::vec3(-yaw, -pitch, 0.0f)); }
	glm::vec3 GetRightVec() { return glm::vec3(glm::mat4(GetOrientation())[0]); }
	glm::vec3 GetUpVec() { return glm::vec3(glm::mat4(GetOrientation())[1]); }
	glm::vec3 GetForwardVec() { return -glm::vec3(glm::mat4(GetOrientation())[2]); }

	float& GetFocalLength() { return focalLength; }
	void SetFocalLength(const float& _length) { focalLength = _length; }

	glm::mat4& GetProjMatrix() { return projMatrix; }
	glm::mat4& GetViewMatrix() { return viewMatrix; }

	Framebuffer& GetFramebuffer() { return framebuffer; }

	property_vtable();
protected:
	glm::vec4 backgroundColor;			// Default solid color when rendering
	glm::vec3 cameraPosition;			// The location of the viewer / eye (Center of the screen, 10 units away)
	glm::vec3 focalPoint;				// The look-at point / target point where the viewer is looking (Center of screen)
	glm::vec2 dimension;				// The dimension of the camera in width and height defined in pixels
	glm::vec2 frustumBottom;			// The bottom left position of the frustum, for frustum culling
	glm::vec2 frustumTop;				// The top right position of the frustum, for frustum culling

	CAMERATYPE cameraType;				// Type of camera

	unsigned int targetDisplay = 0;		// Target display for the camera

	float width = 0.f;					// Pixel width of the camera
	float height = 0.f;					// Pixel height of the camera
	float pitch = 0.f;					// For rotating up and down
	float yaw = 0.f;					// For rotating left and right
	float frustumMargin = 5.f;			// The margin in which the camera will still render objects that are outside the camera's dimension

	float aspect = 0.f;					// The aspect ratio of the camera in width/height (Automatically calculated by screen's aspect ratio)
	float nearClip = 0.f;				// Distance of near clipping plane from the camera
	float farClip = 0.f;				// Distance of far clipping plane from the camera
	float fieldOfView = 0.f;			// The vertical field of view in degrees
	float focalLength = 0.f;			// How close is the camera to the focal point

	char clearFlags = 1;				// The background to render for the camera
	char cullingMask = 0;				// A culling mask to prevent rendering of specific layers, to be implemented in the future
	bool orthographic = false;			// Perspective by default until orthographic camera has been implemented
	bool useOcclusionCulling = false;	// Bean: A feature to be implemented in the future
	bool useFrustumCulling = true;		// Frustum culling for the camera, on by default
	bool enableHDR = true;				// High dynamic range rendering

	glm::mat4 projMatrix{ 0 };			// The projection matrix to use, either orthographic or perspective
	glm::mat4 viewMatrix{ 0 };			// The view matrix -> worldToCamera matrix

	Framebuffer framebuffer;
};

property_begin_name(BaseCamera, "BaseCamera") {
	property_var(clearFlags).Name("ClearFlags"),
	property_var(cullingMask).Name("CullingMask"),
	property_var(focalLength).Name("FocalLength"),
	property_var(nearClip).Name("NearClip"),
	property_var(farClip).Name("FarClip"),
	property_var(fieldOfView).Name("FieldOfView"),
	property_var(orthographic).Name("Orthographic"),
	property_var(targetDisplay).Name("TargetDisplay"),
	property_var(enableHDR).Name("HDR"),
	property_var(useOcclusionCulling).Name("OcclusionCulling"),
	property_var(useFrustumCulling).Name("FrustumCulling"),

}property_vend_h(BaseCamera)

#endif // !BASECAMERA_H