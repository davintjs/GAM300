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

void BaseCamera::Init()
{
	aspect = 16.f / 9.f;
	fieldOfView = 45.0f;
	nearClip = 0.1f;
	farClip = 10000.f;
	focalLength = 10.f;

	UpdateViewMatrix();
	UpdateProjection();

	Framebuffer& framebuffer = FRAMEBUFFER.CreateFramebuffer();
	framebufferID = framebuffer.frameBufferObjectID;

	FRAMEBUFFER.RenderToTexture(framebuffer, 1600, 900, ATTACHMENTTYPE::COLOR, TEXTUREPARAMETERS::DEFAULT);
	colorAttachment = FRAMEBUFFER.GetCurrentColorAttachment(framebuffer);

	FRAMEBUFFER.RenderToBuffer(framebuffer, 1600, 900, ATTACHMENTTYPE::DEPTH, TEXTUREPARAMETERS::BLOOM);
	hdrColorAttachment = FRAMEBUFFER.GetCurrentColorAttachment(framebuffer);

	FRAMEBUFFER.RenderToTexture(framebuffer, 1600, 900, ATTACHMENTTYPE::COLOR, TEXTUREPARAMETERS::BLOOM, BUFFERTYPE::RENDERBUFFER);
	bloomAttachment = FRAMEBUFFER.GetCurrentColorAttachment(framebuffer);
}

void BaseCamera::Init(const glm::vec2& _dimension, const float& _fov, const float& _nearClip, const float& _farClip, const float& _focalLength)
{
	aspect = 16.f / 9.f;
	fieldOfView = _fov;
	nearClip = _nearClip;
	farClip = _farClip;
	focalLength = _focalLength;

	UpdateViewMatrix();
	UpdateProjection();

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
	UpdateFrustum();

	UpdateViewMatrix();

	UpdateProjection();
}

void BaseCamera::UpdateViewMatrix()
{
	cameraPosition = GetCameraPosition();

	glm::quat Orientation = GetOrientation();
	viewMatrix = glm::translate(glm::mat4(1.0f), cameraPosition) * glm::mat4(Orientation);
	viewMatrix = glm::inverse(viewMatrix);
}

void BaseCamera::UpdateProjection()
{
	projMatrix = glm::perspective(glm::radians(fieldOfView), aspect, nearClip, farClip);
	//std::cout << "field of view : " << fieldOfView << "\n";
}

void BaseCamera::UpdateFrustum()
{

}

void BaseCamera::UpdateCamera(const glm::vec3& _position, const glm::vec3& _rotation)
{
	SetCameraPosition(_position);

	SetCameraRotation(_rotation);

	Update();
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

bool BaseCamera::WithinFrustum() const
{
	return false;
}

void BaseCamera::SetCameraRotation(const glm::vec3& _rotation)
{
	pitch = -_rotation.x;
	yaw = -_rotation.y;
	roll = _rotation.z;
}

void BaseCamera::SetCameraPosition(const glm::vec3& _position)
{
	cameraPosition = _position;
	focalPoint = GetFocalPoint();
}

glm::vec3 BaseCamera::GetCameraPosition()
{
	return focalPoint - (GetForwardVec() * GetFocalLength());
}

glm::vec3 BaseCamera::GetFocalPoint()
{
	return cameraPosition + (GetForwardVec() * GetFocalLength());
}