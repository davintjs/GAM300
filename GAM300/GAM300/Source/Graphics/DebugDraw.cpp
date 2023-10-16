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
#include "Editor/EditorCamera.h"
#include "MeshManager.h"

void DebugDraw::Init()
{
	// Euan RayCasting Testing
	raycastLine = new RaycastLine;
	raycastLine->lineinit();

	properties = &RENDERER.GetProperties();
}

void DebugDraw::Update(float)
{
	DrawSegment3D(glm::vec3(0.f, 0.f, 0.f), glm::vec3(100.f, 100.f, 100.f), glm::vec4(1.f, 1.f, 1.f, 1.f));

	if (enableRay)
		DrawRay();

	Draw();
}

void DebugDraw::Draw()
{
	auto& prop = (*properties)["Segment3D"];
	glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, prop.AlbedoBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.Albedo[0]));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLSLShader& shader = SHADER.GetShader(TDR);
	shader.Use();
	// UNIFORM VARIABLES ----------------------------------------
	// Persp Projection
	GLint uniform1 =
		glGetUniformLocation(shader.GetHandle(), "persp_projection");
	GLint uniform2 =
		glGetUniformLocation(shader.GetHandle(), "View");
	GLint uniform3 =
		glGetUniformLocation(shader.GetHandle(), "uColor");
	glUniformMatrix4fv(uniform1, 1, GL_FALSE,
		glm::value_ptr(EditorCam.GetProjMatrix()));
	glUniformMatrix4fv(uniform2, 1, GL_FALSE,
		glm::value_ptr(EditorCam.GetViewMatrix()));
	glUniform3fv(uniform3, 1, glm::value_ptr(glm::vec3(prop.Albedo[0]))); // Bean: This should be something that iterates

	glBindVertexArray(prop.VAO);
	glLineWidth(10.f);
	glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);

	// unbind and free stuff
	glBindVertexArray(0);
	shader.UnUse();
	glLineWidth(1.f);
	prop.iter = 0;
}

void DebugDraw::DrawSegment3D(const Segment3D& _segment3D, const glm::vec4& _color)
{
	auto& prop = (*properties)["Segment3D"];

	// Set reference to property iterator
	unsigned int& i = prop.iter;

	prop.entitySRT[i] = glm::mat4(glm::translate(glm::mat4(1.f), _segment3D.point1));
	prop.entitySRT[i + 1] = glm::mat4(glm::translate(glm::mat4(1.f), _segment3D.point2));

	// Set color of segment
	prop.Albedo[i] = _color;
	prop.Albedo[i + 1] = _color;
	
	i += 2; // Increment by 2 because a line has 2 vertices
}

void DebugDraw::DrawSegment3D(const glm::vec3& _point1, const glm::vec3& _point2, const glm::vec4& _color)
{
	auto& prop = (*properties)["Segment3D"];

	// Set reference to property iterator
	unsigned int& i = prop.iter;

	prop.entitySRT[i] = glm::mat4(glm::translate(glm::mat4(1.f), _point1));
	prop.entitySRT[i + 1] = glm::mat4(glm::translate(glm::mat4(1.f), _point2));

	// Set color of segment
	prop.Albedo[i] = _color;
	prop.Albedo[i + 1] = _color;

	i += 2; // Increment by 2 because a line has 2 vertices
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