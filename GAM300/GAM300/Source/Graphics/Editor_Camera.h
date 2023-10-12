/*!***************************************************************************************
\file			Editor_Camera.h
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

#include "glslshader.h"
#include "IOManager/Handler_GLFW.h"
#include "../Core/SystemInterface.h"
#include "Framebuffer.h"
#include "Scene/Entity.h"

#define EditorCam EditorCamera::Instance()

class Ray3D
{
public:
	// Constructor
	Ray3D() : origin(), direction() { return; }
	Ray3D(const glm::vec3& o, const glm::vec3& d)
		: origin(o), direction(d) {
		return;
	}
	glm::vec3 lerp(const float t) const { return(origin + t * direction); }


	// Containment method
	bool contains(const glm::vec3& point, float* t = NULL) const;
	// Returns paramter of intersection if containment is true and t != NULL

	glm::vec3 origin;
	glm::vec3 direction;
};

// Ray to Obb Test
bool testRayOBB(glm::vec3 ray_origin,        // Ray origin, in world space
	glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
	glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
	glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
	glm::mat4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
	float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB)
);

enum CAMERATYPE
{
	SCENE,
	GAME,
	PREVIEW
};

struct Camera
{
public:
	 
	void Init();

	void UpdateViewMatrix();

	void UpdateProjection();

	void UpdateFrustum();

	bool WithinFrustum();

	glm::vec3 GetCameraPosition();

	glm::vec3 GetFocalPoint();

	// Getting Things relative to Cameras
	float& GetFocalLength() { return focalLength; }
	void SetFocalLength(const float& _length) { focalLength = _length; }

	glm::quat GetOrientation() { return glm::quat(glm::vec3(-yaw, -pitch, 0.0f)); }
	glm::vec3 GetRightVec() { return glm::vec3(glm::mat4(GetOrientation())[0]); }
	glm::vec3 GetUpVec() { return glm::vec3(glm::mat4(GetOrientation())[1]); }
	glm::vec3 GetForwardVec() { return -glm::vec3(glm::mat4(GetOrientation())[2]); }

	glm::mat4& GetProjMatrix() { return projMatrix; }
	glm::mat4& GetViewMatrix() { return viewMatrix; }

	glm::vec2& GetViewportSize() { return dimension; }
	void SetViewportSize(const float& _width, const float& _height) { dimension = glm::vec2(_width, _height); }
	void SetViewportSize(const glm::vec2& _dimension) { dimension = _dimension; }

	Framebuffer& GetFramebuffer() { return framebuffer; }

protected:
	glm::vec4 backgroundColor;			// Default solid color when rendering
	glm::vec3 cameraPosition;			// The location of the viewer / eye (Center of the screen, 10 units away)
	glm::vec3 focalPoint;				// The look-at point / target point where the viewer is looking (Center of screen)
	glm::vec2 dimension;				// The dimension of the camera in width and height defined in pixels
	glm::vec2 frustumBottom;			// The bottom left position of the frustum, for frustum culling
	glm::vec2 frustumTop;				// The top right position of the frustum, for frustum culling

	CAMERATYPE cameraType;				// Type of camera

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
	
	char cullingMask = 0;				// A culling mask to prevent rendering of specific layers, to be implemented in the future
	bool orthographic = true;			// Orthographic by default until perspective camera has been implemented
	bool useOcclusionCulling = false;	// Bean: A feature to be implemented in the future
	bool useFrustumCulling = true;		// Frustum culling for the camera, on by default
	bool enableHDR = true;				// High dynamic range rendering

	glm::mat4 projMatrix{ 0 };			// The projection matrix to use, either orthographic or perspective
	glm::mat4 viewMatrix{ 0 };			// The view matrix -> worldToCamera matrix

	Framebuffer framebuffer;
};

SINGLETON(EditorCamera), public Camera
{
public:
	
	// Initialize the basic editor camera parameters
	void Init();
	
	// Update the editor camera, input controls
	void Update(float dt);

	// Mouse and Keyboard controls for the camera, zoom is separated in Update function
	void InputControls();

	// Adjust Prespective projection based off viewport
	void OnResize(const float& _width, const float& _height);

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
	Ray3D Raycasting(double xpos, double ypos, glm::mat4 proj, glm::mat4 view, glm::vec3 eye);

	// Getter and setter for rotation speed
	float GetRotationSpeed() { return rotationSpeed * speedModifier; }
	void SetRotationSpeed(const float& _speed) { rotationSpeed = _speed; }

	bool canMove = true;
	bool isMoving = false;
private:
	float rotationSpeed = 0.f;			// How fast the camera rotates
	float speedModifier = 1.f;			// How fast all the cameras movements are
	glm::vec2 prevMousePos;
};

#endif // !EDITOR_CAMERA_H