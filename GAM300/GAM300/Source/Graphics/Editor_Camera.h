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

	glm::quat getOrientation();

	glm::vec3 getRightVec();
	glm::vec3 getUpVec();
	glm::vec3 getForwardVec();

	/*private:*/
	glm::vec3 cam_pos{}; // Location of Camera

	glm::mat4 cam_mat;
	glm::mat4 persp_projection;
	glm::mat4 ortho_projection; // Don't think its needed

	glm::vec2 locked_coord{};
	float mov_per_sec = 10000.f;
	float mov_speed;
	
	float spin = 0.f;
	float tilt = 0.f;

	bool lock = false;
	bool showcase = false;

	glm::vec2 prevMousePos;


};

