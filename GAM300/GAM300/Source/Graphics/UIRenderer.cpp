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
#include "Texture/TextureManager.h"

unsigned int Renderer_quadVAO = 0;
unsigned int Renderer_quadVBO = 0;

unsigned int Renderer_quadVAO_WM = 0;
unsigned int Renderer_quadVBO_WM = 0;


void UIRenderer::UIDraw_2D(BaseCamera& _camera)
{
	// Setups required for all UI
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glm::mat4 OrthoProjection = glm::ortho(-800.f, 800.f, -450.f, 450.f, 0.001f, 10.f);
	//glm::mat4 OrthoProjection = glm::ortho(0.f, 16.f, 0.f, 9.f, -10.f, 10.f);
	//glm::mat4 OrthoProjection = glm::ortho(-8.f, 8.f, -4.5f, 4.5f, -10.f, 10.f);
	glm::mat4 OrthoProjection = glm::ortho(-1.f, 1.f, -1.f, 1.f, -10.f, 10.f);

	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UI_SCREEN);
	shader.Use();

	// Setting the projection here since all of them use the same projection

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(OrthoProjection));
	
	const Transform* canvasTransform{ nullptr };

	for (Canvas& currCanvas : currentScene.GetArray<Canvas>())
	{
		if (currCanvas.state == DELETED) continue;
		Entity& entity = currentScene.Get<Entity>(currCanvas);
		canvasTransform = &currentScene.Get<Transform>(entity);
		continue;
	}

	if(!canvasTransform)
	{
		return;
	}

	glm::mat4 canvasMatrix = glm::inverse( canvasTransform->GetWorldMatrix());

	//canvasMatrix = glm::inverse()



	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
	{
		if (Sprite.state == DELETED) continue;

		// This means it's 3D space
		if (Sprite.WorldSpace)
		{
			continue;
		}

		// Declarations for the things we need - SRT
		Entity& entity = currentScene.Get<Entity>(Sprite);
		if (!currentScene.IsActive(entity)) continue;

		Transform& transform = currentScene.Get<Transform>(entity);

		// SRT uniform
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(
				canvasMatrix * transform.GetWorldMatrix() )
		);
		glUniform1f(glGetUniformLocation(shader.GetHandle(), "AlphaScaler"),
			Sprite.AlphaMultiplier);
		// Setting bool to see if there is a sprite to render
		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "RenderSprite");
		GLuint spriteTextureID = TextureManager.GetTexture(Sprite.SpriteTexture);
		if (Sprite.SpriteTexture == 0)
		{
			glUniform1f(uniform1, false);
		}
		else
		{
			glUniform1f(uniform1, true);
		}

		// Binding Texture - might be empty , above uniform will sort it
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spriteTextureID);


		renderQuad(Renderer_quadVAO, Renderer_quadVBO);

	}
	shader.UnUse();
	glDisable(GL_BLEND);
}

// Drawing UI onto worldspace
void UIRenderer::UIDraw_3D(BaseCamera& _camera)
{
	// Setups required for all UI
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UI_WORLD);
	shader.Use();

	// Setting the projection here since all of them use the same projection

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "view"),
		1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));

	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
	{
		if (Sprite.state == DELETED) continue;
		// This means it's 2D space
		if (!Sprite.WorldSpace)
		{
			continue;
		}
		glUniform1f(glGetUniformLocation(shader.GetHandle(), "AlphaScaler"),
			Sprite.AlphaMultiplier);

		// Declarations for the things we need - SRT
		Entity& entity = currentScene.Get<Entity>(Sprite);
		if (!currentScene.IsActive(entity)) continue;

		Transform& transform = currentScene.Get<Transform>(entity);

		// SRT uniform
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix()));

		// Setting bool to see if there is a sprite to render
		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "RenderSprite");
		GLuint spriteTextureID = TextureManager.GetTexture(Sprite.SpriteTexture);
		if (Sprite.SpriteTexture == 0)
		{
			glUniform1f(uniform1, false);
		}
		else
		{
			glUniform1f(uniform1, true);
		}

		// Binding Texture - might be empty , above uniform will sort it
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spriteTextureID);

		renderQuad(Renderer_quadVAO, Renderer_quadVBO);
	}
	shader.UnUse();
	glDisable(GL_BLEND);
}

// Drawing Screenspace UI onto worldspace
void UIRenderer::UIDraw_2DWorldSpace(BaseCamera& _camera)
{
	// Setups required for all UI
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UI_WORLD);
	shader.Use();

	// Setting the projection here since all of them use the same projection

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "view"),
		1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));


	glUniform1f(glGetUniformLocation(shader.GetHandle(), "RenderSprite"), false);
	glUniform1f(glGetUniformLocation(shader.GetHandle(), "RenderIcon"), false);

	for (Canvas& currCanvas : currentScene.GetArray<Canvas>())
	{
		if (currCanvas.state == DELETED) continue;
		Entity& entity = currentScene.Get<Entity>(currCanvas);
		Transform& transform = currentScene.Get<Transform>(entity);

		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix()));

		glLineWidth(20.f);
		renderQuadWireMesh(Renderer_quadVAO_WM, Renderer_quadVBO_WM);
	}
	glLineWidth(1.f);

	// Setting the projection here since all of them use the same projection

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "view"),
		1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));

	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
	{
		// This means it's 2D space
		if (Sprite.WorldSpace)
		{
			continue;
		}

		// Declarations for the things we need - SRT
		Entity& entity = currentScene.Get<Entity>(Sprite);
		if (!currentScene.IsActive(entity)) continue;

		Transform& transform = currentScene.Get<Transform>(entity);

		// SRT uniform
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix()));

		// Setting bool to see if there is a sprite to render
		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "RenderSprite");
		GLuint spriteTextureID = TextureManager.GetTexture(Sprite.SpriteTexture);

		glUniform1f(glGetUniformLocation(shader.GetHandle(), "AlphaScaler"),
			Sprite.AlphaMultiplier);
		if (Sprite.SpriteTexture == 0)
		{
			glUniform1f(uniform1, false);
		}
		else
		{
			glUniform1f(uniform1, true);
		}

		// Binding Texture - might be empty , above uniform will sort it
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spriteTextureID);

		renderQuad(Renderer_quadVAO, Renderer_quadVBO);
	}
	shader.UnUse();
	glDisable(GL_BLEND);
}
