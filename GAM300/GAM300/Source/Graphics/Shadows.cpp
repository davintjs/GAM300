/*!***************************************************************************************
\file			Shadows.cpp
\project
\author

\par			Course: GAM300
\date           11/10/2023

\brief
	This file contains the definitions of Graphics Shadows that includes:
	1.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include "Scene/SceneManager.h"


void Shadows::Init()
{
	FRAMEBUFFER.CreateDirectionalAndSpotLight(depthMapFBO, depthMap, SHADOW_WIDTH_DIRECTIONAL, SHADOW_HEIGHT_DIRECTIONAL);

	FRAMEBUFFER.CreateDirectionalAndSpotLight(depthMapFBO_S, depthMap_S, SHADOW_WIDTH, SHADOW_HEIGHT);

	FRAMEBUFFER.CreatePointLight(depthCubemapFBO, depthCubemap, SHADOW_WIDTH, SHADOW_HEIGHT);
}

void Shadows::Update(float dt)
{
#if defined(_BUILD)
	if (RENDERER.enableShadows())
	{
		DrawDepthSpot();
		DrawDepthDirectional();
		DrawDepthPoint();
	}
#else
	// This helps performance by only updating certains light type per frame
	static int delay = 0;
	if (RENDERER.enableShadows())
	{
		if (delay == 0)
		{
			DrawDepthSpot();
		}
		else if (delay == 1)
		{
			DrawDepthDirectional();
		}
		else if (delay == 2)
		{
			DrawDepthPoint();
		}

		delay = (delay > 1) ? 0 : ++delay;
	}
#endif
	
}

void Shadows::Exit()
{

}

void Shadows::DrawDepthSpot()
{
	unsigned index = 0;
	for (int i = 0; i < (int)LIGHTING.spotLightCount; ++i)
	{
		LightProperties& spot_light_stuffs = LIGHTING.GetSpotLights()[i];
		if (!spot_light_stuffs.enableShadow)
		{
			continue;
		}
		glEnable(GL_DEPTH_TEST);
		glm::mat4 lightProjection, lightView;

		lightProjection = glm::perspective<float>(glm::radians(90.f), 1.f, 0.1f, 100.f);
		lightView = glm::lookAt(spot_light_stuffs.lightpos, spot_light_stuffs.lightpos +
			(spot_light_stuffs.direction * 100.f), glm::vec3(0.0, 0.0, 1.0));

		LIGHTING.GetSpotLights()[i].lightSpaceMatrix = lightProjection * lightView;
		//lightSpaceMatrix_spot = lightProjection * lightView;

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, LIGHTING.spotLightFBO[index++].first);
		glClear(GL_DEPTH_BUFFER_BIT);

		GLSLShader& shader = SHADER.GetShader(SHADERTYPE::SHADOW);
		shader.Use();

		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix");
		glUniformMatrix4fv(uniform1, 1, GL_FALSE,
			glm::value_ptr(LIGHTING.GetSpotLights()[i].lightSpaceMatrix));

		GLint uniform2 = glGetUniformLocation(shader.GetHandle(), "isDefault");

		// render all non-instanced shadows
		glUniform1i(uniform2, true);

		for (DefaultRenderProperties& prop : RENDERER.GetDefaultProperties())
		{
			float distance = glm::distance(prop.position, spot_light_stuffs.lightpos);
			if (distance > spot_light_stuffs.intensity)
				continue;

			if (prop.Albedo.a < 1.f)
			{
				continue;
			}
			GLint uniform3_t =
				glGetUniformLocation(shader.GetHandle(), "defaultSRT");
			glUniformMatrix4fv(uniform3_t, 1, GL_FALSE, glm::value_ptr(prop.entitySRT));

			glUniform1i(glGetUniformLocation(shader.GetHandle(), "isAnim"), prop.isAnimatable);
			if (prop.isAnimatable)
			{
				std::vector<glm::mat4> transforms = *RENDERER.GetFinalBoneContainer()[prop.boneidx];
				GLint uniform4 = glGetUniformLocation(shader.GetHandle(), "finalBonesMatrices");
				glUniformMatrix4fv(uniform4, (GLsizei)transforms.size(), GL_FALSE, glm::value_ptr(transforms[0]));
			}

			glBindVertexArray(prop.VAO);
			glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

		}

		// render all instanced shadows
		glUniform1i(uniform2, false);

		for (auto& [vao, prop] : RENDERER.GetInstanceContainer()[static_cast<int>(SHADERTYPE::PBR)])
		{
			float distance = glm::distance(prop.position, spot_light_stuffs.lightpos);
			if (distance > spot_light_stuffs.intensity)
				continue;

			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (prop.iter) * sizeof(glm::mat4), &(prop.entitySRT[0]));
			//glBufferSubData(GL_ARRAY_BUFFER, 0, SRTs.size() * sizeof(glm::mat4), SRTs.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(prop.VAO);
			glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);
			glBindVertexArray(0);
		}

		shader.UnUse();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void Shadows::DrawDepthDirectional()
{
	unsigned index = 0;
	for (int i = 0; i < (int)LIGHTING.directionalLightCount; ++i)
	{
		LightProperties& directional_light_stuffs = LIGHTING.GetDirectionLights()[i];

		if (!directional_light_stuffs.enableShadow)
		{
			continue;
		}
		glEnable(GL_DEPTH_TEST);
		//glm::vec3 lightPos(-0.2f, -1.0f, -0.3f); // This suppouse to be the actual light direction
		glm::mat4 lightProjection, lightView;
		float near_plane = -1000.f, far_plane = 1000.f;

		lightProjection = glm::ortho(-90.f, 90.f, -90.f, 90.f, near_plane, far_plane);
		//lightView = glm::lookAt(-directional_light_stuffs.direction + EditorCam.GetCameraPosition(), EditorCam.GetCameraPosition(), glm::vec3(0.0, 1.0, 0.0));
		lightView = glm::lookAt(-directional_light_stuffs.direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

		LIGHTING.GetDirectionLights()[i].lightSpaceMatrix = lightProjection * lightView;
		//lightSpaceMatrix_directional = lightProjection * lightView;

		if (index > MAX_DIRECTION_LIGHT_SHADOW - 1)
			continue;

		glViewport(0, 0, SHADOW_WIDTH_DIRECTIONAL, SHADOW_HEIGHT_DIRECTIONAL);
		glBindFramebuffer(GL_FRAMEBUFFER, LIGHTING.directionalLightFBO[index++].first);
		glClear(GL_DEPTH_BUFFER_BIT);

		GLSLShader& shader = SHADER.GetShader(SHADERTYPE::SHADOW);
		shader.Use();

		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix");
		glUniformMatrix4fv(uniform1, 1, GL_FALSE,
			glm::value_ptr(LIGHTING.GetDirectionLights()[i].lightSpaceMatrix));

		GLint uniform2 = glGetUniformLocation(shader.GetHandle(), "isDefault");
		GLint uniform3_t = glGetUniformLocation(shader.GetHandle(), "defaultSRT");
		GLint uniform4 = glGetUniformLocation(shader.GetHandle(), "isAnim");
		GLint uniform5 = glGetUniformLocation(shader.GetHandle(), "finalBonesMatrices");

		// render all non-instanced shadows
		glUniform1i(uniform2, true);

		for (DefaultRenderProperties& prop : RENDERER.GetDefaultProperties())
		{
			if (prop.Albedo.a < 1.f)
			{
				continue;
			}

			glUniformMatrix4fv(uniform3_t, 1, GL_FALSE, glm::value_ptr(prop.entitySRT));

			glUniform1i(uniform4, prop.isAnimatable);
			if (prop.isAnimatable)
			{
				std::vector<glm::mat4> transforms = *RENDERER.GetFinalBoneContainer()[prop.boneidx];
				glUniformMatrix4fv(uniform5, (GLsizei)transforms.size(), GL_FALSE, glm::value_ptr(transforms[0]));
			}

			glBindVertexArray(prop.VAO);
			glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}


		// render all instanced shadows
		glUniform1i(uniform2, false);

		for (auto& [vao, prop] : RENDERER.GetInstanceContainer()[static_cast<int>(SHADERTYPE::PBR)])
		{
			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (prop.iter) * sizeof(glm::mat4), &(prop.entitySRT[0]));
			//glBufferSubData(GL_ARRAY_BUFFER, 0, SRTs.size() * sizeof(glm::mat4), SRTs.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(prop.VAO);
			glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);
			glBindVertexArray(0);
		}

		shader.UnUse();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void Shadows::DrawDepthPoint()
{
	unsigned index = 0;
	for (int i = 0; i < (int)LIGHTING.pointLightCount; ++i)
	{
		LightProperties& point_light_stuffs = LIGHTING.GetPointLights()[i];
		if (!point_light_stuffs.enableShadow)
		{
			continue;
		}

		float near_plane = 0.001f, far_plane = 1000.f;

		std::vector<glm::mat4> shadowTransforms;

		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
		shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, LIGHTING.pointLightFBO[index++].first);
		glClear(GL_DEPTH_BUFFER_BIT);

		GLSLShader& shader = SHADER.GetShader(SHADERTYPE::POINTSHADOW);
		shader.Use();

		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "shadowMatrices")
			, 6, GL_FALSE, glm::value_ptr(shadowTransforms[0]));

		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "lightPos");
		glUniform3fv(uniform1, 1,
			glm::value_ptr(point_light_stuffs.lightpos));

		GLint uniform2 =
			glGetUniformLocation(shader.GetHandle(), "far_plane");
		glUniform1f(uniform2, far_plane);

		GLint uniform3 = glGetUniformLocation(shader.GetHandle(), "isDefault");

		// render all non-instanced shadows
		glUniform1i(uniform3, true);

		for (DefaultRenderProperties& prop : RENDERER.GetDefaultProperties())
		{
			float distance = glm::distance(prop.position, point_light_stuffs.lightpos);
			if (distance > point_light_stuffs.intensity * 10.f)
				continue;

			if (prop.Albedo.a < 1.f)
			{
				continue;
			}
			GLint uniform3_t =
				glGetUniformLocation(shader.GetHandle(), "defaultSRT");
			glUniformMatrix4fv(uniform3_t, 1, GL_FALSE, glm::value_ptr(prop.entitySRT));

			glUniform1i(glGetUniformLocation(shader.GetHandle(), "isAnim"), prop.isAnimatable);
			if (prop.isAnimatable)
			{
				std::vector<glm::mat4> transforms = *RENDERER.GetFinalBoneContainer()[prop.boneidx];
				GLint uniform4 = glGetUniformLocation(shader.GetHandle(), "finalBonesMatrices");
				glUniformMatrix4fv(uniform4, (GLsizei)transforms.size(), GL_FALSE, glm::value_ptr(transforms[0]));
			}

			glBindVertexArray(prop.VAO);
			glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}


		// render all instanced shadows
		glUniform1i(uniform3, false);

		for (auto& [vao, prop] : RENDERER.GetInstanceContainer()[static_cast<int>(SHADERTYPE::PBR)])
		{
			float distance = glm::distance(prop.position, point_light_stuffs.lightpos);
			if (distance > point_light_stuffs.intensity * 10.f)
				continue;

			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (prop.iter) * sizeof(glm::mat4), &(prop.entitySRT[0]));
			//glBufferSubData(GL_ARRAY_BUFFER, 0, SRTs.size() * sizeof(glm::mat4), SRTs.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(prop.VAO);
			glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);
			glBindVertexArray(0);
		}

		shader.UnUse();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

