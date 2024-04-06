/*!***************************************************************************************
\file			BaseCamera.cpp
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           15/10/2023

\brief
    This file contains the definitions of the following:
    1.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "BaseCamera.h"
#include "Framebuffer.h"
#include "Scene/Components.h"

FrustumPlane::FrustumPlane(const glm::vec3& _position, const glm::vec3& _normal)
	: normal(glm::normalize(_normal)), distance(glm::dot(_normal, _position))
{}

bool FrustumPlane::IsOnOrForwardPlane(const glm::vec3& _position, const glm::vec3 _extents) const
{
	// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
	const float r = _extents.x * std::abs(normal.x) +
		_extents.y * std::abs(normal.y) + _extents.z * std::abs(normal.z);

	return -r <= GetSignedDistanceToPlane(_position);
}

float FrustumPlane::GetSignedDistanceToPlane(const glm::vec3& _point) const
{
	return glm::dot(normal, _point) - distance;
}

bool Frustum::CheckIfWithinFrustum(const glm::vec3& _position, const glm::vec3 _extents) const
{
	return (topFace.IsOnOrForwardPlane(_position, _extents) &&
		bottomFace.IsOnOrForwardPlane(_position, _extents) &&
		rightFace.IsOnOrForwardPlane(_position, _extents) &&
		leftFace.IsOnOrForwardPlane(_position, _extents) &&
		farFace.IsOnOrForwardPlane(_position, _extents) &&
		nearFace.IsOnOrForwardPlane(_position, _extents));
}

void BaseCamera::Init()
{
	aspect = 16.f / 9.f;
	fieldOfView = 45.0f;
	nearClip = 0.1f;
	farClip = 100.f;
	lookatDistance = 10.f;
	focalPoint = { 0.f, 0.f, 0.f };
	cameraPosition = GetCameraPosition();

	UpdateViewMatrix();
	UpdateProjection();
	UpdateFrustum();

	Framebuffer& framebuffer = FRAMEBUFFER.CreateFramebuffer();
	framebufferID = framebuffer.frameBufferObjectID;

	FRAMEBUFFER.RenderToTexture(framebuffer, 1600, 900, ATTACHMENTTYPE::COLOR, TEXTUREPARAMETERS::DEFAULT);
	colorAttachment = FRAMEBUFFER.GetCurrentColorAttachment(framebuffer);

	FRAMEBUFFER.RenderToBuffer(framebuffer, 1600, 900, ATTACHMENTTYPE::DEPTH, TEXTUREPARAMETERS::BLOOM);
	hdrColorAttachment = FRAMEBUFFER.GetCurrentColorAttachment(framebuffer);

	FRAMEBUFFER.RenderToTexture(framebuffer, 1600, 900, ATTACHMENTTYPE::COLOR, TEXTUREPARAMETERS::BLOOM, BUFFERTYPE::RENDERBUFFER);
	bloomAttachment = FRAMEBUFFER.GetCurrentColorAttachment(framebuffer);
}

void BaseCamera::Init(const glm::vec2& _dimension, const float& _fov, const float& _nearClip, const float& _farClip, const float& _lookatDistance)
{
	aspect = 16.f / 9.f;
	fieldOfView = _fov;
	nearClip = _nearClip;
	farClip = _farClip;
	lookatDistance = _lookatDistance;
	focalPoint = { 0.f, 0.f, 0.f };
	cameraPosition = GetCameraPosition();

	UpdateViewMatrix();
	UpdateProjection();
	UpdateFrustum();

	Framebuffer& framebuffer = FRAMEBUFFER.CreateFramebuffer(1600, 900, ATTACHMENTTYPE::COLOR, TEXTUREPARAMETERS::DEFAULT);
	framebufferID = framebuffer.frameBufferObjectID;
	colorAttachment = FRAMEBUFFER.GetCurrentColorAttachment(framebuffer);

	FRAMEBUFFER.RenderToBuffer(framebuffer, 1600, 900, ATTACHMENTTYPE::DEPTH, TEXTUREPARAMETERS::BLOOM);
	hdrColorAttachment = FRAMEBUFFER.GetCurrentColorAttachment(framebuffer);

	FRAMEBUFFER.RenderToTexture(framebuffer, 1600, 900, ATTACHMENTTYPE::COLOR, TEXTUREPARAMETERS::BLOOM, BUFFERTYPE::RENDERBUFFER);
	bloomAttachment = FRAMEBUFFER.GetCurrentColorAttachment(framebuffer);
}

void BaseCamera::Update()
{
	UpdateViewMatrix();

	UpdateProjection();

	if (useFrustumCulling)
		UpdateFrustum();
}

void BaseCamera::UpdateViewMatrix()
{
	//glm::quat Orientation = GetOrientation();
	viewMatrix = glm::translate(glm::mat4(1.0f), cameraPosition) * glm::mat4(orientation);
	viewMatrix = glm::inverse(viewMatrix);
}

void BaseCamera::UpdateProjection()
{
	projMatrix = glm::perspective(glm::radians(fieldOfView), aspect, nearClip, farClip);
	//std::cout << "field of view : " << fieldOfView << "\n";
} 

void BaseCamera::UpdateFrustum()
{
	const float radians = tanf(glm::radians(fieldOfView) * 0.5f);
	const float farHeight = farClip * radians;
	const float farWidth = farHeight * aspect;
	const glm::vec3& frontMultFar = farClip * GetForwardVec();

	frustum.nearFace = { cameraPosition + nearClip * GetForwardVec(), GetForwardVec() };
	frustum.farFace = { cameraPosition + frontMultFar, -GetForwardVec() };
	frustum.leftFace = { cameraPosition, glm::cross(glm::normalize(frontMultFar - GetRightVec() * farWidth), GetUpVec()) };
	frustum.rightFace = { cameraPosition, glm::cross(GetUpVec(),glm::normalize(frontMultFar + GetRightVec() * farWidth)) };
	frustum.bottomFace = { cameraPosition, glm::cross(GetRightVec(), glm::normalize(frontMultFar - GetUpVec() * farHeight)) };
	frustum.topFace = { cameraPosition, glm::cross(glm::normalize(frontMultFar + GetUpVec() * farHeight), GetRightVec()) };

	/*const float fixedDT = 0.016f;
	static float timer = 0.f;
	if (timer > 1.f)
	{
		const float radians = tanf(glm::radians(fieldOfView) * 0.5f);
		const float farHeight = farClip * radians;
		const float farWidth = farHeight * aspect;
		const glm::vec3& frontMultFar = farClip * GetForwardVec();
		const glm::vec3& camPos = GetCameraPosition();

		frustum.nearFace = { camPos + nearClip * GetForwardVec(), GetForwardVec() };
		frustum.farFace = { camPos + frontMultFar, -GetForwardVec() };
		frustum.leftFace = { camPos, glm::cross(glm::normalize(frontMultFar - GetRightVec() * farWidth), GetUpVec()) };
		frustum.rightFace = { camPos, glm::cross(GetUpVec(),glm::normalize(frontMultFar + GetRightVec() * farWidth)) };
		frustum.bottomFace = { camPos, glm::cross(GetRightVec(), glm::normalize(frontMultFar - GetUpVec() * farHeight)) };
		frustum.topFace = { camPos, glm::cross(glm::normalize(frontMultFar + GetUpVec() * farHeight), GetRightVec()) };

		timer = 0.f;
	}

	timer += fixedDT;*/
}

void BaseCamera::UpdateCamera(const glm::vec3& _position, const glm::vec3& _rotation)
{
	if (!setFocalPoint)
		focalPoint = GetFocalPoint();

	SetCameraPosition(_position);

	// Gimbal lock condition
	float rotY = abs(_rotation.y);
	if (rotY > 0.001745f && rotY < 1.569051f || rotY > 1.572542f)
		SetCameraRotation(_rotation);

	SetDistance(glm::length(focalPoint - cameraPosition));

	Update();

	setFocalPoint = false;
}

void BaseCamera::UpdateCamera(const glm::vec3& _position)
{
	if (!setFocalPoint)
		focalPoint = GetFocalPoint();

	SetCameraPosition(_position);

	SetDistance(glm::length(focalPoint - cameraPosition));

	Update();

	setFocalPoint = setOrientation = false;
}

void BaseCamera::TryResize(glm::vec2 _newDimension)
{
	if (_newDimension.x != 0 && _newDimension.y != 0)
	{
		bool modified = false;
		_newDimension = glm::floor(_newDimension);

		glm::vec2 adjusted = dimension;
		if (adjusted.y > _newDimension.y || adjusted.y != _newDimension.y)
		{
			modified = true;
			adjusted = { (_newDimension.y) * (16.f / 9.f), _newDimension.y };
		}

		if (adjusted.x > _newDimension.x)
		{
			modified = true;
			adjusted = { _newDimension.x, (_newDimension.x) / (16.f / 9.f) };
		}

		// If there is any changes to the dimension and modifications, return
		if (dimension != adjusted && modified)
		{
			dimension = adjusted;

			OnResize(dimension.x, dimension.y);
		}
	}
}

void BaseCamera::OnResize(const float& _width, const float& _height)
{
	dimension.x = _width;
	dimension.y = _height;
	aspect = dimension.x / dimension.y;

	UpdateProjection();
}

bool BaseCamera::WithinFrustrumAnimation(Transform& _transform, const glm::vec3& _min, const glm::vec3& _max)
{
	if (!useFrustumCulling)
		return true;

	//Get global parameters
	const glm::mat4 worldMatrix = _transform.GetWorldMatrix();
	const glm::vec3 globalCenter = worldMatrix * glm::vec4(((_min + _max) * 0.5f), 1.f);

	// Distance Check
	if (distanceCheck >= glm::distance(globalCenter, cameraPosition))
		return true;

	const glm::vec3 right = worldMatrix[0] * _max.x;
	const glm::vec3 up = worldMatrix[1] * _max.y;
	const glm::vec3 forward = -worldMatrix[2] * _max.z;

	const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

	const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

	const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

	const glm::vec3 globalExtents = glm::vec3(newIi, newIj, newIk);
	farClip /= 2.f;
	const float radians = tanf(glm::radians(fieldOfView) * 0.5f);
	const float farHeight = farClip * radians;
	const float farWidth = farHeight * aspect;
	const glm::vec3& frontMultFar = farClip * GetForwardVec();
	Frustum test;
	test.nearFace = { cameraPosition + nearClip * GetForwardVec(), GetForwardVec() };
	test.farFace = { cameraPosition + frontMultFar, -GetForwardVec() };
	test.leftFace = { cameraPosition, glm::cross(glm::normalize(frontMultFar - GetRightVec() * farWidth), GetUpVec()) };
	test.rightFace = { cameraPosition, glm::cross(GetUpVec(),glm::normalize(frontMultFar + GetRightVec() * farWidth)) };
	test.bottomFace = { cameraPosition, glm::cross(GetRightVec(), glm::normalize(frontMultFar - GetUpVec() * farHeight)) };
	test.topFace = { cameraPosition, glm::cross(glm::normalize(frontMultFar + GetUpVec() * farHeight), GetRightVec()) };


	return (test.topFace.IsOnOrForwardPlane(globalCenter, globalExtents) &&
		test.bottomFace.IsOnOrForwardPlane(globalCenter, globalExtents) &&
		test.rightFace.IsOnOrForwardPlane(globalCenter, globalExtents) &&
		test.leftFace.IsOnOrForwardPlane(globalCenter, globalExtents) &&
		test.farFace.IsOnOrForwardPlane(globalCenter, globalExtents) &&
		test.nearFace.IsOnOrForwardPlane(globalCenter, globalExtents));

}

bool BaseCamera::WithinFrustum(Transform& _transform, const glm::vec3& _min, const glm::vec3& _max)
{
	if (!useFrustumCulling)
		return true;

	//Get global parameters
	const glm::mat4 worldMatrix = _transform.GetWorldMatrix();
	const glm::vec3 globalCenter = worldMatrix * glm::vec4(((_min + _max) * 0.5f), 1.f);
	
	// Distance Check
	if (distanceCheck >= glm::distance(globalCenter, cameraPosition))
		return true;

	const glm::vec3 right = worldMatrix[0] * _max.x;
	const glm::vec3 up = worldMatrix[1] * _max.y;
	const glm::vec3 forward = -worldMatrix[2] * _max.z;

	const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

	const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

	const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

	const glm::vec3 globalExtents = glm::vec3(newIi, newIj, newIk);

	// Frustum Culling
	return frustum.CheckIfWithinFrustum(globalCenter, globalExtents);
}

void BaseCamera::SetCameraRotation(const glm::vec3& _rotation)
{
	orientation = glm::quat(_rotation);
}

void BaseCamera::SetOrientation()
{
	setOrientation = true;
}

void BaseCamera::SetCameraPosition(const glm::vec3& _position)
{
	cameraPosition = _position;

	/*glm::vec3 viewDirection = focalPoint - cameraPosition;
	SetDistance(glm::length(viewDirection));*/
}

glm::vec3 BaseCamera::GetCameraPosition()
{
	return focalPoint - (GetForwardVec() * GetDistance());
}

void BaseCamera::SetFocalPoint(const glm::vec3& _position)
{
	focalPoint = _position;
	setFocalPoint = true;
}

glm::vec3 BaseCamera::GetFocalPoint()
{
	return cameraPosition + (GetForwardVec() * GetDistance());
}