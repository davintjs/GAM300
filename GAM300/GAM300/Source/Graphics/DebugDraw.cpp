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
#include "Texture/TextureManager.h"

void LookAtRotationMatrix(glm::mat4& _rotMat, const glm::vec3& _objPos, const glm::vec3& _tarPos, const glm::vec3& _up, const glm::vec3& _right)
{
	// Calculate the forward vector (the direction the object is currently facing)
	const glm::vec3 forward = glm::normalize(_tarPos - _objPos);

	// Calculate the new up vector
	glm::vec3 up = glm::cross(_right, forward);

	_rotMat[0] = glm::vec4(_right, 0.0f);
	_rotMat[1] = glm::vec4(up, 0.0f);
	_rotMat[2] = glm::vec4(-forward, 0.0f);
}

void DebugDraw::Init()
{
	// Euan RayCasting Testing
	raycastLine = new RaycastLine;
	raycastLine->lineinit();
	pProp = nullptr;

	cameraID = TextureManager.CreateTexture("GAM300/Data/Icons/Camera.dds");
	lightID = TextureManager.CreateTexture("GAM300/Data/Icons/Light.dds");
	particleID = TextureManager.CreateTexture("GAM300/Data/Icons/Particle.dds");
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
	InstanceProperties& iProp = instanceCounter[vao];
	pProp = &iProp;
	glm::vec4 color = { 0.3f, 0.3f, 0.3f, 1.f };

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

	if (showAllColliders)
		DrawCapsuleColliders();
	
	
	glLineWidth(1.5f);
	// Actual Debug Drawing
	{
		GLSLShader& shader = SHADER.GetShader(SHADERTYPE::TDR);
		shader.Use();

		// UNIFORM VARIABLES ----------------------------------------
		// Persp Projection
		GLint uniform1 = glGetUniformLocation(shader.GetHandle(), "persp_projection");
		GLint uniform2 = glGetUniformLocation(shader.GetHandle(), "View");
		//GLint uniform3 = glGetUniformLocation(shader.GetHandle(), "uColor");
		glUniformMatrix4fv(uniform1, 1, GL_FALSE, glm::value_ptr(EditorCam.GetProjMatrix()));
		glUniformMatrix4fv(uniform2, 1, GL_FALSE, glm::value_ptr(EditorCam.GetViewMatrix()));

		glBindBuffer(GL_ARRAY_BUFFER, iProp.entitySRTbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (iProp.iter) * sizeof(glm::mat4), iProp.entitySRT.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, iProp.AlbedoBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (iProp.iter) * sizeof(glm::vec4), iProp.Albedo.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(iProp.VAO);
		glDrawElementsInstanced(iProp.drawType, 2, GL_UNSIGNED_INT, 0, iProp.iter);
		glBindVertexArray(0);

		/*for (size_t i = 0; i < iProp.iter; i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, iProp.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4), &(iProp.entitySRT[i]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, iProp.AlbedoBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4), &(iProp.Albedo[i]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(iProp.VAO);
			glDrawElements(iProp.drawType, iProp.drawCount, GL_UNSIGNED_INT, 0);
		
			glUniform3fv(uniform3, 1, glm::value_ptr(glm::vec3(0.f, 0.f, 0.f)));
			glDrawElements(GL_POINTS, iProp.drawCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}*/

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

	DrawIcons();

	glLineWidth(1.f);

	ResetPhysicDebugContainer();
}

// Gizmos/icons for components
void DebugDraw::DrawIcons()
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UI_WORLD);
	shader.Use();

	GLint uniform1 = glGetUniformLocation(shader.GetHandle(), "projection");
	GLint uniform2 = glGetUniformLocation(shader.GetHandle(), "view");
	GLint uniform3 = glGetUniformLocation(shader.GetHandle(), "uColor");
	GLint uniform4 = glGetUniformLocation(shader.GetHandle(), "SRT");
	glUniform1f(glGetUniformLocation(shader.GetHandle(), "RenderSprite"), true);
	glUniform1f(glGetUniformLocation(shader.GetHandle(), "RenderIcon"), true);
	glUniformMatrix4fv(uniform1, 1, GL_FALSE, glm::value_ptr(EditorCam.GetProjMatrix()));
	glUniformMatrix4fv(uniform2, 1, GL_FALSE, glm::value_ptr(EditorCam.GetViewMatrix()));

	glm::vec4 color = { 0.3f, 0.3f, 0.3f, 1.f };
	glm::mat4 transform, rotation, scale;
	const glm::vec3 up = EditorCam.GetUpVec(), right = EditorCam.GetRightVec(), cameraPosition = EditorCam.GetCameraPosition();
	rotation = glm::identity<glm::mat4>();
	scale = glm::scale(glm::mat4(1.f), glm::vec3(0.3f, 0.3f, 1.f));
	color = glm::vec4(1.f);

	for (Camera& camera : currentScene.GetArray<Camera>())
	{
		if (camera.state == DELETED || !currentScene.IsActive(camera)) continue;

		Transform& t = currentScene.Get<Transform>(camera);
		LookAtRotationMatrix(rotation, t.GetGlobalTranslation(), cameraPosition, up, right);
		transform = glm::translate(glm::mat4(1.f), t.GetGlobalTranslation()) * rotation * scale;
		glUniformMatrix4fv(uniform4, 1, GL_FALSE, glm::value_ptr(transform));
		glUniform4fv(uniform3, 1, glm::value_ptr(color));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cameraID);

		renderQuad(vaoIcon, vboIcon);
	}

	color = glm::vec4(1.f, 1.f, 0.f, 1.f);
	for (LightSource& lightSource : currentScene.GetArray<LightSource>())
	{
		if (!currentScene.IsActive(lightSource)) continue;

		Transform& t = currentScene.Get<Transform>(lightSource);

		LookAtRotationMatrix(rotation, t.GetGlobalTranslation(), cameraPosition, up, right);
		transform = glm::translate(glm::mat4(1.f), t.GetGlobalTranslation()) * rotation * scale;
		glUniformMatrix4fv(uniform4, 1, GL_FALSE, glm::value_ptr(transform));
		glUniform4fv(uniform3, 1, glm::value_ptr(color));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lightID);

		renderQuad(vaoIcon, vboIcon);
	}

	glUniform1f(glGetUniformLocation(shader.GetHandle(), "RenderIcon"), false);
	shader.UnUse();
	glDisable(GL_BLEND);
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

void DebugDraw::DrawCapsuleColliders()
{
	if (!pProp)
		return;

	Scene& scene = MySceneManager.GetCurrentScene();
	auto& iProp = *pProp;
	glm::vec4 color = { 0.f, 1.f, 0.f, 1.f };

	for (CapsuleCollider& capsuleCollider : scene.GetArray<CapsuleCollider>())
	{
		if (capsuleCollider.state == DELETED) continue;
		if (!scene.IsActive(capsuleCollider)) continue;

		Entity& entity = scene.Get<Entity>(capsuleCollider);

		if (!scene.IsActive(entity)) continue;

		Transform& t = scene.Get<Transform>(capsuleCollider);
		DrawCapsuleCollider(iProp, t.GetGlobalTranslation(), t.GetGlobalRotation(), color, capsuleCollider.radius, capsuleCollider.height);
	}
}

void DebugDraw::DrawCapsuleBounds(const Engine::UUID& _euid)
{
	if (!pProp)
		return;

	Scene& currentScene = MySceneManager.GetCurrentScene();
	Transform& t = currentScene.Get<Transform>(_euid);
	Entity& entity = currentScene.Get<Entity>(_euid);
	CapsuleCollider& capsuleCollider = currentScene.Get<CapsuleCollider>(_euid);
	const float pi = glm::pi<float>();
	auto& iProp = *pProp;

	glm::vec4 color = { 0.f, 1.f, 0.f, 1.f };
	DrawCapsuleCollider(iProp, t.GetGlobalTranslation(), t.GetGlobalRotation(), color, capsuleCollider.radius, capsuleCollider.height);
}

void DebugDraw::DrawCameraBounds(const Engine::UUID& _euid)
{
	if (!pProp)
		return;

	Scene& currentScene = MySceneManager.GetCurrentScene();
	Camera& camera = currentScene.Get<Camera>(_euid);
	auto& iProp = *pProp;
	glm::vec4 color = glm::vec4(0.3f, 0.3f, 0.3f, 1.f);

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

void DebugDraw::DrawLightBounds(const Engine::UUID& _euid)
{
	if (!pProp)
		return;

	Scene& currentScene = MySceneManager.GetCurrentScene();
	Transform& t = currentScene.Get<Transform>(_euid);
	LightSource& lightSource = currentScene.Get<LightSource>(_euid);
	const float pi = glm::pi<float>();
	auto& iProp = *pProp;
	const glm::vec4 color = glm::vec4(1.f, 0.95f, 0.2f, 1.0f);

	switch (lightSource.lightType)
	{
	case (int)LIGHT_TYPE::SPOT_LIGHT:
		DrawSpotLight(iProp, t.GetWorldMatrix(), color, lightSource.intensity, lightSource.inner_CutOff, lightSource.outer_CutOff);
		break;

	case (int)LIGHT_TYPE::DIRECTIONAL_LIGHT:
		DrawDirectionalLight(iProp, t.GetWorldMatrix(), color);
		break;

	case (int)LIGHT_TYPE::POINT_LIGHT:
		DrawCircle2D(iProp, t.GetGlobalTranslation(), glm::vec3(0.f, 0.f, 0.f), color, lightSource.intensity * 0.8f);
		DrawCircle2D(iProp, t.GetGlobalTranslation(), glm::vec3(pi * 0.5f, 0.f, 0.f), color, lightSource.intensity * 0.8f);
		DrawCircle2D(iProp, t.GetGlobalTranslation(), glm::vec3(0.f, 0.f, pi * 0.5f), color, lightSource.intensity * 0.8f);

		DrawCircle2D(iProp, t.GetGlobalTranslation(), glm::vec3(0.f, 0.f, 0.f), color, lightSource.intensity * 0.1f);
		DrawCircle2D(iProp, t.GetGlobalTranslation(), glm::vec3(pi * 0.5f, 0.f, 0.f), color, lightSource.intensity * 0.1f);
		DrawCircle2D(iProp, t.GetGlobalTranslation(), glm::vec3(0.f, 0.f, pi * 0.5f), color, lightSource.intensity * 0.1f);
		break;
	}
}

void DebugDraw::DrawCapsuleCollider(InstanceProperties& _iProp, const glm::vec3& _center, const glm::vec3& _rotation, const glm::vec4& _color, const float& _radius, const float& _height)
{
	glm::vec3 point1, point2;
	int vCount = 4;
	float angle = 360.0f / vCount;
	const float circleOffset = _height - _radius;
	const float pi = glm::pi<float>();
	float currentAngle, x, z;

	glm::mat4 rotMatrix = glm::eulerAngleXYZ(_rotation.x, _rotation.y, _rotation.z);
	glm::mat4 transform = glm::translate(glm::mat4(1.f), _center) * rotMatrix;

	for (int i = 0; i < vCount; i++)
	{
		currentAngle = angle * i;
		x = _radius * cos(glm::radians(currentAngle));
		z = _radius * sin(glm::radians(currentAngle));

		point1 = transform * glm::vec4(x, circleOffset, z, 1.f);
		point2 = transform * glm::vec4(x, -circleOffset, z, 1.f);

		DrawSegment3D(_iProp, point1, point2, _color);
	}

	point1 = transform * glm::vec4(0.f, circleOffset, 0.f, 1.f);
	point2 = transform * glm::vec4(0.f, -circleOffset, 0.f, 1.f);
	DrawCircle2D(_iProp, point1, _rotation, _color, _radius);
	DrawCircle2D(_iProp, point2, _rotation, _color, _radius);

	vCount = 32;
	angle = 180.f / vCount;
	float x1, x2, y1, y2;

	glm::mat4 transform1 = glm::translate(glm::mat4(1.f), point1) * rotMatrix;
	glm::mat4 transform2 = glm::translate(glm::mat4(1.f), point2) * rotMatrix;

	for (int i = 0; i < vCount; i++)
	{
		currentAngle = angle * i;
		x1 = _radius * cos(glm::radians(currentAngle));
		y1 = _radius * sin(glm::radians(currentAngle));

		currentAngle = angle * (i + 1);
		x2 = _radius * cos(glm::radians(currentAngle));
		y2 = _radius * sin(glm::radians(currentAngle));

		point1 = transform1 * glm::vec4(x1, y1, 0.f, 1.f);
		point2 = transform1 * glm::vec4(x2, y2, 0.f, 1.f);
		DrawSegment3D(_iProp, point1, point2, _color);

		point1 = transform1 * glm::vec4(0.f, y1, x1, 1.f);
		point2 = transform1 * glm::vec4(0.f, y2, x2, 1.f);
		DrawSegment3D(_iProp, point1, point2, _color);

		point1 = transform2 * glm::vec4(x1, -y1, 0.f, 1.f);
		point2 = transform2 * glm::vec4(x2, -y2, 0.f, 1.f);
		DrawSegment3D(_iProp, point1, point2, _color);

		point1 = transform2 * glm::vec4(0.f, -y1, x1, 1.f);
		point2 = transform2 * glm::vec4(0.f, -y2, x2, 1.f);
		DrawSegment3D(_iProp, point1, point2, _color);
	}
}

void DebugDraw::DrawSpotLight(InstanceProperties& _iProp, const glm::mat4& _t, const glm::vec4& _color, const float& _range, const float& _innerCutOff, const float& _outerCutOff)
{
	int vCount = 4;
	float angle = 360.0f / vCount;
	const float pi = glm::pi<float>();
	float currentAngle, x, z;
	glm::vec3 point1, point2;

	const glm::vec3 center = _t[3];

	for (int i = 0; i < vCount; i++)
	{
		currentAngle = angle * i;
		x = _range * cos(glm::radians(currentAngle));
		z = _range * sin(glm::radians(currentAngle));

		point1 = _t * glm::vec4(x, -_range, z, 1.f);

		DrawSegment3D(_iProp, center, point1, _color);
	}

	point2 = glm::vec4(0.f, -_range, 0.f, 1.f);
	DrawCircle2D(*pProp, _t, point2, _color, _range);
}

void DebugDraw::DrawDirectionalLight(InstanceProperties& _iProp, const glm::mat4& _t, const glm::vec4& _color)
{
	const int vCount = 8;
	const float angle = 360.0f / vCount;
	const float radius = 0.25f;
	float currentAngle, x, z;
	glm::vec3 point1, point2;

	for (int i = 0; i < vCount; i++)
	{
		currentAngle = angle * i;
		x = radius * cos(glm::radians(currentAngle));
		z = radius * sin(glm::radians(currentAngle));


		point1 = glm::vec3(x, 0.f, z);
		point2 = glm::vec3(x, -4 * radius, z);

		point1 = _t * glm::vec4(point1, 1.f);
		point2 = _t * glm::vec4(point2, 1.f);

		DrawSegment3D(_iProp, point1, point2, _color);
	}

	DrawCircle2D(*pProp, _t, glm::vec3(0.f, 0.f, 0.f), _color, radius);
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
	float currentAngle, x1, x2, z1, z2;
	glm::vec3 point1, point2;

	glm::mat4 transform = glm::translate(glm::mat4(1.f), _center) * glm::eulerAngleXYZ(_rotation.x, _rotation.y, _rotation.z);


	for (int i = 0; i < vCount; i++)
	{
		currentAngle = angle * i;
		x1 = _radius * cos(glm::radians(currentAngle));
		z1 = _radius * sin(glm::radians(currentAngle));

		if (i + 1 >= vCount)
			currentAngle = 0.f;
		else
			currentAngle = angle * (i + 1);
		x2 = _radius * cos(glm::radians(currentAngle));
		z2 = _radius * sin(glm::radians(currentAngle));

		point1 = glm::vec3(x1, 0.f, z1);
		point2 = glm::vec3(x2, 0.f, z2);

		point1 = transform * glm::vec4(point1, 1.f);
		point2 = transform * glm::vec4(point2, 1.f);

		DrawSegment3D(_iProp, point1, point2, _color);
	}
}

void DebugDraw::DrawCircle2D(InstanceProperties& _iProp, const glm::mat4& _t, const glm::vec3& _center, const glm::vec4& _color, const float& _radius)
{
	const int vCount = 64;
	const float angle = 360.0f / vCount;
	float currentAngle, x1, x2, z1, z2;
	glm::vec3 point1, point2;

	for (int i = 0; i < vCount; i++)
	{
		currentAngle = angle * i;
		x1 = _radius * cos(glm::radians(currentAngle));
		z1 = _radius * sin(glm::radians(currentAngle));

		if (i + 1 >= vCount)
			currentAngle = 0.f;
		else
			currentAngle = angle * (i + 1);
		x2 = _radius * cos(glm::radians(currentAngle));
		z2 = _radius * sin(glm::radians(currentAngle));

		point1 = _center + glm::vec3(x1, 0.f, z1);
		point2 = _center + glm::vec3(x2, 0.f, z2);

		point1 = _t * glm::vec4(point1, 1.f);
		point2 = _t * glm::vec4(point2, 1.f);

		DrawSegment3D(_iProp, point1, point2, _color);
	}
}

void DebugDraw::DrawSemiCircle2D(InstanceProperties& _iProp, const glm::vec3& _center, const glm::vec3& _rotation, const glm::vec4& _color, const float& _radius)
{
	const int vCount = 32;
	const float angle = 180.f / vCount;
	float currentAngle, x1, x2, z1, z2;
	glm::vec3 point1, point2;

	glm::mat4 transform = glm::translate(glm::mat4(1.f), _center) * glm::eulerAngleXYZ(_rotation.x, _rotation.y, _rotation.z);

	for (int i = 0; i < vCount; i++)
	{
		currentAngle = angle * i;
		x1 = _radius * cos(glm::radians(currentAngle));
		z1 = _radius * sin(glm::radians(currentAngle));

		currentAngle = angle * (i + 1);
		x2 = _radius * cos(glm::radians(currentAngle));
		z2 = _radius * sin(glm::radians(currentAngle));

		point1 = transform * glm::vec4(x1, 0.f, z1, 1.f);
		point2 = transform * glm::vec4(x2, 0.f, z2, 1.f);
		DrawSegment3D(_iProp, point1, point2, _color);
	}
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
