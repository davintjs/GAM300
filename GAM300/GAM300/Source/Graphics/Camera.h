#pragma once

#include "glslshader.h"
#include "IOManager/Handler_GLFW.h"

class Camera
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
	
	/*private:*/
	glm::vec3 cam_pos{}; // Location of Camera
	glm::vec3 target_pos{}; //  Target ( location camera is looking at )
	glm::vec3 direction_vec{};// cam - target & normalized ( its pointing towards me )
	glm::vec3 right_vec{}; // right vector
	glm::vec3 up_vec{}; // upwards vector

	glm::mat4 cam_mat;
	glm::mat4 persp_projection;
	glm::mat4 ortho_projection;

	glm::vec2 locked_coord{};
	float mov_per_sec = 10000.f;
	float mov_speed;
	bool lock = false;
	bool showcase = false;
};