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

// Bean: Should not be here and instead be in the rendering system
#include "BaseCamera.h"

#define GRAPHICS GraphicsSystem::Instance()

ENGINE_SYSTEM(GraphicsSystem)
{
public:
	// Initialize graphics system
	void Init();

	// update values that is needed to draw
	void Update(float dt);

	// General draw call
	void Draw(BaseCamera& _camera);
	
	// 2D UI draw call
	void Draw_Screen(BaseCamera& _camera);

	void Exit();

private:
	std::vector<ISystem*> graphicSystems;
};
#endif // !GRAPHICS_SYSTEM_H