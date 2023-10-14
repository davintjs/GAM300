/*!***************************************************************************************
\file			DebugDraw.cpp
\project
\author         

\par			Course: GAM300
\date           10/10/2023

\brief
	This file contains the definitions of Debug Draw that includes:
	1.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include "Core/EventsManager.h"
#include "Editor/EditorHeaders.h"
#include "Editor_Camera.h"
#include "Model3d.h"

void DebugDraw::Init()
{
	// Euan RayCasting Testing
	raycastLine = new RaycastLine;
	raycastLine->lineinit();
}

void DebugDraw::Update(float)
{
	intersected = FLT_MAX;
	tempIntersect = 0.f;

	if (enableRay)
		DrawRay();
}

void DebugDraw::DrawRay()
{
	// This is to render the Rays
	if (rayContainer.size() > 0)
	{
		for (int i = 0; i < rayContainer.size(); ++i)
		{
			Ray3D ray = rayContainer[i];

			//std::cout << "ray " << ray.origin.x << "\n";
			//std::cout << "ray direc" << ray.direction.x << "\n";

			glm::mat4 SRT
			{
				glm::vec4(ray.direction.x * 1000000.f, 0.f , 0.f , 0.f),
				glm::vec4(0.f, ray.direction.y * 1000000.f, 0.f , 0.f),
				glm::vec4(0.f , 0.f , ray.direction.z * 1000000.f , 0.f),
				glm::vec4(ray.origin.x, ray.origin.y, ray.origin.z,1.f)
			};
			//std::cout << "in here draw\n";
			raycastLine->debugline_draw(SRT);
		}
	}
}

void DebugDraw::Exit()
{
	delete raycastLine;
}