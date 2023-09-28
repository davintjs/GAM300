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

#pragma once

#include "glslshader.h"
#include "IOManager/Handler_GLFW.h"
#include "../Core/SystemInterface.h"
#include "Framebuffer.h"
#include "Scene/Entity.h"

#define EditorCam Editor_Camera::Instance()

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

SINGLETON(Editor_Camera)
{
public:
	/*!*****************************************************************************
	\author
		Euan Lim
	\brief
		Initalize camera
	\return
		void
	*******************************************************************************/
	void Init();
	/*!*****************************************************************************
	\author
		Euan Lim
	\brief
		Update Loop
	param [in] dt
		delta time
	\return
		void
	*******************************************************************************/
	void Update(float dt);
	//void Update();
	
	// update cam_mat
	void updateView();

	// Adjust Prespective projection based off viewport
	void onResize(float _width, float _height);

	// Get mouse coord in NDC
	glm::vec2 GetMouseInNDC();

	// set distanceto distanceToFP
	void setDistanceToFocalPoint(float distance);
	
	// rotate camera
	void rotateCamera(glm::vec2 delta);
	
	// pan camera
	void panCamera(glm::vec2 delta);
	
	// zoom camera
	void zoomCamera();

	// Change viewport size
	void setViewportSize(float width , float height);

	// Set rotation speed
	void setRotationSpeed(float speed);

	// Getting Things relative to Cameras
	float getDistanceToFocalPoint();

	glm::vec3 GetCameraPosition();
	
	glm::quat getOrientation();
	glm::vec3 getRightVec();
	glm::vec3 getUpVec();
	glm::vec3 getForwardVec();

	glm::mat4 getViewMatrix();
	glm::mat4 getPerspMatrix();

	float getZoomSpeed();
	glm::vec2 getPanSpeed();

	glm::vec2 getViewportSize();

	float getRotationSpeed();

	Framebuffer& getFramebuffer() { return framebuffer; }

	// Shoots a Ray from camera
	Ray3D Raycasting(double xpos, double ypos, glm::mat4 proj, glm::mat4 view, glm::vec3 eye);


	bool canMove = true;
	bool isMoving = false;
private:

	glm::vec2 viewport{ 1600.f,900.f };

	glm::vec3 cam_pos{}; // Location of Camera

	glm::mat4 cam_mat;
	glm::mat4 persp_projection;
	glm::mat4 ortho_projection; // Don't think its needed

	glm::vec3 focalPoint{};

	float distanceToFP;
	float aspect;
	float rotationSpeed;

	float spin = 0.f;
	float tilt = 0.f;

	bool lock = false;
	bool showcase = false;

	glm::vec2 prevMousePos;

	Framebuffer framebuffer;
};

