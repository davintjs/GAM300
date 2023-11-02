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

void FramebufferManager::Init()
{

}

void FramebufferManager::Update(float)
{

}

void FramebufferManager::Exit()
{
	for (Framebuffer& framebuffer : framebuffers)
	{
		// Delete the textures in the framebuffer
		for (auto& map : framebuffer.attachments)
		{
			Attachment& attachment = map.second;
			
			if(attachment.index)
				glDeleteTextures(1, &attachment.index);
			if (attachment.depthIndex)
				glDeleteTextures(1, &attachment.depthIndex);
		}

		glDeleteFramebuffers(1, &framebuffer.frameBufferObjectID);
	}
}

void FramebufferManager::CreateDirectionalAndSpotLight(GLuint& _index, GLuint& _textureID, const GLsizei& _width, const GLsizei& _height)
{
	Framebuffer& fm = CreateFramebuffer();
	RenderToTexture(fm, _width, _height, ATTACHMENTTYPE::DEPTH, TEXTUREPARAMETERS::BORDER, BUFFERTYPE::DIRECTIONALANDSPOTLIGHT);

	_index = fm.frameBufferObjectID;
	_textureID = fm.attachments[GL_DEPTH_ATTACHMENT].depthIndex;
}

void FramebufferManager::CreatePointLight(GLuint& _index, GLuint& _textureID, const GLsizei& _width, const GLsizei& _height)
{
	Framebuffer& fm = CreateFramebuffer();
	RenderToTexture(fm, _width, _height, ATTACHMENTTYPE::DEPTH, TEXTUREPARAMETERS::CUBEMAP, BUFFERTYPE::POINTLIGHT);

	_index = fm.frameBufferObjectID;
	_textureID = fm.attachments[GL_DEPTH_ATTACHMENT].depthIndex;
}


void FramebufferManager::CreateColorPicking(GLuint& _index, GLuint& _textureID)
{

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glGenFramebuffers(1, &_index);
	glBindFramebuffer(GL_FRAMEBUFFER, _index);
	//std::cout << "generate ID : " << tex_Id << "\n"; // debug
	glGenTextures(1, &_textureID);
	//std::cout << "generated ID : " << tex_Id << "\n"; // debug
	//glActiveTexture(GL_TEXTURE0 + tex_Id);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1600, 900
		, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureID, 0);
	//unsigned int rbo_id;
	//glGenRenderbuffers(1, &rbo_id);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1600, 900);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_id);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void FramebufferManager::CreateBloom(GLuint* _indexes, GLuint* _textureIDs)
{
	for (unsigned int i = 0; i < 2; i++)
	{
		Framebuffer& fm = CreateFramebuffer();
		RenderToTexture(fm, 1600, 900, ATTACHMENTTYPE::COLOR, TEXTUREPARAMETERS::BLOOM);
		_indexes[i] = fm.frameBufferObjectID;
		_textureIDs[i] = fm.attachments[GL_COLOR_ATTACHMENT0].index;
	}
}

Framebuffer& FramebufferManager::CreateFramebuffer()
{
	framebuffers.push_back(Framebuffer());
	glCreateFramebuffers(1, &framebuffers.back().frameBufferObjectID);

	return framebuffers.back();
}

Framebuffer& FramebufferManager::CreateFramebuffer(const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	// Check if there is framebuffer with the specific texture type and still has attachment
	Framebuffer* framebuffer = nullptr;
	for (size_t i = 0; i < framebuffers.size(); i++)
	{
		// Check if the framebuffer still has color attachments
		if (framebuffers[i].attachment < 16)
		{
			framebuffer = &framebuffers[i];
			break;
		}
	}

	// Create new framebuffer
	if (!framebuffer)
		framebuffer = &CreateFramebuffer();

	if (_attachmentType == ATTACHMENTTYPE::DEPTH)
		RenderToBuffer(*framebuffer, _width, _height, _attachmentType, _textureFormat, _type);
	else // Bean: To change later
		RenderToTexture(*framebuffer, _width, _height, _attachmentType, _textureFormat, _type);

	return *framebuffer;
}

Framebuffer* FramebufferManager::GetFramebufferByID(const GLuint& _framebufferId)
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

GLenum FramebufferManager::GetCurrentAttachment(Framebuffer& _framebuffer) const
{
	return _framebuffer.readBuffer;
}

GLenum FramebufferManager::GetCurrentAttachment(const GLuint& _framebufferId)
{
	return GetFramebufferByID(_framebufferId)->readBuffer;
}

GLenum FramebufferManager::GetCurrentColorAttachment(Framebuffer& _framebuffer) const
{
	return GL_COLOR_ATTACHMENT0 + _framebuffer.colorAttachments - 1;
}

GLenum FramebufferManager::GetCurrentColorAttachment(const GLuint& _framebufferId)
{
	return GL_COLOR_ATTACHMENT0 + GetFramebufferByID(_framebufferId)->colorAttachments - 1;
}

GLuint FramebufferManager::GetTextureID(Framebuffer& _framebuffer, const GLenum& _attachment)
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

void FramebufferManager::RenderToTexture(Framebuffer& _framebuffer, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
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

Attachment FramebufferManager::CreateTextureAttachment(Framebuffer& _framebuffer, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	Attachment attachment;
	attachment.width = _width;
	attachment.height = _height;
	attachment.target = GL_TEXTURE_2D;
	attachment.depthIndex = 0;
	attachment.type = _type;

	switch (_attachmentType)
	{
	case ATTACHMENTTYPE::COLOR:
		CreateTexture(attachment.index, _width, _height, _textureFormat, _type);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _framebuffer.colorAttachments, GL_TEXTURE_2D, attachment.index, 0);
		break;
	case ATTACHMENTTYPE::DEPTH:
		CreateTexture(attachment.depthIndex, _width, _height, _textureFormat, _type);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, attachment.depthIndex, 0);
		break;
	case ATTACHMENTTYPE::STENCIL:
		CreateTexture(attachment.depthIndex, _width, _height, _textureFormat, _type);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, attachment.depthIndex, 0);
		break;
	case ATTACHMENTTYPE::DEPTHSTENCIL:
		CreateTexture(attachment.depthIndex, _width, _height, _textureFormat, _type);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, attachment.depthIndex, 0);
		break;
	}

	return attachment;
}

void FramebufferManager::RenderToBuffer(const GLuint& _framebufferId, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	RenderToBuffer(*GetFramebufferByID(_framebufferId), _width, _height, _attachmentType, _textureFormat, _type);
}

void FramebufferManager::RenderToBuffer(Framebuffer& _framebuffer, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
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

Attachment FramebufferManager::CreateRenderBufferAttachment(Framebuffer& _framebuffer, const GLsizei& _width, const GLsizei& _height, const TEXTUREPARAMETERS& _textureFormat, const BUFFERTYPE& _type)
{
	Attachment attachment;
	attachment.width = _width;
	attachment.height = _height;
	attachment.target = GL_TEXTURE_2D;
	attachment.depthIndex = 0;
	attachment.type = _type;

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
	const float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };

	switch (_textureFormat)
	{
	case TEXTUREPARAMETERS::DEFAULT:
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case TEXTUREPARAMETERS::BORDER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
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

void FramebufferManager::Bind(Framebuffer& _framebuffer) const
{
	Attachment& attachment = _framebuffer.attachments[GL_COLOR_ATTACHMENT0];
	glViewport(0, 0, attachment.width, attachment.height);

	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer.frameBufferObjectID);
}

void FramebufferManager::Bind(const GLuint& _framebufferId)
{
	Framebuffer& framebuffer = *GetFramebufferByID(_framebufferId);
	Attachment& attachment = framebuffer.attachments[GL_COLOR_ATTACHMENT0];
	
	glViewport(0, 0, attachment.width, attachment.height);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);
}

void FramebufferManager::Bind(const GLuint& _framebufferId, const GLenum& _attachment)
{
	Framebuffer& framebuffer = *GetFramebufferByID(_framebufferId);
	Attachment& attachment = framebuffer.attachments[_attachment];

	glViewport(0, 0, attachment.width, attachment.height);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);
}

void FramebufferManager::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}