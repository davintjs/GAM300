#include "Precompiled.h"

#include "Editor/EditorHeaders.h"
#include "Editor_Camera.h"

extern std::vector<Ray3D> Ray_Container;


void Editor_Camera::Init()
{
	setDistanceToFocalPoint(1000.f);
	setRotationSpeed(2.f);
	updateView();

	aspect = 16.f / 9.f;
	persp_projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000000.f);

	framebuffer.set_size((unsigned int) 1600, (unsigned int) 900);
	framebuffer.init();
	//bool pass = false;
	//E_ASSERT(pass, "Ccoord" , 15 ,"hehe");
}


void Editor_Camera::Update(float dt)
//void Update()
{
	// To Move / Adjust the editor camera
	if (InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT_ALT))
	{
		isMoving = true;
		//--------------------------------------------------------------
		// Rotating / Panning / Zooming
		//--------------------------------------------------------------

		glm::vec2 delta = (InputHandler::getMousePos() - prevMousePos) * 0.003f;

		prevMousePos = InputHandler::getMousePos();

		if (InputHandler::isMouseButtonHolding_L()) // Rotating
		{
			if (canMove)
				rotateCamera(delta);
		}
		else if (InputHandler::isMouseButtonHolding_R()) // Panning
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

		//std::cout << "Cam : " << cam_pos.x << " , " << cam_pos.y << " , " << cam_pos.z << "\n";
		//std::cout << "Focal Point : " << focalPoint.x << " , " << focalPoint.y << " , " << focalPoint.z << "\n";

		updateView();
	}
	else
		isMoving = false;

	if (InputHandler::isKeyButtonHolding(GLFW_KEY_LEFT_CONTROL))
	{
		if (InputHandler::isMouseButtonPressed_L())
		{
			Ray3D temp = Raycasting(GetMouseInNDC().x, GetMouseInNDC().y, getPerspMatrix(), getViewMatrix(), GetCameraPosition());
			
			// No Editor Version
			//Ray3D temp = Raycasting(InputHandler::getMouseX(), InputHandler::getMouseY(), getPerspMatrix(), getViewMatrix(), GetCameraPosition());
				
			Ray_Container.push_back(temp);


		}
	}

	

	if (InputHandler::isMouseButtonHolding_L())
	{
		glm::vec2 position = GetMouseInNDC();
		//EditorDebugger::Instance().AddLog("Position: %f %f\n", position.x, position.y);
	}


	canMove = true;// The false check happens in editorscene, incase guizmo is being used
}

void Editor_Camera::updateView()
{
	cam_pos = GetCameraPosition();

	glm::quat Orientation = getOrientation();
	cam_mat = glm::translate(glm::mat4(1.0f), cam_pos) * glm::mat4(Orientation);
	cam_mat = glm::inverse(cam_mat);

}

// Bean: Temporary resize needed for resizing the scene viewport
void Editor_Camera::onResize(float _width, float _height)
{
	viewport.x = _width;
	viewport.y = _height;
	aspect = viewport.x / viewport.y;

	persp_projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000000.f);
}

glm::vec2 Editor_Camera::GetMouseInNDC()
{
	glm::vec2 scenePosition = EditorScene::Instance().GetPosition();
	glm::vec2 sceneDimension = EditorScene::Instance().GetDimension();
	glm::vec2 mousePosition = InputHandler::getMousePos();
	mousePosition.y -= GLFW_Handler::height - scenePosition.y - sceneDimension.y;
	glm::vec2 mouseScenePosition = { mousePosition.x - scenePosition.x, mousePosition.y + 22.f };
	glm::vec2 mouseToNDC = mouseScenePosition / sceneDimension;

	glm::vec2 mouseTo1600By900 = mouseToNDC * glm::vec2(GLFW_Handler::width, GLFW_Handler::height);
	//mouseTo1600By900.y = GLFW_Handler::height - mouseTo1600By900.y;
	return mouseTo1600By900;
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
	//std::cout << "Rotate Camera\n";
	tilt += delta.x * getRotationSpeed();
	spin -= delta.y * getRotationSpeed();
}

void Editor_Camera::panCamera(glm::vec2 delta)
{
	//std::cout << "Panning\n";
	glm::vec2 panSpeed = getPanSpeed();
	focalPoint += -getRightVec() * delta.x * panSpeed.x * getDistanceToFocalPoint();
	focalPoint += getUpVec() * delta.y * panSpeed.y * getDistanceToFocalPoint();
}

void Editor_Camera::zoomCamera()
{
	distanceToFP += -InputHandler::getMouseScrollState() * getZoomSpeed();
	if (distanceToFP < 1.f)
	{
		//std::cout << "we hitting here ouchy\n";
		//focalPoint += getForwardVec();
		distanceToFP = 1.1f;
	}
}

glm::vec2 Editor_Camera::getPanSpeed() //  Copied from Cherno no cappo
{
	float x = std::min(viewport.x / 1000.0f, 2.4f); // max = 2.4f
	float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

	float y = std::min(viewport.y / 1000.0f, 2.4f); // max = 2.4f
	float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

	return { xFactor, yFactor };
}

void Editor_Camera::setViewportSize(float width, float height)
{
	viewport = glm::vec2(width, height);
}

glm::vec2 Editor_Camera::getViewportSize()
{
	return viewport;
}

void Editor_Camera::setRotationSpeed(float speed)
{
	rotationSpeed = speed;
}

float Editor_Camera::getRotationSpeed()
{
	return rotationSpeed;
}


Ray3D Editor_Camera::Raycasting(double xpos, double ypos, glm::mat4 proj, glm::mat4 view, glm::vec3 eye)
{
	EditorDebugger::Instance().AddLog("Dimension: %f %f\n", EditorScene::Instance().GetDimension().x, EditorScene::Instance().GetDimension().y);

	//ypos += GLFW_Handler::height;

	//float x = (2.0f * xpos) / EditorScene::Instance().GetDimension().x - 1.0f;
	//float y = 1.0f - (2.0f * ypos) / EditorScene::Instance().GetDimension().y;

	//float x = (2.0f * xpos) / EditorScene::Instance().GetDimension().x - 1.0f;
	//float y = (2.0f * ypos) / EditorScene::Instance().GetDimension().y - 1.0f;


	float x = (2.0f * xpos) / 1600.f - 1.0f;
	float y = (2.0f * ypos) / 900.f - 1.0f;


	float z = 1.0f;

	glm::vec4 ray_clip(x, y, -1.0f, 1.0f);
	glm::vec4 ray_eye = glm::inverse(proj) * ray_clip;
	ray_eye.z = -1.0f;
	ray_eye.w = 0.0f;
	glm::vec4 inverse_ray_world = glm::inverse(view) * ray_eye;
	glm::vec3 ray_world = glm::vec3(inverse_ray_world);
	ray_world = glm::normalize(ray_world);

	Ray3D temp;
	temp.origin = eye;
	temp.direction = ray_world;

	return temp;
}


bool testRayOBB(glm::vec3 ray_origin,        // Ray origin, in world space
	glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
	glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
	glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
	glm::mat4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
	float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB)
)
{
	//ray_direction = glm::normalize(ray_direction);
	//float tMin = 0.0f;
	//float tMax = 100000.0f;

	//glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

	//glm::vec3 delta = OBBposition_worldspace - ray_origin;

	//for (int i = 0; i < 3; ++i)
	//{
	//	glm::vec3 axis(ModelMatrix[i].x, ModelMatrix[i].y, ModelMatrix[i].z);
	//	float e = glm::dot(axis, delta);
	//	float f = glm::dot(ray_direction, axis);

	//	// Beware, don't do the division if f is near 0 ! See full source code for details.
	//	std::cout << "f : " << f << "\n";
	//	float t1 = (e + aabb_min[i]) / f; // Intersection with the "left" plane
	//	float t2 = (e + aabb_max[i]) / f; // Intersection with the "right" plane

	//	if (t1 > t2) { // if wrong order
	//		float w = t1; t1 = t2; t2 = w; // swap t1 and t2
	//	}
	//	// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
	//	if (t2 < tMax) tMax = t2;
	//	// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
	//	if (t1 > tMin) tMin = t1;
	//	if (tMax < tMin)
	//	{
	//		std::cout << "tmax: " << tMax << "\n";
	//		std::cout << "tMin: " << tMin << "\n";
	//		std::cout << "failed in axis " << i << "\n";
	//		return false;
	//	}
	//}
	//return true;


	// Intersection method from Real-Time Rendering and Essential Mathematics for Games

	ray_direction = glm::normalize(ray_direction);


	float tMin = 0.0f;
	float tMax = 100000.0f;

	glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

	glm::vec3 delta = OBBposition_worldspace - ray_origin;

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
		float e = glm::dot(xaxis, delta);
		float f = glm::dot(ray_direction, xaxis);

		if (fabs(f) > 0.001f) { // Standard case

			float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
			float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
			// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

			// We want t1 to represent the nearest intersection, 
			// so if it's not the case, invert t1 and t2
			if (t1 > t2) {
				float w = t1; t1 = t2; t2 = w; // swap t1 and t2
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < tMax)
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > tMin)
				tMin = t1;

			// And here's the trick :
			// If "far" is closer than "near", then there is NO intersection.
			// See the images in the tutorials for the visual explanation.
			if (tMax < tMin)
				return false;

		}
		else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
				return false;
		}
	}

	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
		glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
		float e = glm::dot(yaxis, delta);
		float f = glm::dot(ray_direction, yaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.y) / f;
			float t2 = (e + aabb_max.y) / f;

			if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
				return false;
		}
	}

	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
		glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
		float e = glm::dot(zaxis, delta);
		float f = glm::dot(ray_direction, zaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.z) / f;
			float t2 = (e + aabb_max.z) / f;

			if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
				return false;
		}
	}

	intersection_distance = tMin;
	return true;

}

