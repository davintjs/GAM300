/*!***************************************************************************************
\file			DebugDraw.cpp
\project
\author         

\par			Course: GAM300
\date           10/10/2023

\brief
	This file contains the definitions of Debug Draw that includes:
	1.

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include "Core/EventsManager.h"
#include "Editor/EditorHeaders.h"
#include "Editor/EditorCamera.h"
#include "MESHMANAGER.h"
#include "AI/NavMeshBuilder.h"
#include "AI/NavMesh.h"
#include "Scene/SceneManager.h"

void DebugDraw::Init()
{
	// Euan RayCasting Testing
	raycastLine = new RaycastLine;
	raycastLine->lineinit();
}

void DebugDraw::Update(float)
{
	if (enableDebugDraw)
		DrawBoxColliders();

	if (enableRay)
		DrawRay();
}

void DebugDraw::Draw()
{
	// Must be at the top of the draw function
	GLuint vao = MESHMANAGER.vaoMap[ASSET_SEG3D];
	size_t s = static_cast<int>(SHADERTYPE::TDR);
	if (RENDERER.GetInstanceContainer()[s].find(vao) == RENDERER.GetInstanceContainer()[s].cend()) { // if container does not have this vao, emplace
		RENDERER.GetInstanceContainer()[s].emplace(std::pair(vao, RENDERER.GetInstanceProperties()[vao]));
	}

	Scene& currentScene = MySceneManager.GetCurrentScene();
	glm::vec4 color = { 0.3f, 0.3f, 0.3f, 1.f };

	for (Camera& camera : currentScene.GetArray<Camera>())
	{
		if (camera.state == DELETED || !currentScene.IsActive(camera)) continue;

		const glm::vec3 frontMultFar = camera.GetFarClip() * camera.GetForwardVec();
		const glm::vec3 frontMultNear = camera.GetNearClip() * camera.GetForwardVec();
		const glm::vec3 farCenter = camera.GetCameraPosition() + frontMultFar;
		const glm::vec3 nearCenter = camera.GetCameraPosition() + frontMultNear;
		
		const float farHeight = camera.GetFarClip() * tanf(glm::radians(camera.GetFOV()) * 0.5f);
		const float farWidth = farHeight * camera.GetAspect();
		const float nearHeight = camera.GetNearClip() * tanf(glm::radians(camera.GetFOV()) * 0.5f);
		const float nearWidth = nearHeight * camera.GetAspect();

		const glm::vec3 rightVec = camera.GetRightVec(), upVec = camera.GetUpVec();

		glm::vec3 farLeftTop = farCenter - rightVec * farWidth + upVec * farHeight;
		glm::vec3 farRightTop = farCenter + rightVec * farWidth + upVec * farHeight;
		glm::vec3 farLeftBottom = farCenter - rightVec * farWidth - upVec * farHeight;
		glm::vec3 farRightBottom = farCenter + rightVec * farWidth - upVec * farHeight;

		glm::vec3 nearLeftTop = nearCenter - rightVec * nearWidth + upVec * nearHeight;
		glm::vec3 nearRightTop = nearCenter + rightVec * nearWidth + upVec * nearHeight;
		glm::vec3 nearLeftBottom = nearCenter - rightVec * nearWidth - upVec * nearHeight;
		glm::vec3 nearRightBottom = nearCenter + rightVec * nearWidth - upVec * nearHeight;

		DrawSegment3D(nearLeftTop, farLeftTop, color);
		DrawSegment3D(nearRightTop, farRightTop, color);
		DrawSegment3D(nearLeftBottom, farLeftBottom, color);
		DrawSegment3D(nearRightBottom, farRightBottom, color);

		DrawSegment3D(farLeftTop, farLeftBottom, color);
		DrawSegment3D(farLeftBottom, farRightBottom, color);
		DrawSegment3D(farRightBottom, farRightTop, color);
		DrawSegment3D(farRightTop, farLeftTop, color);

		DrawSegment3D(nearLeftTop, nearLeftBottom, color);
		DrawSegment3D(nearLeftBottom, nearRightBottom, color);
		DrawSegment3D(nearRightBottom, nearRightTop, color);
		DrawSegment3D(nearRightTop, nearLeftTop, color);

		color = glm::vec4(1.f, 0.f, 0.f, 1.f);
		DrawSegment3D(camera.GetConstCameraPosition(), camera.GetConstFocalPoint(), color);
	}

	auto* navMesh = NAVMESHBUILDER.GetNavMesh();

	color = { 0.3f, 1.f, 0.3f, 1.f };
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

		for (const auto& mAgent : MySceneManager.GetCurrentScene().GetArray<NavMeshAgent>())
		{
			if (mAgent.mPoints.empty())
			{
				continue;
			}
			for (int i = 0; i < mAgent.mPoints.size() - 1; ++i)
			{
				DrawSegment3D(mAgent.mPoints[i], mAgent.mPoints[i + 1], glm::vec4(0.f, 0.f, 0.f, 1.f));
			}
		}
	}
	
	auto& prop = RENDERER.GetInstanceContainer()[s][vao];
	glLineWidth(4.f);
	glPointSize(10.f);
	// NAV MESH Draw Call
	{
		GLSLShader& shader = SHADER.GetShader(SHADERTYPE::TDR);
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

		for (size_t i = 0; i < prop.iter; i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4), &(prop.entitySRT[i]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//glBindBuffer(GL_ARRAY_BUFFER, prop.AlbedoBuffer);
			//glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.Albedo[0]));
			//glBindBuffer(GL_ARRAY_BUFFER, 0);
		
			// UNIFORM VARIABLES ----------------------------------------		
			glUniform3fv(uniform3, 1, glm::value_ptr(glm::vec3(prop.Albedo[i])));

			glBindVertexArray(prop.VAO);
			glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
		
			glUniform3fv(uniform3, 1, glm::value_ptr(glm::vec3(0.f, 0.f, 0.f)));
			glDrawElements(GL_POINTS, prop.drawCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

		}

		shader.UnUse();
	
		prop.iter = 0;

	}

	// Physic's Debug Draw
	{
		glLineWidth(5.f);

		GLSLShader& shader = SHADER.GetShader(SHADERTYPE::FORWARDDEBUG);
		shader.Use();

		// UNIFORM VARIABLES ----------------------------------------		
		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "persp_projection");
		GLint uniform2 =
			glGetUniformLocation(shader.GetHandle(), "View");
		GLint uniform3 =
			glGetUniformLocation(shader.GetHandle(), "uColor");
		GLint uniform4 =
			glGetUniformLocation(shader.GetHandle(), "SRT");

		glUniformMatrix4fv(uniform1, 1, GL_FALSE,
			glm::value_ptr(EditorCam.GetProjMatrix()));
		glUniformMatrix4fv(uniform2, 1, GL_FALSE,
			glm::value_ptr(EditorCam.GetViewMatrix()));
		glUniform3fv(uniform3, 1, glm::value_ptr(glm::vec3(0.f, 1.f, 0.f)));

		for (int i = 0; i < boxColliderContainer.size(); ++i)
		{
			RigidDebug currRD = boxColliderContainer[i];
			
			//glUniform3fv(uniform3, 1, glm::value_ptr(glm::vec3(175.f / 255.f, 225.f / 255.f, 175.f / 255.f)));


			// UNIFORM VARIABLES ----------------------------------------		
			glUniformMatrix4fv(uniform4, 1, GL_FALSE, glm::value_ptr(currRD.SRT));
	
		
			glBindVertexArray(currRD.vao);
			glDrawElements(GL_LINES, 2 * 12, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		shader.UnUse();

	}

	if(LIGHTING.pointLightCount && enableDebugDraw)
	{

		GLSLShader& shader = SHADER.GetShader(SHADERTYPE::FORWARDDEBUG);
		shader.Use();

		// UNIFORM VARIABLES ----------------------------------------		
		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "persp_projection");
		GLint uniform2 =
			glGetUniformLocation(shader.GetHandle(), "View");
		GLint uniform3 =
			glGetUniformLocation(shader.GetHandle(), "uColor");
		GLint uniform4 =
			glGetUniformLocation(shader.GetHandle(), "SRT");


		auto pointLights = LIGHTING.GetPointLights();
		for (int i = 0; i < (int)LIGHTING.pointLightCount; ++i)
		{
			// I need to make a SRT here regarding the light's stuff
			glm::mat4 translation = glm::translate(glm::mat4(1.f), pointLights[i].lightpos);
			glm::mat4 scalar = glm::scale(glm::mat4(1.f), glm::vec3(pointLights[i].intensity/10.f));
			

			glUniformMatrix4fv(uniform4, 1, GL_FALSE, glm::value_ptr(translation * scalar));
			Mesh* Sphere = MESHMANAGER.DereferencingMesh(ASSET_SPHERE);

			glBindVertexArray(Sphere->vaoID);
			glDrawElements(GL_LINES, Sphere->drawCounts, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

		}
		shader.UnUse();

	}
	glLineWidth(1.f);

	ResetPhysicDebugContainer();
}

void DebugDraw::DrawBoxColliders()
{
	Scene& scene = MySceneManager.GetCurrentScene();
	for (BoxCollider& bc : scene.GetArray<BoxCollider>())
	{
		if (bc.state == DELETED) continue;
		if (!scene.IsActive(bc)) continue;

		Entity& entity = scene.Get<Entity>(bc);

		if (!scene.IsActive(entity)) continue;

		Transform& t = scene.Get<Transform>(bc);
		RigidDebug currRigidDebug;

		currRigidDebug.vao = MESHMANAGER.offsetAndBoundContainer[ASSET_CUBE].vao;

		glm::mat4 SRT = t.GetWorldMatrix();

		glm::mat4 scalarMat = glm::scale(glm::mat4(1.f), glm::vec3(bc.dimensions));
		glm::mat4 transMat = glm::translate(glm::mat4(1.f), glm::vec3(bc.offset));
		SRT *= transMat * scalarMat;

		currRigidDebug.SRT = SRT;

		DEBUGDRAW.AddBoxColliderDraw(currRigidDebug);
	}
}

void DebugDraw::DrawSegment3D(const Segment3D& _segment3D, const glm::vec4& _color)
{
	if (MESHMANAGER.vaoMap.find(ASSET_SEG3D) == MESHMANAGER.vaoMap.end())
	{
		E_ASSERT(false, "ERROR: Mesh vao does not exist!");
	}

	auto& prop = RENDERER.GetInstanceContainer()[static_cast<int>(SHADERTYPE::TDR)][MESHMANAGER.vaoMap[ASSET_SEG3D]];

	glm::vec3 direction = _segment3D.point2 - _segment3D.point1;
	prop.entitySRT.emplace_back(glm::mat4
	{
		glm::vec4(direction.x, 0.f, 0.f, 0.f),
		glm::vec4(0.f, direction.y, 0.f, 0.f),
		glm::vec4(0.f, 0.f, direction.z, 0.f),
		glm::vec4(_segment3D.point1.x, _segment3D.point1.y, _segment3D.point1.z, 1.f)
	});

	// Set color of segment
	prop.Albedo.emplace_back(_color);

	prop.iter++; // Increase instance count
}

void DebugDraw::DrawSegment3D(const glm::vec3& _point1, const glm::vec3& _point2, const glm::vec4& _color)
{
	if (MESHMANAGER.vaoMap.find(ASSET_SEG3D) == MESHMANAGER.vaoMap.end())
	{
		E_ASSERT(false, "ERROR: Mesh vao does not exist!");
	}

	auto& prop = RENDERER.GetInstanceContainer()[static_cast<int>(SHADERTYPE::TDR)][MESHMANAGER.vaoMap[ASSET_SEG3D]];

	glm::vec3 direction = _point2 - _point1;
	prop.entitySRT.emplace_back(glm::mat4
	{
		glm::vec4(direction.x, 0.f, 0.f, 0.f),
		glm::vec4(0.f, direction.y, 0.f, 0.f),
		glm::vec4(0.f, 0.f, direction.z, 0.f),
		glm::vec4(_point1.x, _point1.y, _point1.z, 1.f)
	});

	// Set color of segment
	prop.Albedo.emplace_back(_color);

	prop.iter++; // Increase instance count
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

void DebugDraw::LoopAndGetRigidBodies()
{

}



void DebugDraw::AddBoxColliderDraw(const RigidDebug& rigidDebugDraw)
{
	boxColliderContainer.emplace_back(rigidDebugDraw);
}

void DebugDraw::ResetPhysicDebugContainer()
{
	boxColliderContainer.clear();
}
