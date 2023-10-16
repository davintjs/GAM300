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

#include "glslshader.h"

class Framebuffer
{
public:

	// Initialises the framebuffer by creating a frame buffer and binding it to a texture
	void init();

	// Deletes the framebuffer and its textures
	void exit();

	// Binds the framebuffer
	void bind();

	// Unbinds the framebuffer
	void unbind();

	// Resizes the framebuffer viewport
	void resize(GLuint _width, GLuint _height);

	// Getter and setter for framebuffer size/dimension
	void set_size(GLuint _width, GLuint _height);
	glm::vec2 get_size() { return glm::vec2(width, height); }

	// Getter for color attachment id, used in the editor scene
	GLuint get_color_attachment_id() const { return colorAttachment; }

	// Get the framebuffer id as reference
	GLuint& get_buffer_object_id() { return frameBufferObjectID; }

	unsigned int hdrFBO{};
	unsigned int rboDepth{};
	unsigned int colorBuffer{};

	unsigned int gBuffer{};
	unsigned int gPosition{};
	unsigned int gNormal{};
	unsigned int gAlbedoSpec{};

private:
	GLuint frameBufferObjectID = 0;

	GLuint colorAttachment = 0;
	GLuint depthAttachment = 0;


	GLuint width = 0, height = 0;
};

#endif // !FRAMEBUFFER_H
