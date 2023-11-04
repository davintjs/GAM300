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

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "Core/SystemInterface.h"
#include "GraphicStructsAndClass.h"
// Bean: Should not be here and instead be in the rendering system
#include "BaseCamera.h"

#define GRAPHICS GraphicsSystem::Instance()

ENGINE_SYSTEM(GraphicsSystem)
{
public:
	// Initialize graphics system
	void Init();

	// Update values that is needed to draw
	void Update(float dt);

	// Bunch of comment code in the update loop
	void OldUpdate();

	// Bind buffers, textures and use shaders
	void PreDraw(BaseCamera& _camera, unsigned int& _vao, unsigned int& _vbo);

	// General draw call
	void Draw(BaseCamera& _camera);
	
	// 2D UI draw call
	void Draw_Screen(BaseCamera& _camera);

	// Reset buffers and instance properities
	void PostDraw();

	void Exit();

	std::vector<temp_instance> temporary_presets;

private:
	std::vector<ISystem*> graphicSystems;
};
#endif // !GRAPHICS_SYSTEM_H