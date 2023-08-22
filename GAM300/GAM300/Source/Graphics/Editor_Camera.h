#pragma once

#include "glslshader.h"
#include "IOManager/Handler_GLFW.h"

class Editor_Camera
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
	
	void updateView();

	void setDistanceToFocalPoint(float distance);
	
	void rotateCamera(glm::vec2 delta);
	void panCamera(glm::vec2 delta);
	void zoomCamera();

	void setViewportSize(float width , float height);

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

private:

	glm::vec2 viewport{ 1600.f,900.f };

	glm::vec3 cam_pos{}; // Location of Camera

	glm::mat4 cam_mat;
	glm::mat4 persp_projection;
	glm::mat4 ortho_projection; // Don't think its needed

	glm::vec3 focalPoint{};

	float distanceToFP;

	float rotationSpeed;

	float spin = 0.f;
	float tilt = 0.f;

	bool lock = false;
	bool showcase = false;

	glm::vec2 prevMousePos;


};

