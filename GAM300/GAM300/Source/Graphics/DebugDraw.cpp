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

	properties = &RENDERER.GetInstanceProperties();
}

void DebugDraw::Update(float)
{
	ResetPhysicDebugContainer();

	Scene& scene = MySceneManager.GetCurrentScene();

	auto& rbArray = scene.GetArray<Rigidbody>();

	auto& bcArray = scene.GetArray<BoxCollider>();

	for (auto it = bcArray.begin(); it != bcArray.end(); ++it)
	{
		BoxCollider& bc = *it;

		if (bc.state == DELETED) continue;

		Transform& t = scene.Get<Transform>(bc);
		Entity& entity = scene.Get<Entity>(bc);


		/*geometryDebugData temp;
		if (scene.Has<MeshRenderer>(entity))
		{
			MeshRenderer& mr = scene.Get<MeshRenderer>(entity);

			temp = MeshManager.offsetAndBoundContainer.find(mr.meshID)->second;
		}
		else
		{
			temp = MeshManager.offsetAndBoundContainer.find(DEFAULT_MESH)->second;
		}*/

		/*if (scene.Has<MeshRenderer>(bc))
		{

		}
		else
		{

		}
		MeshRenderer& renderer = scene.Get<MeshRenderer>(bc);

		if (MeshManager.vaoMap.find(renderer.meshID) == MeshManager.vaoMap.end())
		{
			std::cout << "hit\n";
			continue;
		}*/

		//GLuint vao = MeshManager.vaoMap[renderer.meshID];
		GLuint vao = MESHMANAGER.vaoMap[DEFAULT_MESH];
		InstanceProperties& temporary = MESHMANAGER.instanceProperties->find(vao)->second;

		RigidDebug currRigidDebug;

		currRigidDebug.vao = temporary.debugVAO;
		glm::mat4 SRT = t.GetWorldMatrix();
		//glm::mat4 scalarMat = glm::scale(glm::mat4(1.f), glm::vec3(bc.x, bc.y, bc.z));

		//SRT *= scalarMat;

		//std::cout << "scalar vals : " << temp.scalarBound.x << "\n";
		//std::cout << "scalar offset : " << temp.offset.x << "\n";

		//glm::mat4 scalarMat = glm::scale(glm::mat4(1.f), temp.scalarBound);
		//glm::mat4 transMat = glm::translate(glm::mat4(1.f), temp.offset);

		glm::mat4 scalarMat = glm::scale(glm::mat4(1.f), glm::vec3(bc.x, bc.y, bc.z));
		glm::mat4 transMat = glm::translate(glm::mat4(1.f), glm::vec3(bc.offset));

		SRT *= transMat * scalarMat;

		currRigidDebug.SRT = SRT;
		//currRigidDebug.RigidScalar = glm::vec3(bc.x, bc.y, bc.z);

		DEBUGDRAW.AddBoxColliderDraw(currRigidDebug);
	}




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
	;
	auto& prop = (*properties)[MESHMANAGER.vaoMap[DEFAULT_ASSETS["Segment3D.geom"]]];
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
	
	glLineWidth(1.f);
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


		for (int i = 0; i < boxColliderContainer.size(); ++i)
		{
			RigidDebug currRD = boxColliderContainer[i];
			
			glUniformMatrix4fv(uniform1, 1, GL_FALSE,
				glm::value_ptr(EditorCam.GetProjMatrix()));
			glUniformMatrix4fv(uniform2, 1, GL_FALSE,
				glm::value_ptr(EditorCam.GetViewMatrix()));
			glUniform3fv(uniform3, 1, glm::value_ptr(glm::vec3(0.f, 1.f, 0.f)));
			//glUniform3fv(uniform3, 1, glm::value_ptr(glm::vec3(175.f / 255.f, 225.f / 255.f, 175.f / 255.f)));


			// UNIFORM VARIABLES ----------------------------------------		
			glUniformMatrix4fv(uniform4, 1, GL_FALSE, glm::value_ptr(currRD.SRT));
	
		
			glBindVertexArray(currRD.vao);
			glDrawElements(GL_LINES, 2 * 12, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		shader.UnUse();

	}
	glLineWidth(1.f);

}

void DebugDraw::DrawSegment3D(const Segment3D& _segment3D, const glm::vec4& _color)
{
	//auto& prop = (*properties)[DEFAULT_ASSETS["Segment3D.geom"]];
	auto& prop = (*properties)[MESHMANAGER.vaoMap[DEFAULT_ASSETS["Segment3D.geom"]]];

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
	auto& prop = (*properties)[MESHMANAGER.vaoMap[DEFAULT_ASSETS["Segment3D.geom"]]];

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

void DebugDraw::LoopAndGetRigidBodies()
{

}



void DebugDraw::AddBoxColliderDraw(RigidDebug rigidDebugDraw)
{
	boxColliderContainer.emplace_back(rigidDebugDraw);
}

void DebugDraw::ResetPhysicDebugContainer()
{
	boxColliderContainer.clear();
}
