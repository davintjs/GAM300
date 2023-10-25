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
	
	// Singluar color attachment
	/*
	// create floating point color buffer
	glCreateTextures(GL_TEXTURE_2D, 1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1600, 900, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, colorBuffer, 0);
	*/
	
	glGenTextures(2, colorBuffer);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1600, 900, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 + i, GL_TEXTURE_2D, colorBuffer[i], 0);
	}
	
	
	// create depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1600, 900);

	// attach buffers
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
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

	//for (unsigned int i = 0; i < 2; i++)
	//{
	//	glBindTexture(GL_TEXTURE_2D, colorBuffer[i]);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//	// attach texture to framebuffer
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 + i, GL_TEXTURE_2D, colorBuffer[i], 0);
	//}


	// Rebind depth attachment
	//glBindTexture(GL_TEXTURE_2D, depthAttachment);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width,
	//	height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

	//// Attaching depth and stencil attachment onto framebuffer
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void FramebufferManager::CreateDirectionalAndSpotLight(GLuint& _index, GLuint& _textureID, const GLsizei& _width, const GLsizei& _height)
{
	// This Framebuffer is used for both directional and spotlight
	glGenFramebuffers(1, &_index);
	// create depth texture
	glGenTextures(1, &_textureID);
	glBindFramebuffer(GL_FRAMEBUFFER, _index);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _textureID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "depth framebuffer exploded\n";
	else
		std::cout << "depth framebuffer created successfully\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferManager::CreatePointLight(GLuint& _index, GLuint& _textureID, const GLsizei& _width, const GLsizei& _height)
{
	/*Framebuffer2& fm = CreateFramebuffer();
	RenderToTexture(fm, _width, _height, ATTACHMENTTYPE::DEPTH, TEXTUREPARAMETERS::CUBEMAP);

	_index = fm.frameBufferObjectID;
	_textureID = fm.attachments[GL_DEPTH_ATTACHMENT].depthIndex;*/
	
	// This Framebuffer is used for pointlight
	glGenFramebuffers(1, &_index);
	// create depth cubemap texture
	glGenTextures(1, &_textureID);
	glBindFramebuffer(GL_FRAMEBUFFER, _index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach depth texture as FBO's depth buffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _textureID, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferManager::CreateBloom(GLuint* _indexes, GLuint* _textureIDs)
{
	for (unsigned int i = 0; i < 2; i++)
	{
		Framebuffer2& fm = CreateFramebuffer();
		RenderToTexture(fm, 1600, 900, ATTACHMENTTYPE::COLOR, TEXTUREPARAMETERS::BLOOM);
		_indexes[i] = fm.frameBufferObjectID;
		_textureIDs[i] = fm.attachments[GL_COLOR_ATTACHMENT0].index;
	}

	//glGenFramebuffers(2, pingpongFBO);
	//glGenTextures(2, pingpongColorbuffers);
	//for (unsigned int i = 0; i < 2; i++)
	//{
	//	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
	//	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1600, 900, 0, GL_RGBA, GL_FLOAT, NULL);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
	//}
}

Framebuffer2& FramebufferManager::CreateFramebuffer(const GLboolean& _isStatic)
{
	framebuffers.push_back(Framebuffer2(_isStatic));
	glCreateFramebuffers(1, &framebuffers.back().frameBufferObjectID);

	return framebuffers.back();
}

Framebuffer2& FramebufferManager::CreateStaticFramebuffer(const RENDERTEXTURE& _textureType, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	// Check if there is framebuffer with the specific texture type and still has attachment
	Framebuffer2* framebuffer = nullptr;
	for (size_t i = 0; i < framebuffers.size(); i++)
	{
		// Only check for static framebuffers
		if (!framebuffers[i].isStatic)
			continue;

		// Check if the framebuffer still has color attachments, same width and height
		if (framebuffers[i].attachment < 16 && framebuffers[i].attachments[0].width == _width && framebuffers[i].attachments[0].height == _height)
		{
			framebuffer = &framebuffers[i];
			break;
		}
	}

	// Create new framebuffer
	if (!framebuffer)
		framebuffer = &CreateFramebuffer();

	if (_textureType == RENDERTEXTURE::NONE)
		RenderToBuffer(*framebuffer, _width, _height, _attachmentType, _textureFormat, _type);
	else // Bean: To change later
		RenderToTexture(*framebuffer, _width, _height, _attachmentType, _textureFormat, _type);

	return *framebuffer;
}

Framebuffer2& FramebufferManager::CreateStaticFramebuffer(const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	return CreateStaticFramebuffer(RENDERTEXTURE::NONE, _width, _height, _attachmentType, _textureFormat, _type);
}

Framebuffer2& FramebufferManager::CreateDynamicFramebuffer(const RENDERTEXTURE& _textureType, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	// Check if there is framebuffer with the specific texture type and still has attachment
	Framebuffer2* framebuffer = nullptr;
	for (size_t i = 0; i < framebuffers.size(); i++)
	{
		// Only check for non-static framebuffers
		if (framebuffers[i].isStatic)
			continue;

		// Check if the framebuffer still has color attachments
		if (framebuffers[i].attachment < 16)
		{
			framebuffer = &framebuffers[i];
			break;
		}
	}

	// Create new framebuffer
	if (!framebuffer)
		framebuffer = &CreateFramebuffer(false);

	if (_textureType == RENDERTEXTURE::NONE)
		RenderToBuffer(*framebuffer, _width, _height, _attachmentType, _textureFormat, _type);
	else // Bean: To change later
		RenderToTexture(*framebuffer, _width, _height, _attachmentType, _textureFormat, _type);

	return *framebuffer;
}

Framebuffer2& FramebufferManager::CreateDynamicFramebuffer(const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	return CreateDynamicFramebuffer(RENDERTEXTURE::NONE, _width, _height, _attachmentType, _textureFormat, _type);
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
	return _framebuffer.readBuffer;
}

GLenum FramebufferManager::GetCurrentAttachment(const GLuint& _framebufferId)
{
	return GetFramebufferByID(_framebufferId)->readBuffer;
}

GLenum FramebufferManager::GetCurrentColorAttachment(Framebuffer2& _framebuffer) const
{
	return GL_COLOR_ATTACHMENT0 + _framebuffer.colorAttachments - 1;
}

GLenum FramebufferManager::GetCurrentColorAttachment(const GLuint& _framebufferId)
{
	return GL_COLOR_ATTACHMENT0 + GetFramebufferByID(_framebufferId)->colorAttachments - 1;
}

GLuint FramebufferManager::GetTextureID(Framebuffer2& _framebuffer, const GLenum& _attachment)
{
	return _framebuffer.attachments[_attachment].index;
}

GLuint FramebufferManager::GetTextureID(const GLuint& _framebufferId, const GLenum& _attachment)
{
	return GetFramebufferByID(_framebufferId)->attachments[_attachment].index;
}

void FramebufferManager::RenderToTexture(const GLuint& _framebufferId, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	RenderToTexture(*GetFramebufferByID(_framebufferId), _width, _height, _attachmentType, _textureFormat, _type);
}

void FramebufferManager::RenderToTexture(Framebuffer2& _framebuffer, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer.frameBufferObjectID);

	// Creating the color attachment
	Attachment textureAttachment = CreateTextureAttachment(_framebuffer, _width, _height, _attachmentType, _textureFormat, _type);

	Completeness();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	switch (_attachmentType)
	{
	case ATTACHMENTTYPE::COLOR:
		_framebuffer.attachments[GL_COLOR_ATTACHMENT0 + _framebuffer.colorAttachments] = textureAttachment;
		_framebuffer.drawBuffers[_framebuffer.attachment] = GL_COLOR_ATTACHMENT0 + _framebuffer.colorAttachments;
		_framebuffer.readBuffer = GL_COLOR_ATTACHMENT0 + _framebuffer.colorAttachments; // Read the latest buffer
		_framebuffer.colorAttachments++;
		break;
	case ATTACHMENTTYPE::DEPTH:
		_framebuffer.attachments[GL_DEPTH_ATTACHMENT] = textureAttachment;
		_framebuffer.drawBuffers[_framebuffer.attachment] = GL_DEPTH_ATTACHMENT;
		_framebuffer.readBuffer = GL_DEPTH_ATTACHMENT;
		break;
	case ATTACHMENTTYPE::STENCIL:
		_framebuffer.attachments[GL_STENCIL_ATTACHMENT] = textureAttachment;
		_framebuffer.drawBuffers[_framebuffer.attachment] = GL_STENCIL_ATTACHMENT;
		_framebuffer.readBuffer = GL_STENCIL_ATTACHMENT;
		break;
	case ATTACHMENTTYPE::DEPTHSTENCIL:
		_framebuffer.attachments[GL_DEPTH_STENCIL_ATTACHMENT] = textureAttachment;
		_framebuffer.drawBuffers[_framebuffer.attachment] = GL_DEPTH_STENCIL_ATTACHMENT;
		_framebuffer.readBuffer = GL_DEPTH_STENCIL_ATTACHMENT;
		break;
	}

	_framebuffer.attachment++;
}

Attachment FramebufferManager::CreateTextureAttachment(Framebuffer2& _framebuffer, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	Attachment attachment;
	attachment.width = _width;
	attachment.height = _height;
	attachment.target = GL_TEXTURE_2D;
	attachment.depthIndex = 0;

	CreateTexture(attachment.index, _width, _height, _textureFormat, _type);

	switch (_attachmentType)
	{
	case ATTACHMENTTYPE::COLOR:
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _framebuffer.colorAttachments, GL_TEXTURE_2D, attachment.index, 0);
		break;
	case ATTACHMENTTYPE::DEPTH:
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, attachment.depthIndex, 0);
		break;
	case ATTACHMENTTYPE::STENCIL:
		glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, attachment.depthIndex, 0);
		break;
	case ATTACHMENTTYPE::DEPTHSTENCIL:
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, attachment.depthIndex, 0);
		break;
	}

	return attachment;
}

void FramebufferManager::RenderToBuffer(const GLuint& _framebufferId, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	RenderToBuffer(*GetFramebufferByID(_framebufferId), _width, _height, _attachmentType, _textureFormat, _type);
}

void FramebufferManager::RenderToBuffer(Framebuffer2& _framebuffer, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer.frameBufferObjectID);

	// Creating the color attachment
	Attachment renderbufferAttachment = CreateRenderBufferAttachment(_framebuffer, _width, _height, _textureFormat, _type);

	Completeness();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Add the color attachment
	_framebuffer.attachments[GL_COLOR_ATTACHMENT0 + _framebuffer.colorAttachments] = renderbufferAttachment;
	_framebuffer.drawBuffers[_framebuffer.attachment] = GL_COLOR_ATTACHMENT0 + _framebuffer.colorAttachments; // Draw the max number of buffers
	_framebuffer.readBuffer = GL_COLOR_ATTACHMENT0 + _framebuffer.colorAttachments; // Read the latest buffer

	// Add the depth attachment
	_framebuffer.attachments[GL_DEPTH_ATTACHMENT] = renderbufferAttachment;
	_framebuffer.drawBuffers[_framebuffer.attachment + 1] = GL_DEPTH_ATTACHMENT;
	_framebuffer.readBuffer = GL_DEPTH_ATTACHMENT;

	_framebuffer.colorAttachments++;
	_framebuffer.attachment += 2; // Two attachments were created
}

Attachment FramebufferManager::CreateRenderBufferAttachment(Framebuffer2& _framebuffer, const GLsizei& _width, const GLsizei& _height, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	Attachment attachment;
	attachment.width = _width;
	attachment.height = _height;
	attachment.target = GL_TEXTURE_2D;
	attachment.depthIndex = 0;

	CreateTexture(attachment.index, _width, _height, _textureFormat, _type);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _framebuffer.colorAttachments, GL_TEXTURE_2D, attachment.index, 0);

	glGenRenderbuffers(1, &attachment.depthIndex);
	glBindRenderbuffer(GL_RENDERBUFFER, attachment.depthIndex);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, attachment.depthIndex);

	return attachment;
}

void FramebufferManager::CreateTexture(GLuint& _index, const GLsizei& _width, const GLsizei& _height, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	if (_type != BUFFERTYPE::POINTLIGHT)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &_index);
		glBindTexture(GL_TEXTURE_2D, _index);
	}
	else
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_index);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _index);
	}
	
	SetTextureFormat(_width, _height, _type);
	
	SetTextureParameters(_textureFormat);
}

void FramebufferManager::SetTextureFormat(const GLsizei& _width, const GLsizei& _height, const BUFFERTYPE& _type)
{
	switch (_type)
	{
	case BUFFERTYPE::TEXTURE:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		break;
	case BUFFERTYPE::RENDERBUFFER:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGBA, GL_FLOAT, nullptr);
		break;
	case BUFFERTYPE::DIRECTIONALANDSPOTLIGHT:
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		break;
	case BUFFERTYPE::POINTLIGHT:
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		break;
	case BUFFERTYPE::DEPTH:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, _width, _height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		break;
	}
}

void FramebufferManager::SetTextureParameters(const TEXTUREPARAMETERS& _textureFormat)
{
	switch (_textureFormat)
	{
	case TEXTUREPARAMETERS::DEFAULT:
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case TEXTUREPARAMETERS::CUBEMAP:
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		break;
	case TEXTUREPARAMETERS::BLOOM:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	}
}

void FramebufferManager::ChangeTexture(const GLuint& _framebufferId, const GLsizei& _width, const GLsizei& _height, const GLenum& _attachment)
{
	Framebuffer2* framebuffer = GetFramebufferByID(_framebufferId);

	if (framebuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

		Attachment& attachment = framebuffer->attachments[_attachment];
		attachment.width = _width;
		attachment.height = _height;

		// Rebind color attachment
		glBindTexture(attachment.target, attachment.index);
		glTexImage2D(attachment.target, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, _attachment, GL_TEXTURE_2D, attachment.index, 0);

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
	Attachment& attachment = _framebuffer.attachments[GL_COLOR_ATTACHMENT0];
	glViewport(0, 0, attachment.width, attachment.height);

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