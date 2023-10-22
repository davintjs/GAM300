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

	glCreateTextures(GL_TEXTURE_2D, 1, &depthAttachment);
	glBindTexture(GL_TEXTURE_2D, depthAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width,
		height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

	// Attaching depth and stencil attachment onto framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);
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
	glBindTexture(GL_TEXTURE_2D, depthAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width,
		height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

	// Attaching depth and stencil attachment onto framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);

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

	E_ASSERT(errorId != GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!!");
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
	std::cout << "we were here\n";

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