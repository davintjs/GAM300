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

	/*framebuffer.SetSize((unsigned int)1600, (unsigned int)900);
	framebuffer.Init();*/

	Framebuffer2& framebuffer = FRAMEBUFFER.CreateFramebuffer();
	framebufferID = framebuffer.frameBufferObjectID;

	FRAMEBUFFER.RenderToTexture(framebuffer, GL_TEXTURE_2D, 1600, 900);
	colorAttachment = FRAMEBUFFER.GetCurrentAttachment(framebuffer);

	FRAMEBUFFER.RenderToBuffer(framebuffer, GL_DEPTH_ATTACHMENT, 1600, 900);
	hdrColorAttachment = FRAMEBUFFER.GetCurrentAttachment(framebuffer);

	// Bean: To modulate
	FRAMEBUFFER.Bind(framebufferID, hdrColorAttachment);

	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	glBindTexture(GL_TEXTURE_2D, hdrColorAttachment);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, hdrColorAttachment, GL_TEXTURE_2D, FRAMEBUFFER.GetTextureID(framebufferID, hdrColorAttachment), 0);

	RenderbufferAttachment renderbufferAttachment;
	renderbufferAttachment.width = 1600;
	renderbufferAttachment.height = 900;
	renderbufferAttachment.target = GL_TEXTURE_2D;	

	glCreateTextures(GL_TEXTURE_2D, 1, &renderbufferAttachment.index);
	glBindTexture(GL_TEXTURE_2D, renderbufferAttachment.index);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1600, 900, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// attach texture to framebuffer
	bloomAttachment = GL_COLOR_ATTACHMENT0 + framebuffer.attachment;
	framebuffer.attachments[bloomAttachment] = renderbufferAttachment;

	glFramebufferTexture2D(GL_FRAMEBUFFER, bloomAttachment, GL_TEXTURE_2D, renderbufferAttachment.index, 0);

	framebuffer.drawBuffers[framebuffer.attachment] = bloomAttachment; // Draw the max number of buffers
	framebuffer.readBuffer = bloomAttachment; // Read the latest buffer

	framebuffer.attachment++;

	FRAMEBUFFER.Completeness();

	FRAMEBUFFER.Unbind();
	// Bean: End of to modulate
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

	/*framebuffer.SetSize((unsigned int)_dimension.x, (unsigned int)_dimension.y);
	framebuffer.Init();*/
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

void BaseCamera::OnResize(const float& _width, const float& _height)
{
	dimension.x = _width;
	dimension.y = _height;
	aspect = dimension.x / dimension.y;

	UpdateProjection();

	//framebuffer.Resize((GLuint)dimension.x, (GLuint)dimension.y);
	FRAMEBUFFER.ChangeTexture(framebufferID, (GLsizei)dimension.x, (GLsizei)dimension.y, colorAttachment);
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