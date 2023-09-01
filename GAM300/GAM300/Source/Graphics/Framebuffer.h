#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "glslshader.h"

class Framebuffer
{
public:
	// Constructors

	/***************************************************************************/
	/*!
	\brief
		Initialises the framebuffer by creating a frame buffer and binding it to
		a texture
	*/
	/***************************************************************************/
	void init();

	/***************************************************************************/
	/*!
	\brief
		Deletes the framebuffer and its textures
	*/
	/***************************************************************************/
	void exit();

	/***************************************************************************/
	/*!
	\brief
		Binds the framebuffer
	*/
	/***************************************************************************/
	void bind();

	/***************************************************************************/
	/*!
	\brief
		Unbinds the framebuffer
	*/
	/***************************************************************************/
	void unbind();

	/***************************************************************************/
	/*!
	\brief
		Resizes the scene view
	*/
	/***************************************************************************/
	void resize(GLuint _width, GLuint _height);

	void set_size(GLuint _width, GLuint _height);
	glm::vec2 get_size() { return glm::vec2(width, height); }

	GLuint get_color_attachment_id() const { return colorAttachment; }

	GLuint& get_buffer_object_id() { return frameBufferObjectID; }


private:
	GLuint frameBufferObjectID = 0;

	GLuint colorAttachment = 0;
	GLuint depthAttachment = 0;

	GLuint width = 0, height = 0;
};

#endif // !FRAMEBUFFER_H
