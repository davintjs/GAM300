#include "Precompiled.h"

#include "Editor_Camera.h"

void Editor_Camera::Init()
{
	cam_pos = glm::vec3(0.f, 0.f, 0.f);
	target_pos = glm::vec3(0.f, 0.f, -10000.f);

	direction_vec = glm::normalize(cam_pos - target_pos);
	right_vec = glm::vec3(0.f, 1.f, 0.f);
	right_vec = glm::normalize(glm::cross(right_vec, direction_vec));
	up_vec = glm::cross(direction_vec, glm::vec3(right_vec));

	updateView();

	persp_projection = glm::perspective(glm::radians(45.0f), 16.f / 9.f, 0.1f, 1000000.f);

}


void Editor_Camera::Update(float dt)
//void Update()
{

	if (InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT_ALT))
	{

		glm::vec2 delta = (InputHandler::getMousePos() - prevMousePos) * 0.003f;
		prevMousePos = InputHandler::getMousePos();

		//std::cout << "yes\n";
		if (InputHandler::isMouseButtonPressed_L())
		{
			//std::cout << "yup\n";
			tilt += delta.x * 0.5f;
			spin += delta.y * 0.5f;

		}
		updateView();
	}
	


}

void Editor_Camera::updateView()
{
	glm::quat Orientation = glm::quat(glm::vec3(-spin, -tilt, 0.0f));

	cam_mat = glm::translate(glm::mat4(1.0f), cam_pos) * glm::mat4(Orientation);
	cam_mat = glm::inverse(cam_mat);

}