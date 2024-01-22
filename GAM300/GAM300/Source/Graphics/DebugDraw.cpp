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

#include <glm/gtx/euler_angles.hpp>

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
	if (showAllColliders)
		DrawBoxColliders();

	if (enableRay)
		DrawRay();
}

void DebugDraw::Draw()
{
	if (!enableDebugDraw)
		return;

	// Must be at the top of the draw function
	GLuint vao = MESHMANAGER.vaoMap[ASSET_SEG3D];
	size_t s = static_cast<int>(SHADERTYPE::TDR);
	InstanceContainer& instanceCounter = RENDERER.GetInstanceContainer()[s];
	if (instanceCounter.find(vao) == instanceCounter.cend()) { // if container does not have this vao, emplace
		instanceCounter.emplace(vao, RENDERER.GetInstanceProperties()[vao]);
		instanceCounter[vao].ResizeContainers();
	}

	Scene& currentScene = MySceneManager.GetCurrentScene();
	auto& iProp = instanceCounter[vao];
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

		DrawSegment3D(iProp, nearLeftTop, farLeftTop, color);
		DrawSegment3D(iProp, nearRightTop, farRightTop, color);
		DrawSegment3D(iProp, nearLeftBottom, farLeftBottom, color);
		DrawSegment3D(iProp, nearRightBottom, farRightBottom, color);

		DrawSegment3D(iProp, farLeftTop, farLeftBottom, color);
		DrawSegment3D(iProp, farLeftBottom, farRightBottom, color);
		DrawSegment3D(iProp, farRightBottom, farRightTop, color);
		DrawSegment3D(iProp, farRightTop, farLeftTop, color);

		DrawSegment3D(iProp, nearLeftTop, nearLeftBottom, color);
		DrawSegment3D(iProp, nearLeftBottom, nearRightBottom, color);
		DrawSegment3D(iProp, nearRightBottom, nearRightTop, color);
		DrawSegment3D(iProp, nearRightTop, nearLeftTop, color);

		color = glm::vec4(1.f, 0.f, 0.f, 1.f);
		DrawSegment3D(iProp, camera.GetConstCameraPosition(), camera.GetConstFocalPoint(), color);
	}

	{
		for (LightSource& lightSource : currentScene.GetArray<LightSource>())
		{
			Transform& t = currentScene.Get<Transform>(lightSource);

			switch (lightSource.lightType)
			{
			case (int)LIGHT_TYPE::SPOT_LIGHT:

				break;

			case (int)LIGHT_TYPE::DIRECTIONAL_LIGHT:

				break;

			case (int)LIGHT_TYPE::POINT_LIGHT:
				DrawCircle2D(iProp, t.GetGlobalTranslation(), t.GetGlobalRotation(), color, lightSource.intensity);
				break;
			}
		}
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
			DrawSegment3D(iProp, tri[0], tri[1], color);
			DrawSegment3D(iProp, tri[1], tri[2], color);
			DrawSegment3D(iProp, tri[0], tri[2], color);
		}

		for (const auto& mAgent : MySceneManager.GetCurrentScene().GetArray<NavMeshAgent>())
		{
			if (mAgent.mPoints.empty())
			{
				continue;
			}
			for (int i = 0; i < mAgent.mPoints.size() - 1; ++i)
			{
				DrawSegment3D(iProp, mAgent.mPoints[i], mAgent.mPoints[i + 1], glm::vec4(0.f, 0.f, 0.f, 1.f));
			}
		}
	}
	
	
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

		for (size_t i = 0; i < iProp.iter; i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, iProp.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4), &(iProp.entitySRT[i]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//glBindBuffer(GL_ARRAY_BUFFER, prop.AlbedoBuffer);
			//glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.Albedo[0]));
			//glBindBuffer(GL_ARRAY_BUFFER, 0);
		
			// UNIFORM VARIABLES ----------------------------------------		
			glUniform3fv(uniform3, 1, glm::value_ptr(glm::vec3(iProp.Albedo[i])));

			glBindVertexArray(iProp.VAO);
			glDrawElements(iProp.drawType, iProp.drawCount, GL_UNSIGNED_INT, 0);
		
			glUniform3fv(uniform3, 1, glm::value_ptr(glm::vec3(0.f, 0.f, 0.f)));
			glDrawElements(GL_POINTS, iProp.drawCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

		}

		shader.UnUse();
	
		iProp.iter = 0;

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

	if(LIGHTING.pointLightCount)
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

		glm::mat4 scalarMat = glm::scale(glm::mat4(1.f), glm::vec3(bc.dimensions));
		glm::mat4 transMat = glm::translate(glm::mat4(1.f), glm::vec3(bc.offset));

		currRigidDebug.SRT = t.GetWorldMatrix() * transMat * scalarMat;

		DEBUGDRAW.AddBoxColliderDraw(currRigidDebug);
	}
}

void DebugDraw::DrawSegment3D(InstanceProperties& _iProp, const Segment3D& _segment3D, const glm::vec4& _color)
{
	if (MESHMANAGER.vaoMap.find(ASSET_SEG3D) == MESHMANAGER.vaoMap.end())
	{
		E_ASSERT(false, "ERROR: Mesh vao does not exist!");
	}

	if (_iProp.iter >= _iProp.maxSize)
		_iProp.ResizeContainers();

	glm::vec3 direction = _segment3D.point2 - _segment3D.point1;
	/*prop.entitySRT.emplace_back(glm::mat4
	{
		glm::vec4(direction.x, 0.f, 0.f, 0.f),
		glm::vec4(0.f, direction.y, 0.f, 0.f),
		glm::vec4(0.f, 0.f, direction.z, 0.f),
		glm::vec4(_segment3D.point1.x, _segment3D.point1.y, _segment3D.point1.z, 1.f)
	});*/
	_iProp.entitySRT[_iProp.iter] = glm::mat4
	{
		glm::vec4(direction.x, 0.f, 0.f, 0.f),
		glm::vec4(0.f, direction.y, 0.f, 0.f),
		glm::vec4(0.f, 0.f, direction.z, 0.f),
		glm::vec4(_segment3D.point1.x, _segment3D.point1.y, _segment3D.point1.z, 1.f)
	};

	// Set color of segment
	//prop.Albedo.emplace_back(_color);
	_iProp.Albedo[_iProp.iter] = _color;

	_iProp.iter++; // Increase instance count
}

void DebugDraw::DrawSegment3D(InstanceProperties& _iProp, const glm::vec3& _point1, const glm::vec3& _point2, const glm::vec4& _color)
{
	if (MESHMANAGER.vaoMap.find(ASSET_SEG3D) == MESHMANAGER.vaoMap.end())
	{
		E_ASSERT(false, "ERROR: Mesh vao does not exist!");
	}

	if (_iProp.iter >= _iProp.maxSize)
		_iProp.ResizeContainers();

	glm::vec3 direction = _point2 - _point1;
	/*prop.entitySRT.emplace_back(glm::mat4
	{
		glm::vec4(direction.x, 0.f, 0.f, 0.f),
		glm::vec4(0.f, direction.y, 0.f, 0.f),
		glm::vec4(0.f, 0.f, direction.z, 0.f),
		glm::vec4(_point1.x, _point1.y, _point1.z, 1.f)
	});*/
	_iProp.entitySRT[_iProp.iter] = glm::mat4
		{
			glm::vec4(direction.x, 0.f, 0.f, 0.f),
			glm::vec4(0.f, direction.y, 0.f, 0.f),
			glm::vec4(0.f, 0.f, direction.z, 0.f),
			glm::vec4(_point1.x, _point1.y, _point1.z, 1.f)
		};

	// Set color of segment
	//prop.Albedo.emplace_back(_color);
	_iProp.Albedo[_iProp.iter] = _color;

	_iProp.iter++; // Increase instance count
}

void DebugDraw::DrawCircle2D(InstanceProperties& _iProp, const glm::vec3& _center, const glm::vec3& _rotation, const glm::vec4& _color, const float& _radius)
{
	const int vCount = 64;
	const float angle = 360.0f / vCount;
	const float radius = _radius * 0.5f;
	float currentAngle, x1, x2, z1, z2;
	glm::vec3 point1, point2;

	glm::mat4 rotation = glm::eulerAngleXYZ(_rotation.x, _rotation.y, _rotation.z);

	for (int i = 0; i < vCount; i++)
	{
		currentAngle = angle * i;
		x1 = radius * cos(glm::radians(currentAngle));
		z1 = radius * sin(glm::radians(currentAngle));

		if (i + 1 >= vCount)
			currentAngle = 0.f;
		else
			currentAngle = angle * (i + 1);
		x2 = radius * cos(glm::radians(currentAngle));
		z2 = radius * sin(glm::radians(currentAngle));

		point1 = glm::vec3(x1, 0.f, z1) + _center;
		point2 = glm::vec3(x2, 0.f, z2) + _center;

		point1 = rotation * glm::vec4(point1, 1.f);
		point2 = rotation * glm::vec4(point2, 1.f);

		DrawSegment3D(_iProp, point1, point2, _color);
	}
}

void DebugDraw::DrawSemiCircle2D(InstanceProperties& _iProp, const glm::vec3& _center, const glm::vec3& _rotation, const glm::vec4& _color, const float& _radius)
{
	
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
