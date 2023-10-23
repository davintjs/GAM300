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

enum class RENDERTEXTURE
{
	NONE,
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
	Framebuffer2(const GLboolean& _isStatic = true) : isStatic{ _isStatic } {}

	std::map<GLenum, Attachment> attachments;
	GLenum drawBuffers[16] = { GL_NONE };
	GLenum readBuffer = GL_COLOR_ATTACHMENT0;
	GLuint attachment = 0;						// Number of attachments
	GLuint frameBufferObjectID = 0;
	GLboolean isStatic = true;					// Checks if the framebuffer is static(Same texture dimensions) or dynamic(Different texture dimensions)
};

ENGINE_SYSTEM(FramebufferManager)
{
public:
	void Init();

	void Update(float dt);

	void Exit();

	// Creates an empty framebuffer of specific type, either STATIC(Same Width & Height for all attachments)
	// or DYANMIC(Different Width & Height, usually for editor camera only)
	Framebuffer2& CreateFramebuffer(const GLenum& _textureType, const GLsizei& _width, const GLsizei& _height);
	
	// Creates an empty framebuffer, either STATIC(Same Width & Height for all attachments)
	// or DYANMIC(Different Width & Height, usually for editor camera only)
	Framebuffer2& CreateFramebuffer(const GLboolean& _isStatic = true);

	// Creates a STATIC(Same Width & Height for all attachments) framebuffer of
	// specific texture type with a texture attachment
	Framebuffer2& CreateStaticFramebuffer(const RENDERTEXTURE& _textureType, const GLenum& _attachment, const GLsizei& _width, const GLsizei& _height);

	// Creates a STATIC(Same Width & Height for all attachments) framebuffer of
	// specific texture type with a render buffer attachment
	Framebuffer2& CreateStaticFramebuffer(const GLenum& _attachment, const GLsizei& _width, const GLsizei& _height);
	
	// Creates an empty DYNAMIC(Different Width & Height, usually for editor camera only) framebuffer of 
	// specific texture type with a texture attachment
	Framebuffer2& CreateDynamicFramebuffer(const RENDERTEXTURE& _textureType, const GLenum& _attachment, const GLsizei& _width, const GLsizei& _height);

	// Creates an empty DYNAMIC(Different Width & Height, usually for editor camera only) framebuffer of 
	// specific texture type with a render buffer attachment
	Framebuffer2& CreateDynamicFramebuffer(const GLenum& _attachment, const GLsizei& _width, const GLsizei& _height);

	// Retrieve a pointer to the framebuffer by their id
	Framebuffer2* GetFramebufferByID(const GLuint& _framebufferId);

	// Get the current ATTACHMENT of the framebuffer
	GLenum GetCurrentAttachment(Framebuffer2& _framebuffer) const;

	// Get the current ATTACHMENT of the framebuffer using the id
	GLenum GetCurrentAttachment(const GLuint& _framebufferId);

	// Get the texture id of the framebuffer with the specific attachment
	GLuint GetTextureID(Framebuffer2& _framebuffer, const GLenum& _attachment);

	// Get the texture id of the framebuffer with the specific attachment using the framebuffer id
	GLuint GetTextureID(const GLuint& _framebufferId, const GLenum& _attachment);

	// Adds a render texture attachment into the framebuffer of specific ATTACHMENT, RENDERTEXTURE and dimension using the framebuffer id
	void RenderToTexture(const GLuint& _framebufferId, const GLenum& _textureType, const GLsizei& _width, const GLsizei& _height);

	// Adds a render texture attachment into the framebuffer of specific ATTACHMENT, RENDERTEXTURE and dimension
	void RenderToTexture(Framebuffer2& _framebuffer, const GLenum& _textureType, const GLsizei& _width, const GLsizei& _height);

	// Creates a texture attachment for the framebuffer
	TextureAttachment CreateTextureAttachment(const GLenum& _textureType, const GLsizei& _width, const GLsizei& _height);

	// Adds a render buffer into the framebuffer of specific ATTACHMENT and dimension using the framebuffer id
	void RenderToBuffer(const GLuint& _framebufferId, const GLenum& _attachment, const GLsizei& _width, const GLsizei& _height);

	// Adds a render buffer into the framebuffer of specific ATTACHMENT and dimension
	void RenderToBuffer(Framebuffer2& _framebuffer, const GLenum& _attachment, const GLsizei& _width, const GLsizei& _height);

	// Creates a render buffer attachment for the framebuffer
	RenderbufferAttachment CreateRenderBufferAttachment(const GLsizei& _width, const GLsizei& _height);

	// Change the texture's dimensions of the framebuffer using the framebuffer id
	void ChangeTexture(const GLuint& _framebufferId, const GLsizei& _width, const GLsizei& _height, const GLenum& _attachment);

	// Check for completeness of the framebuffer
	void Completeness();

	// Bind the frambuffer
	void Bind(Framebuffer2& _framebuffer) const;

	// Bind the framebuffer using the framebuffer id
	void Bind(const GLuint& _framebufferId);

	// Bind the framebuffer using the framebuffer id and specific 
	// ATTACHMENT(Set the viewport to the attachments width and height)
	void Bind(const GLuint& _framebufferId, const GLenum& _attachment);

	// Unbind the framebuffer
	void Unbind();

private:
	std::vector<Framebuffer2> framebuffers;
};

#endif // !FRAMEBUFFER_H
