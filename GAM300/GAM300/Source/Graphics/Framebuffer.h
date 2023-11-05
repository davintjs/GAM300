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

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include "Core/SystemInterface.h"

#define FRAMEBUFFER FramebufferManager::Instance()

enum class TEXTUREPARAMETERS
{
	DEFAULT,
	BORDER,		// For directional and spotlight
	CUBEMAP,	// For point light
	BLOOM
};

enum class BUFFERTYPE
{
	TEXTURE,
	RENDERBUFFER,
	DIRECTIONALANDSPOTLIGHT,
	POINTLIGHT,
	DEPTH
};

enum class ATTACHMENTTYPE
{
	COLOR,
	DEPTH,
	STENCIL,
	DEPTHSTENCIL
};

struct Attachment
{
	GLuint index;		// The texture object or renderbuffer object
	GLenum target;
	GLuint width = 0;
	GLuint height = 0;
	GLuint depthIndex;	// The texture object for depth attachment
	BUFFERTYPE type;
};

struct Framebuffer
{
	std::map<GLenum, Attachment> attachments;
	GLenum drawBuffers[16] = { GL_NONE };
	GLenum readBuffer = GL_COLOR_ATTACHMENT0;
	GLuint attachment = 0;						// Number of attachments
	GLuint colorAttachments = 0;				// Number of color attachments
	GLuint frameBufferObjectID = 0;
};

ENGINE_SYSTEM(FramebufferManager)
{
public:
	// Initialize the framebuffer manager
	void Init();

	// Update the manager
	void Update(float dt);

	// Exit the framebuffer manager
	void Exit();

	// Create a framebuffer for directional and spot lights
	void CreateDirectionalAndSpotLight(GLuint& _index, GLuint& _textureID, const GLsizei& _width, const GLsizei& _height);

	// Create a framebuffer for point light
	void CreatePointLight(GLuint& _index, GLuint& _textureID, const GLsizei& _width, const GLsizei& _height);

	// Create a framebuffer for bloom
	void CreateBloom(GLuint* _indexes, GLuint* _textureIDs);

	// Create a framebuffer for color picking
	void CreateColorPicking(GLuint & _index, GLuint & _textureID);

	// Creates an empty framebuffer, all attachments will have the same width and height
	Framebuffer& CreateFramebuffer();

	// Creates an empty DYNAMIC(Different Width & Height, usually for editor camera only) framebuffer of 
	// specific texture type with a texture attachment
	Framebuffer& CreateFramebuffer(const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType = ATTACHMENTTYPE::COLOR, const TEXTUREPARAMETERS& _textureFormat = TEXTUREPARAMETERS::DEFAULT, const BUFFERTYPE& _type = BUFFERTYPE::TEXTURE);

	// Retrieve a pointer to the framebuffer by their id
	Framebuffer* GetFramebufferByID(const GLuint& _framebufferId);

	// Get the current ATTACHMENT of the framebuffer
	GLenum GetCurrentAttachment(Framebuffer& _framebuffer) const;

	// Get the current ATTACHMENT of the framebuffer using the id
	GLenum GetCurrentAttachment(const GLuint& _framebufferId);

	// Get the current color ATTACHMENT of the framebuffer
	GLenum GetCurrentColorAttachment(Framebuffer& _framebuffer) const;

	// Get the current color ATTACHMENT of the framebuffer using the id
	GLenum GetCurrentColorAttachment(const GLuint& _framebufferId);

	// Get the texture id of the framebuffer with the specific attachment
	GLuint GetTextureID(Framebuffer& _framebuffer, const GLenum& _attachment);

	// Get the texture id of the framebuffer with the specific attachment using the framebuffer id
	GLuint GetTextureID(const GLuint& _framebufferId, const GLenum& _attachment);

	// Adds a render texture attachment into the framebuffer of specific ATTACHMENT, RENDERTEXTURE and dimension using the framebuffer id
	void RenderToTexture(const GLuint& _framebufferId, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType = ATTACHMENTTYPE::COLOR, const TEXTUREPARAMETERS& _textureFormat = TEXTUREPARAMETERS::DEFAULT, const BUFFERTYPE& _type = BUFFERTYPE::TEXTURE);

	// Adds a render texture attachment into the framebuffer of specific ATTACHMENT, RENDERTEXTURE and dimension
	void RenderToTexture(Framebuffer& _framebuffer, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType = ATTACHMENTTYPE::COLOR, const TEXTUREPARAMETERS& _textureFormat = TEXTUREPARAMETERS::DEFAULT, const BUFFERTYPE& _type = BUFFERTYPE::TEXTURE);

	// Adds a render buffer into the framebuffer of specific ATTACHMENT and dimension using the framebuffer id
	void RenderToBuffer(const GLuint& _framebufferId, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType = ATTACHMENTTYPE::COLOR, const TEXTUREPARAMETERS& _textureFormat = TEXTUREPARAMETERS::DEFAULT, const BUFFERTYPE& _type = BUFFERTYPE::RENDERBUFFER);

	// Adds a render buffer into the framebuffer of specific ATTACHMENT and dimension
	void RenderToBuffer(Framebuffer& _framebuffer, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType = ATTACHMENTTYPE::COLOR, const TEXTUREPARAMETERS& _textureFormat = TEXTUREPARAMETERS::DEFAULT, const BUFFERTYPE& _type = BUFFERTYPE::RENDERBUFFER);

	// Create a texture for the attachment
	void CreateTexture(GLuint& _index, const GLsizei& _width, const GLsizei& _height, const TEXTUREPARAMETERS& _textureFormat = TEXTUREPARAMETERS::DEFAULT, const BUFFERTYPE& _type = BUFFERTYPE::TEXTURE);

	// Set the TexImage2D formats for the texture
	void SetTextureFormat(const GLsizei& _width, const GLsizei& _height, const BUFFERTYPE& _type = BUFFERTYPE::TEXTURE);

	// Set the texture parameters for the texture that is assigned to the attachment
	void SetTextureParameters(const TEXTUREPARAMETERS& _textureFormat);

	// Bind the frambuffer
	void Bind(Framebuffer& _framebuffer) const;

	// Bind the framebuffer using the framebuffer id
	void Bind(const GLuint& _framebufferId);

	// Bind the framebuffer using the framebuffer id and specific 
	// ATTACHMENT(Set the viewport to the attachments width and height)
	void Bind(const GLuint& _framebufferId, const GLenum& _attachment);

	// Unbind the framebuffer
	void Unbind();

private:

	// Creates a texture attachment for the framebuffer
	Attachment CreateTextureAttachment(Framebuffer& _framebuffer, const GLsizei& _width, const GLsizei& _height, const ATTACHMENTTYPE& _attachmentType = ATTACHMENTTYPE::COLOR, const TEXTUREPARAMETERS& _textureFormat = TEXTUREPARAMETERS::DEFAULT, const BUFFERTYPE& _type = BUFFERTYPE::TEXTURE);

	// Creates a render buffer attachment for the framebuffer
	Attachment CreateRenderBufferAttachment(Framebuffer& _framebuffer, const GLsizei& _width, const GLsizei& _height, const TEXTUREPARAMETERS& _textureFormat = TEXTUREPARAMETERS::DEFAULT, const BUFFERTYPE& _type = BUFFERTYPE::RENDERBUFFER);

	// Check for completeness of the framebuffer
	void Completeness();

	std::vector<Framebuffer> framebuffers;
};

#endif // !FRAMEBUFFER_H
