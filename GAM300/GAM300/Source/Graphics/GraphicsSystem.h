/*!***************************************************************************************
\file			GraphicsSystem.h
\project
\author         Lian Khai Kiat, Euan Lim, Theophelia Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declaration of Graphics System that includes:
	1. Setting up of shaders and VAOs
	2. Updating buffers
	3. Drawing

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "Core/SystemInterface.h"
#include "GraphicStructsAndClass.h"

class Ray3D;

ENGINE_SYSTEM(GraphicsSystem)
{
public:
	// Initialize graphics system
	void Init();

	// update values that is needed to draw
	void Update(float dt);\

	// General draw call
	void Draw();

	// Draw 3d meshes
	void Draw_Meshes(GLuint vaoid, unsigned int instance_count,
		unsigned int prim_count, GLenum prim_type, LightProperties LightSource);
		//glm::vec4 Albe, glm::vec4 Spec, glm::vec4 Diff, glm::vec4 Ambi, float Shin);
		//Materials Mat);

	// Draw debug lines
	void Draw_Debug(GLuint vaoid, unsigned int instance_count);

	// Draw grids at Y = 0
	void Draw_Grid(GLuint vaoid, unsigned int instance_count);

	// Set up grids for editor
	void SetupGrid(int gridamt);

	void Exit();

	// function for raycasting
	bool Raycasting(Ray3D& _ray);
};
#endif // !GRAPHICS_SYSTEM_H