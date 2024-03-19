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

#define AR 1.777778f


void UIRenderer::UIDrawScreenSpace(BaseCamera& _camera)
{
	// Setups required for all UI
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	const Transform* canvasTransform{ nullptr };
	sortedUI.clear();

	for (Canvas& currCanvas : currentScene.GetArray<Canvas>())
	{
		if (currCanvas.state == DELETED) continue;
		Entity& entity = currentScene.Get<Entity>(currCanvas);
		canvasTransform = &currentScene.Get<Transform>(entity);
		continue;
	}

	if (!canvasTransform)
	{
		glDisable(GL_BLEND);
		return;
	}

	const glm::mat4& canvasMatrix = glm::inverse(canvasTransform->GetWorldMatrix());
	glm::mat4 scaleMat = glm::identity<glm::mat4>();

	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
	{
		if (!currentScene.IsActive(Sprite) || Sprite.state == DELETED || Sprite.WorldSpace)
			continue;

		// Declarations for the things we need - SRT
		Entity& entity = currentScene.Get<Entity>(Sprite);
		if (!currentScene.IsActive(entity)) continue;

		Transform& transform = currentScene.Get<Transform>(entity);

		bool been_inserted = false;
		float dist = transform.GetGlobalTranslation().z;
		std::pair<Entity, float> temp{ entity, dist };
		for (int i = 0; i < sortedUI.size(); ++i)
		{
			if (dist > sortedUI[i].second)
			{
				sortedUI.insert(sortedUI.begin() + i, temp);
				been_inserted = true;
				break;
			}
		}
		if (!been_inserted)
		{
			sortedUI.push_back(temp);
		}
	}

	for (TextRenderer& text : currentScene.GetArray<TextRenderer>())
	{
		if (!currentScene.IsActive(text) || text.state == DELETED || text.worldSpace)
			continue;

		// Declarations for the things we need - SRT
		Entity& entity = currentScene.Get<Entity>(text);
		if (!currentScene.IsActive(entity)) continue;

		Transform& transform = currentScene.Get<Transform>(entity);

		bool been_inserted = false;
		float dist = transform.GetGlobalTranslation().z;
		std::pair<Entity, float> temp{ entity, dist };
		for (int i = 0; i < sortedUI.size(); ++i)
		{
			if (dist > sortedUI[i].second)
			{
				sortedUI.insert(sortedUI.begin() + i, temp);
				been_inserted = true;
				break;
			}
		}
		if (!been_inserted)
		{
			sortedUI.push_back(temp);
		}
	}

	for (int i = 0; i < sortedUI.size(); ++i)
	{
		Entity& entity = sortedUI[i].first;
		if (entity.HasComponent<SpriteRenderer>())
		{
			RenderSprite2D(currentScene, _camera, entity.EUID(), canvasMatrix, scaleMat);
		}
		if (entity.HasComponent<TextRenderer>())
		{
			TEXTSYSTEM.RenderScreenSpace(currentScene, _camera, entity.EUID(), canvasMatrix, scaleMat);
		}
	}

	
	glDisable(GL_BLEND);
}

// Drawing UI onto worldspace
void UIRenderer::UIDrawWorldSpace(BaseCamera& _camera)
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
	std::vector<std::pair<SpriteRenderer,float>> Sorted_SR;

	glm::mat4 scaleMat = glm::identity<glm::mat4>();
	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
	{
		if (Sprite.state == DELETED) continue;
		// This means it's 2D space
		if (!Sprite.WorldSpace)
		{
			continue;
		}
		// Declarations for the things we need - SRT
		Entity& entity = currentScene.Get<Entity>(Sprite);
		if (!currentScene.IsActive(entity)) continue;
		Transform& transform = currentScene.Get<Transform>(entity);

		bool been_inserted = false;
		float dist = glm::distance(transform.GetGlobalTranslation(), _camera.GetCameraPosition());
		std::pair<SpriteRenderer, float> temp{ Sprite,dist };
		for (int i = 0; i < Sorted_SR.size(); ++i)
		{
			if (dist > Sorted_SR[i].second)
			{
				Sorted_SR.insert(Sorted_SR.begin() + i, temp);
				been_inserted = true;
				break;
			}
		}
		if (!been_inserted)
		{
			Sorted_SR.push_back(temp);
		}
	}

	for (int i = 0; i < Sorted_SR.size(); ++i)
	{
		SpriteRenderer& Sprite = Sorted_SR[i].first;
		Transform& transform = currentScene.Get<Transform>(Sprite);

		glUniform1f(glGetUniformLocation(shader.GetHandle(), "AlphaScaler"),
			Sprite.AlphaMultiplier);

		// Setting bool to see if there is a sprite to render
		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "RenderSprite");
		
		BaseTexture* pTexture = TextureManager.GetBaseTexture(Sprite.SpriteTexture);
		GLuint spriteTextureID = 0;

		if (pTexture)
		{
			spriteTextureID = pTexture->textureID;
			scaleMat[0][0] = pTexture->pixelDimension.x / 1000.f;
			scaleMat[1][1] = pTexture->pixelDimension.y / 1000.f;
			// SRT uniform
			glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
				1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix() * scaleMat)
			);

			if (Sprite.SpriteTexture == 0)
			{
				glUniform1f(uniform1, false);
			}
			else
			{
				glUniform1f(uniform1, true);
			}
		}
		else
		{
			// SRT uniform
			glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
				1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix())
			);

			glUniform1f(uniform1, false);
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
void UIRenderer::UIDrawSceneView(BaseCamera& _camera)
{
	// Setups required for all UI
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_DEPTH_BUFFER_BIT);
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UI_WORLD);
	shader.Use();

	const Transform* canvasTransform{ nullptr };

	for (Canvas& currCanvas : currentScene.GetArray<Canvas>())
	{
		if (currCanvas.state == DELETED) continue;
		Entity& entity = currentScene.Get<Entity>(currCanvas);
		canvasTransform = &currentScene.Get<Transform>(entity);
		continue;
	}

	if (!canvasTransform)
	{
		return;
	}

	const glm::mat4& canvasMatrix = glm::inverse(canvasTransform->GetWorldMatrix());

	// Setting the projection here since all of them use the same projection

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "view"),
		1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));


	glUniform1f(glGetUniformLocation(shader.GetHandle(), "RenderSprite"), false);
	glUniform1f(glGetUniformLocation(shader.GetHandle(), "RenderIcon"), false);

	glm::mat4 scaleMat = glm::identity<glm::mat4>();
	scaleMat[0][0] = AR;

	// Setting the projection here since all of them use the same projection
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

		// Setting bool to see if there is a sprite to render
		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "RenderSprite");

		glUniform1f(glGetUniformLocation(shader.GetHandle(), "AlphaScaler"),
			Sprite.AlphaMultiplier);

		BaseTexture* pTexture = TextureManager.GetBaseTexture(Sprite.SpriteTexture);
		GLuint spriteTextureID = 0;

		if (pTexture)
		{
			spriteTextureID = pTexture->textureID;
			scaleMat[0][0] = pTexture->pixelDimension.x / 1000.f;
			scaleMat[1][1] = pTexture->pixelDimension.y / 1000.f;

			// SRT uniform
			glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
				1, GL_FALSE, glm::value_ptr(
					canvasMatrix * transform.GetWorldMatrix() * scaleMat)
			);

			if (Sprite.SpriteTexture == 0)
			{
				glUniform1f(uniform1, false);
			}
			else
			{
				glUniform1f(uniform1, true);
			}
		}
		else
		{
			// SRT uniform
			glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
				1, GL_FALSE, glm::value_ptr(
					canvasMatrix * transform.GetWorldMatrix())
			);

			glUniform1f(uniform1, false);
		}

		// Binding Texture - might be empty , above uniform will sort it
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spriteTextureID);

		renderQuad(Renderer_quadVAO, Renderer_quadVBO);
	}
	shader.UnUse();
	glDisable(GL_BLEND);
}

void UIRenderer::RenderSprite2D(Scene& _scene, BaseCamera& _camera, const Engine::UUID& _euid, const glm::mat4& _canvasMtx, glm::mat4& _scaleMtx)
{
	glm::mat4 OrthoProjection = glm::ortho(-1.f * AR, 1.f * AR, -1.f, 1.f, -10.f, 10.f);

	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UI_SCREEN);
	shader.Use();

	// Setting the projection here since all of them use the same projection
	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(OrthoProjection));

	SpriteRenderer& Sprite = _scene.Get<SpriteRenderer>(_euid);
	Transform& transform = _scene.Get<Transform>(_euid);

	glUniform1f(glGetUniformLocation(shader.GetHandle(), "AlphaScaler"),
		Sprite.AlphaMultiplier);
	
	// Setting bool to see if there is a sprite to render
	GLint uniform1 =
		glGetUniformLocation(shader.GetHandle(), "RenderSprite");

	BaseTexture* pTexture = TextureManager.GetBaseTexture(Sprite.SpriteTexture);
	GLuint spriteTextureID = 0;

	if (pTexture)
	{
		spriteTextureID = pTexture->textureID;
		_scaleMtx[0][0] = pTexture->pixelDimension.x / 1000.f;
		_scaleMtx[1][1] = pTexture->pixelDimension.y / 1000.f;
		// SRT uniform
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(_canvasMtx * transform.GetWorldMatrix() * _scaleMtx)
		);

		if (Sprite.SpriteTexture == 0)
		{
			glUniform1f(uniform1, false);
		}
		else
		{
			glUniform1f(uniform1, true);
		}
	}
	else
	{
		// SRT uniform
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(_canvasMtx * transform.GetWorldMatrix())
		);

		glUniform1f(uniform1, false);
	}

	// Binding Texture - might be empty , above uniform will sort it
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, spriteTextureID);
	renderQuad(Renderer_quadVAO, Renderer_quadVBO);

	shader.UnUse();
}