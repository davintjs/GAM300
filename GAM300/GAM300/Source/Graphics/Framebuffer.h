/*!***************************************************************************************
\file			Framebuffer.h
\project
\author         Sean Ngo

\par			Course: GAM300
\date           10/09/2023

\brief
	This file contains the declarations of the following:
	1. Framebuffer class which contains the basic two functions, init and exit
		a. Bind and Unbinding of the framebuffer
		b. Resizing the framebuffer viewport
		c. Getters and Setters for private data members

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include "Core/SystemInterface.h"

#define FRAMEBUFFER FramebufferManager::Instance()

class Framebuffer
{
public:

	// Initialises the framebuffer by creating a frame buffer and binding it to a texture
	void Init();

	// Reinitialise the framebuffer without deleting
	void ReInit();

	void Completeness();

	// Deletes the framebuffer and its textures
	void Exit();

	// Binds the framebuffer
	void Bind();

	// Binds the framebuffer
	void Bind(const unsigned int& _objectID);

	// Unbinds the framebuffer
	void Unbind();

	// Resizes the framebuffer viewport
	void Resize(GLuint _width, GLuint _height);

	// Getter and setter for framebuffer size/dimension
	void SetSize(GLuint _width, GLuint _height);
	glm::vec2 GetSize() { return glm::vec2(width, height); }

	// Getter for color attachment id, used in the editor scene
	GLuint GetColorAttachmentId() const { return colorAttachment; }

	// Get the framebuffer id as reference
	GLuint& GetBufferObjectId() { return frameBufferObjectID; }

	unsigned int hdrFBO;
	unsigned int rboDepth;
	unsigned int colorBuffer;

private:
	GLuint frameBufferObjectID = 0;

	GLuint colorAttachment = 0;
	GLuint depthAttachment = 0;


	GLuint width = 0, height = 0;
};

enum class RenderTexture
{
	DEFAULT,
	MIPMAPS,
	CUBEMAPS,
	DEPTH,
	DEPTHSTENCIL
};

struct Attachment
{
	GLuint index;		// The texture object or renderbuffer object
	GLenum target;
	GLuint width = 0;
	GLuint height = 0;
};

struct TextureAttachment : public Attachment
{
	GLuint mipLevel;
	GLuint layer;
	GLuint depthID;
};

struct RenderbufferAttachment : public Attachment
{
	GLuint depthID;
};

struct Framebuffer2
{
	std::map<GLenum, Attachment> attachments;
	GLuint attachment = 0;						// Number of attachments
	GLuint frameBufferObjectID = 0;
	GLenum drawBuffers[16] = { GL_NONE };
	GLenum readBuffer = GL_COLOR_ATTACHMENT0;
};

ENGINE_SYSTEM(FramebufferManager)
{
public:
	void Init();

	void Update(float dt);

	void Exit();

	Framebuffer2& CreateFramebuffer(const GLenum& _textureType, const GLsizei& _width, const GLsizei& _height);

	Framebuffer2* GetFramebufferByID(const GLuint& _framebufferId);

	GLenum GetCurrentAttachment(Framebuffer2& _framebuffer) const;

	GLenum GetCurrentAttachment(const GLuint& _framebufferId);

	GLuint GetTextureID(Framebuffer2& _framebuffer, const GLenum& _attachment);

	GLuint GetTextureID(const GLuint& _framebufferId, const GLenum& _attachment);

	void RenderToTexture(const GLuint& _framebufferId, const GLenum& _textureType, const GLsizei& _width, const GLsizei& _height);
	
	void RenderToTexture(Framebuffer2& _framebuffer, const GLenum& _textureType, const GLsizei& _width, const GLsizei& _height);

	TextureAttachment CreateTextureAttachment(const GLenum& _textureType, const GLsizei& _width, const GLsizei& _height);

	void RenderToBuffer(const GLuint& _framebufferId, const GLenum& _attachment, const GLsizei& _width, const GLsizei& _height);

	void RenderToBuffer(Framebuffer2& _framebuffer, const GLenum& _attachment, const GLsizei& _width, const GLsizei& _height);

	RenderbufferAttachment CreateRenderBufferAttachment(const GLsizei& _width, const GLsizei& _height);

	void ChangeTexture(const GLuint& _framebufferId, const GLsizei& _width, const GLsizei& _height, const GLenum& _attachment);

	void Completeness();

	void Bind(Framebuffer2& _framebuffer) const;

	void Bind(const GLuint& _framebufferId);

	void Bind(const GLuint& _framebufferId, const GLenum& _attachment);

	void Unbind();

private:
	std::vector<Framebuffer2> framebuffers;
};

#endif // !FRAMEBUFFER_H
