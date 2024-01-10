/*!***************************************************************************************
\file			BaseCamera.h
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           15/10/2023

\brief
    This file contains the declarations of the following:
    1. The base camera used in editor and the game

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef BASECAMERA_H
#define BASECAMERA_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Framebuffer.h"
#include "Properties.h"

struct Transform;

enum CAMERATYPE
{
	SCENE,
	GAME,
	PREVIEW
};

struct FrustumPlane
{
	FrustumPlane() = default;
	FrustumPlane(const glm::vec3& _position, const glm::vec3& _normal);

	bool IsOnOrForwardPlane(const glm::vec3& _position, const glm::vec3 _extents) const;

	float GetSignedDistanceToPlane(const glm::vec3& _point) const;

	glm::vec3 normal = { 0.f, 1.f, 0.f };
	float distance = 0.f;
};

struct Frustum
{
	FrustumPlane topFace;
	FrustumPlane bottomFace;

	FrustumPlane rightFace;
	FrustumPlane leftFace;

	FrustumPlane farFace;
	FrustumPlane nearFace;

	bool CheckIfWithinFrustum(const glm::vec3& _position, const glm::vec3 _extents) const;
};

struct BaseCamera : property::base
{
public:

	// Initialize the base camera which sets the default properties of the camera and creates a framebuffer
	void Init();
	
	// Initialize the base camera with the parameters as the properties of the camera and creates a framebuffer
	void Init(const glm::vec2& _dimension, const float& _fov, const float& _nearClip, const float& _farClip, const float& _lookatDistance);

	// Update the camera
	void Update();
	
	// Update the view matrix of the camera
	void UpdateViewMatrix();

	// Update the projection matrix of the camera
	void UpdateProjection();

	// Update the camera's frustrum
	void UpdateFrustum();

	// Update the camera's position and rotation
	void UpdateCamera(const glm::vec3& _position, const glm::vec3& _rotation);

	// Attemp to resize the camera's dimension
	void TryResize(glm::vec2 _newDimension);

	// Adjust Prespective projection based off viewport
	void OnResize(const float& _width, const float& _height);

	// Check if items are within the camera's frustum
	bool WithinFrustum(Transform& _transform, const glm::vec3& _min, const glm::vec3& _max);
	
	// Getting Things relative to Cameras 
	void SetCameraRotation(const glm::vec3& _rotation);

	void SetCameraPosition(const glm::vec3& _position);
	glm::vec3 GetCameraPosition();

	void SetFocalPoint(const glm::vec3& _position);
	glm::vec3 GetFocalPoint();
	
	glm::vec2& GetViewportSize() { return dimension; }
	void SetViewportSize(const float& _width, const float& _height) { dimension = glm::vec2(_width, _height); }
	void SetViewportSize(const glm::vec2& _dimension) { dimension = _dimension; }
	Frustum& GetFrustum() { return frustum; }

	CAMERATYPE GetCameraType() const { return cameraType; }
	unsigned int& GetTargetDisplay() { return targetDisplay; }

	float& GetPitch() { return pitch; }
	float& GetYaw() { return yaw; }

	glm::quat GetOrientation() { return glm::quat(glm::vec3(-pitch, -yaw, -roll)); }
	glm::vec3 GetRightVec() { return glm::vec3(glm::mat4(GetOrientation())[0]); }
	glm::vec3 GetUpVec() { return glm::vec3(glm::mat4(GetOrientation())[1]); }
	glm::vec3 GetForwardVec() { return -glm::vec3(glm::mat4(GetOrientation())[2]); }

	float& GetAspect() { return aspect; }
	float& GetNearClip() { return nearClip; }
	float& GetFarClip() { return farClip; }
	float& GetFOV() { return fieldOfView; }
	float& GetDistance() { return lookatDistance; }
	void SetDistance(const float& _distance) { lookatDistance = _distance; }

	glm::mat4& GetProjMatrix() { return projMatrix; }
	glm::mat4& GetViewMatrix() { return viewMatrix; }

	unsigned int& GetFramebufferID() { return framebufferID; }
	unsigned int& GetAttachment() { return colorAttachment; }
	unsigned int& GetHDRAttachment() { return hdrColorAttachment; }
	unsigned int& GetBloomAttachment() { return bloomAttachment; }

	property_vtable();
protected:
	glm::vec4 backgroundColor;			// Default solid color when rendering
	glm::vec3 cameraPosition{};			// The location of the viewer / eye (Center of the screen, 10 units away)
	glm::vec3 focalPoint{};				// The look-at point / target point where the viewer is looking (Center of screen)
	glm::vec2 dimension;				// The dimension of the camera in width and height defined in pixels

	CAMERATYPE cameraType;				// Type of camera

	unsigned int targetDisplay = 0;		// Target display for the camera

	float width = 0.f;					// Pixel width of the camera
	float height = 0.f;					// Pixel height of the camera
	float pitch = 0.f;					// For rotating about the x axis
	float yaw = 0.f;					// For rotating about the y axis
	float roll = 0.f;					// For rotating about the z axis
	float distanceCheck = 5.f;			// The margin in which the camera will still render objects that are outside the camera's dimension

	float aspect = 0.f;					// The aspect ratio of the camera in width/height (Automatically calculated by screen's aspect ratio)
	float nearClip = 0.f;				// Distance of near clipping plane from the camera
	float farClip = 0.f;				// Distance of far clipping plane from the camera
	float fieldOfView = 0.f;			// The vertical field of view in degrees
	float lookatDistance = 0.f;			// THe distance from the camera to the focal point

	Frustum frustum;					// The frustum of the camera

	char clearFlags = 1;				// The background to render for the camera
	char cullingMask = 0;				// A culling mask to prevent rendering of specific layers, to be implemented in the future
	bool orthographic = false;			// Perspective by default until orthographic camera has been implemented
	bool useOcclusionCulling = false;	// Bean: A feature to be implemented in the future
	bool useFrustumCulling = true;		// Frustum culling for the camera, on by default
	bool enableHDR = true;				// High dynamic range rendering

	glm::mat4 projMatrix{ 0 };			// The projection matrix to use, either orthographic or perspective
	glm::mat4 viewMatrix{ 0 };			// The view matrix -> worldToCamera matrix

	unsigned int framebufferID;			// The framebuffer used for this camera
	unsigned int colorAttachment;		// The color attachment used for this camera
	unsigned int hdrColorAttachment;	// The hdr color attachment used for this camera
	unsigned int bloomAttachment;		// The bloom attachment used for this camera
};

property_begin_name(BaseCamera, "BaseCamera") {
	property_var(clearFlags).Name("ClearFlags"),
	property_var(cullingMask).Name("CullingMask"),
	property_var(lookatDistance).Name("LookAtDistance"),
	property_var(nearClip).Name("NearClip"),
	property_var(farClip).Name("FarClip"),
	property_var(fieldOfView).Name("FieldOfView"),
	property_var(orthographic).Name("Orthographic"),
	property_var(targetDisplay).Name("TargetDisplay"),
	property_var(enableHDR).Name("HDR"),
	property_var(useOcclusionCulling).Name("OcclusionCulling"),
	property_var(useFrustumCulling).Name("FrustumCulling"),
	property_var(distanceCheck).Name("DistanceCulling"),

}property_vend_h(BaseCamera)

#endif // !BASECAMERA_H