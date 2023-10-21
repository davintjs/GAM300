/*!***************************************************************************************
\file			Framebuffer.cpp
\project
\author         Sean Ngo

\par			Course: GAM300
\date           10/09/2023

\brief
	This file contains the definitions of the following:
	1. Framebuffer class which contains the basic two functions, init and exit
		a. Bind and Unbinding of the framebuffer
		b. Resizing the framebuffer viewport
		c. Getters and Setters for private data members

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "Framebuffer.h"
#include <GL/glew.h>

void Framebuffer::Init()
{
	glCreateFramebuffers(1, &frameBufferObjectID);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjectID);
	// Creating the color attachment
	glCreateTextures(GL_TEXTURE_2D, 1, &colorAttachment);
	glBindTexture(GL_TEXTURE_2D, colorAttachment);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, // Tweaked This -> Euan
	//	height, 0, GL_RGBA, GL_FLOAT, nullptr);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, // Sean
		height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	

	/*
	// Attaching color attachment onto framebuffer

	// Creating the depth and stencil attachment

	//glGenRenderbuffers(1, &depthAttachment);
	//glBindRenderbuffer(GL_RENDERBUFFER, depthAttachment);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	//GLuint stencilattachment;
	//glGenRenderbuffers(1, &stencilattachment);
	//glBindRenderbuffer(GL_RENDERBUFFER, stencilattachment);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_COMPONENTS, width, height);



	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthAttachment);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilattachment);
	*/

	//glCreateTextures(GL_TEXTURE_2D, 1, &depthAttachment);
	//glBindTexture(GL_TEXTURE_2D, depthAttachment);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width,
	//	height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

	//// Attaching depth and stencil attachment onto framebuffer
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Checking Completeness
	//Completeness();

	// Creating a HDR framebuffer object
	glCreateFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	// create floating point color buffer
	glCreateTextures(GL_TEXTURE_2D, 1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1600, 900, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	// create depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1600, 900);

	// attach buffers
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Checking Completeness
	//Completeness();
}

void Framebuffer::ReInit()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjectID);

	// Rebind color attachment
	glBindTexture(GL_TEXTURE_2D, colorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, // Sean
		height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

	// Rebind depth attachment
	//glBindTexture(GL_TEXTURE_2D, depthAttachment);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width,
	//	height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

	//// Attaching depth and stencil attachment onto framebuffer
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Completeness()
{
	int errorId = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (errorId == GL_FRAMEBUFFER_UNDEFINED)
		PRINT("Specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist.");
	else if (errorId == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
		PRINT("Any of the framebuffer attachment points are framebuffer incomplete");
	else if (errorId == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
		PRINT("Framebuffer does not have at least one image attached to it");
	else if (errorId == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
		PRINT("Value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi");
	else if (errorId == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
		PRINT("GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER");
	else if (errorId == GL_FRAMEBUFFER_UNSUPPORTED)
		PRINT("Combination of internal formats of the attached images violates an implementation-dependent set of restrictions");
	else if (errorId == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
		PRINT("Value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES");
	else if (errorId == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
		PRINT("Any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target");

	E_ASSERT(errorId == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!!");
}

void Framebuffer::Bind()
{
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjectID);
}

// Binds the framebuffer
void Framebuffer::Bind(const unsigned int& _objectID)
{
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, _objectID);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Bean: update via messaging
void Framebuffer::Resize(GLuint _width, GLuint _height)
{
	//COPIUM_ASSERT(_width == 0 || _height == 0, "Resize of Framebuffer Invalid!!");
	width = _width;
	height = _height;

	//PRINT("  Resize framebuffer " << width << " " << height);
	ReInit();
}

void Framebuffer::SetSize(GLuint _width, GLuint _height)
{
	width = _width;
	height = _height;
}

void Framebuffer::Exit()
{
	glDeleteFramebuffers(1, &frameBufferObjectID);
	glDeleteTextures(1, &colorAttachment);
	glDeleteTextures(1, &depthAttachment);
}

void FramebufferManager::Init()
{

}

void FramebufferManager::Update(float)
{

}

void FramebufferManager::Exit()
{

}

Framebuffer2& FramebufferManager::CreateFramebuffer(const GLenum& _textureType, const GLsizei& _width, const GLsizei& _height)
{
	// Check if there is framebuffer with the specific texture type and still has attachment
	Framebuffer2* framebuffer = nullptr;
	/*for (size_t i = 0; i < framebuffers.size(); i++)
	{
		if (framebuffers[i].attachment < 16)
		{
			framebuffer = &framebuffers[i];
			break;
		}
	}*/

	// Create new framebuffer
	if (!framebuffer)
	{
		framebuffers.push_back(Framebuffer2());
		framebuffer = &framebuffers.back();
		glCreateFramebuffers(1, &framebuffer->frameBufferObjectID);
	}

	//RenderToTexture(*framebuffer, _textureType, _width, _height);

	return *framebuffer;
}

Framebuffer2* FramebufferManager::GetFramebufferByID(const GLuint& _framebufferId)
{
	for (size_t i = 0; i < framebuffers.size(); i++)
	{
		if (framebuffers[i].frameBufferObjectID == _framebufferId)
		{
			return &framebuffers[i];
		}
	}

	return nullptr;
}

GLenum FramebufferManager::GetCurrentAttachment(Framebuffer2& _framebuffer) const
{
	return GL_COLOR_ATTACHMENT0 + _framebuffer.attachment - 1;
}

GLenum FramebufferManager::GetCurrentAttachment(const GLuint& _framebufferId)
{
	return GL_COLOR_ATTACHMENT0 + GetFramebufferByID(_framebufferId)->attachment - 1;
}

GLuint FramebufferManager::GetTextureID(Framebuffer2& _framebuffer, const GLenum& _attachment)
{
	return _framebuffer.attachments[_attachment].index;
}

GLuint FramebufferManager::GetTextureID(const GLuint& _framebufferId, const GLenum& _attachment)
{
	return GetFramebufferByID(_framebufferId)->attachments[_attachment].index;
}

void FramebufferManager::RenderToTexture(const GLuint& _framebufferId, const GLenum& _textureType, const GLsizei& _width, const GLsizei& _height)
{
	RenderToTexture(*GetFramebufferByID(_framebufferId), _textureType, _width, _height);
}

void FramebufferManager::RenderToTexture(Framebuffer2& _framebuffer, const GLenum& _textureType, const GLsizei& _width, const GLsizei& _height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer.frameBufferObjectID);

	// Creating the color attachment
	TextureAttachment textureAttachment = CreateTextureAttachment(_textureType, _width, _height);

	_framebuffer.attachments[GL_COLOR_ATTACHMENT0 + _framebuffer.attachment] = textureAttachment;

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _framebuffer.attachment, _textureType, textureAttachment.index, 0);
	
	// Attaching depth and stencil attachment onto framebuffer
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, _textureType, textureAttachment.depthID, 0);

	Completeness();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_framebuffer.drawBuffers[_framebuffer.attachment] = GL_COLOR_ATTACHMENT0 + _framebuffer.attachment; // Draw the max number of buffers
	_framebuffer.readBuffer = GL_COLOR_ATTACHMENT0 + _framebuffer.attachment; // Read the latest buffer

	_framebuffer.attachment++;

}

TextureAttachment FramebufferManager::CreateTextureAttachment(const GLenum& _textureType, const GLsizei& _width, const GLsizei& _height)
{
	TextureAttachment textureAttachment;
	textureAttachment.width = _width;
	textureAttachment.height = _height;
	textureAttachment.target = _textureType;
	textureAttachment.mipLevel = 0;
	textureAttachment.layer = 0;
	glCreateTextures(_textureType, 1, &textureAttachment.index);
	glBindTexture(_textureType, textureAttachment.index);

	glTexImage2D(_textureType, textureAttachment.mipLevel, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(_textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(_textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/*glCreateTextures(_textureType, 1, &textureAttachment.depthID);
	glBindTexture(_textureType, textureAttachment.depthID);
	glTexImage2D(_textureType, 0, GL_DEPTH24_STENCIL8, _width, _height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);*/

	return textureAttachment;
}

void FramebufferManager::RenderToBuffer(const GLuint& _framebufferId, const GLenum& _attachment, const GLsizei& _width, const GLsizei& _height)
{
	RenderToBuffer(*GetFramebufferByID(_framebufferId), _attachment, _width, _height);
}

void FramebufferManager::RenderToBuffer(Framebuffer2& _framebuffer, const GLenum& _attachment, const GLsizei& _width, const GLsizei& _height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer.frameBufferObjectID);

	// Creating the color attachment
	RenderbufferAttachment renderbufferAttachment = CreateRenderBufferAttachment(_width, _height);

	_framebuffer.attachments[GL_COLOR_ATTACHMENT0 + _framebuffer.attachment] = renderbufferAttachment;

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _framebuffer.attachment, GL_TEXTURE_2D, renderbufferAttachment.index, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, _attachment, GL_RENDERBUFFER, renderbufferAttachment.depthID);

	Completeness();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_framebuffer.drawBuffers[_framebuffer.attachment] = GL_COLOR_ATTACHMENT0 + _framebuffer.attachment; // Draw the max number of buffers
	_framebuffer.readBuffer = GL_COLOR_ATTACHMENT0 + _framebuffer.attachment; // Read the latest buffer

	_framebuffer.attachment++;

}

RenderbufferAttachment FramebufferManager::CreateRenderBufferAttachment(const GLsizei& _width, const GLsizei& _height)
{
	RenderbufferAttachment renderbufferAttachment;
	renderbufferAttachment.width = _width;
	renderbufferAttachment.height = _height;
	renderbufferAttachment.target = GL_TEXTURE_2D;
	glCreateTextures(GL_TEXTURE_2D, 1, &renderbufferAttachment.index);
	glBindTexture(GL_TEXTURE_2D, renderbufferAttachment.index);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenRenderbuffers(1, &renderbufferAttachment.depthID);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbufferAttachment.depthID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);

	return renderbufferAttachment;
}

void FramebufferManager::ChangeTexture(const GLuint& _framebufferId, const GLsizei& _width, const GLsizei& _height, const GLenum& _attachment)
{
	Framebuffer2* framebuffer = GetFramebufferByID(_framebufferId);

	if (framebuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

		TextureAttachment& attachment = *reinterpret_cast<TextureAttachment*>(&framebuffer->attachments[_attachment]);
		attachment.width = _width;
		attachment.height = _height;

		// Rebind color attachment
		glBindTexture(attachment.target, attachment.index);
		glTexImage2D(attachment.target, attachment.mipLevel, GL_RGBA8, _width,
			_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, _attachment, attachment.target, attachment.index, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void FramebufferManager::Completeness()
{
	int errorId = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (errorId == GL_FRAMEBUFFER_UNDEFINED)
		PRINT("Specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist.\n");
	else if (errorId == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
		PRINT("Any of the framebuffer attachment points are framebuffer incomplete\n");
	else if (errorId == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
		PRINT("Framebuffer does not have at least one image attached to it\n");
	else if (errorId == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
		PRINT("Value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi\n");
	else if (errorId == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
		PRINT("GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER\n");
	else if (errorId == GL_FRAMEBUFFER_UNSUPPORTED)
		PRINT("Combination of internal formats of the attached images violates an implementation-dependent set of restrictions\n");
	else if (errorId == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
		PRINT("Value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES\n");
	else if (errorId == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
		PRINT("Any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target\n");

	E_ASSERT(errorId == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!!");
}

void FramebufferManager::Bind(Framebuffer2& _framebuffer) const
{
	TextureAttachment* attachment = reinterpret_cast<TextureAttachment*>(&_framebuffer.attachments[GL_COLOR_ATTACHMENT0]);
	glViewport(0, 0, attachment->width, attachment->height);

	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer.frameBufferObjectID);
}

void FramebufferManager::Bind(const GLuint& _framebufferId)
{
	Framebuffer2& framebuffer = *GetFramebufferByID(_framebufferId);
	Attachment& attachment = framebuffer.attachments[GL_COLOR_ATTACHMENT0];
	
	glViewport(0, 0, attachment.width, attachment.height);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);
}

void FramebufferManager::Bind(const GLuint& _framebufferId, const GLenum& _attachment)
{
	Framebuffer2& framebuffer = *GetFramebufferByID(_framebufferId);
	Attachment& attachment = framebuffer.attachments[_attachment];

	glViewport(0, 0, attachment.width, attachment.height);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);
}

void FramebufferManager::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}