#include "Precompiled.h"

#include "Editor_Camera.h"

void Editor_Camera::Init()
{
	cam_pos = glm::vec3(0.f, 0.f, 0.f);
	//target_pos = glm::vec3(0.f, 0.f, -10000.f);

	//direction_vec = glm::normalize(cam_pos - target_pos);
	//right_vec = glm::vec3(0.f, 1.f, 0.f);
	//right_vec = glm::normalize(glm::cross(right_vec, direction_vec));
	//up_vec = glm::cross(direction_vec, glm::vec3(right_vec));
	//focalPoint = { -10.f,-10.f,-10.f };
	setDistanceToFocalPoint(1000.f);

	updateView();

	persp_projection = glm::perspective(glm::radians(45.0f), 16.f / 9.f, 0.1f, 1000000.f);

}


void Editor_Camera::Update(float dt)
//void Update()
{
	// To Move / Adjust the editor camera
	if (InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT_ALT))
	{

		//--------------------------------------------------------------
		// Rotating / Panning / Zooming
		//--------------------------------------------------------------

		glm::vec2 delta = (InputHandler::getMousePos() - prevMousePos) * 0.003f;
		prevMousePos = InputHandler::getMousePos();

		if (InputHandler::isMouseButtonPressed_L()) // Rotating
		{
			rotateCamera(delta);
		}
		else if (InputHandler::isMouseButtonPressed_R()) // Panning
		{
			panCamera(delta);
		}
		else if (InputHandler::getMouseScrollState() != 0) // Panning
		{
			zoomCamera();
		}

		//--------------------------------------------------------------
		// Moving the Editor Camera
		//--------------------------------------------------------------


		if (InputHandler::isKeyButtonHolding(GLFW_KEY_W))
		{
			focalPoint += getForwardVec() * 10.f;
		}
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_A))
		{
			focalPoint += -getRightVec() * 10.f;
		}
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_S))
		{
			focalPoint += -getForwardVec() * 10.f;
		}
		if (InputHandler::isKeyButtonHolding(GLFW_KEY_D))
		{
			focalPoint += getRightVec() * 10.f;
		}


		std::cout << "Cam : " << cam_pos.x << " , " << cam_pos.y << " , " << cam_pos.z << "\n";
		std::cout << "Focal Point : " << focalPoint.x << " , " << focalPoint.y << " , " << focalPoint.z << "\n";
		updateView();
	}
	


}

void Editor_Camera::updateView()
{
	cam_pos = GetCameraPosition();

	glm::quat Orientation = getOrientation();
	cam_mat = glm::translate(glm::mat4(1.0f), cam_pos) * glm::mat4(Orientation);
	cam_mat = glm::inverse(cam_mat);

}


glm::quat Editor_Camera::getOrientation()
{
	return glm::quat(glm::vec3(-spin, -tilt, 0.0f));

}

glm::vec3 Editor_Camera::getRightVec()
{
	return glm::vec3(glm::mat4(getOrientation())[0]);
}

glm::vec3 Editor_Camera::getUpVec()
{
	//return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	return glm::vec3(glm::mat4(getOrientation())[1]);

}

glm::vec3 Editor_Camera::getForwardVec()
{
	//return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	return -glm::vec3(glm::mat4(getOrientation())[2]);

}

void Editor_Camera::setDistanceToFocalPoint(float distance)
{
	distanceToFP = distance;
}

float Editor_Camera::getDistanceToFocalPoint()
{
	return distanceToFP;
}

glm::vec3 Editor_Camera::GetCameraPosition()
{
	return focalPoint - (getForwardVec() * getDistanceToFocalPoint());
}


glm::mat4 Editor_Camera::getViewMatrix()
{
	return cam_mat;
}


glm::mat4 Editor_Camera::getPerspMatrix()
{
	return persp_projection;
}

float Editor_Camera::getZoomSpeed()
{
	float distance = distanceToFP * 0.2f;
	distance = std::max(distance, 0.0f);
	float speed = distance * distance;
	speed = std::min(speed, 100.0f); // max speed = 100
	return speed;
}

void Editor_Camera::rotateCamera(glm::vec2 delta)
{
	std::cout << "Rotate Camera\n";
	tilt += delta.x * 0.5f;
	spin -= delta.y * 0.5f;
}
void Editor_Camera::panCamera(glm::vec2 delta)
{
	std::cout << "Panning\n";
	focalPoint += -getRightVec() * delta.x * 100.f;
	focalPoint += getUpVec() * delta.y * 100.f;
}
void Editor_Camera::zoomCamera()
{
	distanceToFP += -InputHandler::getMouseScrollState() * getZoomSpeed();
	if (distanceToFP < 1.f)
	{
		std::cout << "we hitting here ouchy\n";
		//focalPoint += getForwardVec();
		distanceToFP = 1.1f;
	}
}

