#include <Precompiled.h>

#include "Camera.h"

void Camera::Init()
{
	cam_pos = glm::vec3(0.f, 0.f, 0.f);
	target_pos = glm::vec3(0.f, 0.f, -10000.f);

	direction_vec = glm::normalize(cam_pos - target_pos);
	right_vec = glm::vec3(0.f, 1.f, 0.f);
	right_vec = glm::normalize(glm::cross(right_vec, direction_vec));
	up_vec = glm::cross(direction_vec, glm::vec3(right_vec));

	cam_mat = glm::mat4(1.0f);
	cam_mat = glm::lookAt(cam_pos,
		target_pos,
		up_vec);

	persp_projection = glm::perspective(glm::radians(45.0f), 16.f / 9.f, 0.1f, 1000000.f);

}



/*
void Update(float dt)
{

}
*/