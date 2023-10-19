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
#include "AI/NavMeshBuilder.h"
#include "AI/NavMesh.h"

void DebugDraw::Init()
{
	// Euan RayCasting Testing
	raycastLine = new RaycastLine;
	raycastLine->lineinit();

	properties = &RENDERER.GetProperties();
}

void DebugDraw::Update(float)
{
	if (enableRay)
		DrawRay();
}

void DebugDraw::Draw()
{
	auto* navMesh = NAVMESHBUILDER.GetNavMesh();

	glm::vec4 color = { 0.3f, 1.f, 0.3f, 1.f };
	if (navMesh)
	{
		for (Triangle3D triangle : navMesh->GetNavMeshTriangles())
		{
			auto tri = triangle;
			tri[0].y += 0.1f;
			tri[1].y += 0.1f;
			tri[2].y += 0.1f;
			DrawSegment3D(tri[0], tri[1], color);
			DrawSegment3D(tri[1], tri[2], color);
			DrawSegment3D(tri[0], tri[2], color);
		}
	}

	auto& prop = (*properties)["Segment3D"];
	glLineWidth(4.f);
	glPointSize(10.f);

	for (size_t i = 0; i < prop.iter; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4), &(prop.entitySRT[i]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//glBindBuffer(GL_ARRAY_BUFFER, prop.AlbedoBuffer);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.Albedo[0]));
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

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
		glUniform3fv(uniform3, 1, glm::value_ptr(glm::vec3(prop.Albedo[i])));

		glBindVertexArray(prop.VAO);
		glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
		
		glUniform3fv(uniform3, 1, glm::value_ptr(glm::vec3(0.f, 0.f, 0.f)));
		glDrawElements(GL_POINTS, prop.drawCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		shader.UnUse();
	}

	glLineWidth(1.f);
	prop.iter = 0;
}

void DebugDraw::DrawSegment3D(const Segment3D& _segment3D, const glm::vec4& _color)
{
	auto& prop = (*properties)["Segment3D"];

	// Set reference to property iterator
	unsigned int& i = prop.iter;

	glm::vec3 direction = _segment3D.point2 - _segment3D.point1;
	prop.entitySRT[i] = glm::mat4
	{
		glm::vec4(direction.x, 0.f, 0.f, 0.f),
		glm::vec4(0.f, direction.y, 0.f, 0.f),
		glm::vec4(0.f, 0.f, direction.z, 0.f),
		glm::vec4(_segment3D.point1.x, _segment3D.point1.y, _segment3D.point1.z, 1.f)
	};

	// Set color of segment
	prop.Albedo[i] = _color;

	i++; // Increase instance count
}

void DebugDraw::DrawSegment3D(const glm::vec3& _point1, const glm::vec3& _point2, const glm::vec4& _color)
{
	auto& prop = (*properties)["Segment3D"];

	// Set reference to property iterator
	unsigned int& i = prop.iter;

	glm::vec3 direction = _point2 - _point1;
	prop.entitySRT[i] = glm::mat4
	{
		glm::vec4(direction.x, 0.f, 0.f, 0.f),
		glm::vec4(0.f, direction.y, 0.f, 0.f),
		glm::vec4(0.f, 0.f, direction.z, 0.f),
		glm::vec4(_point1.x, _point1.y, _point1.z, 1.f)
	};

	// Set color of segment
	prop.Albedo[i] = _color;

	i++; // Increase instance count
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