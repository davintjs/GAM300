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

void UIRenderer::Init()
{

}

void UIRenderer::Update(float dt)
{
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	const Transform* canvasTransform{ nullptr };

	sortedUIScreenSpace.clear();
	sortedUIWorldSpace.clear();

	for (Canvas& currCanvas : currentScene.GetArray<Canvas>())
	{
		if (currCanvas.state == DELETED) continue;
		
		Entity& entity = currentScene.Get<Entity>(currCanvas);
		canvasTransform = &currentScene.Get<Transform>(entity);
	}

	canvasMatrix = scaleMatrix = glm::identity<glm::mat4>();
	if (canvasTransform)
	{
		canvasMatrix = glm::inverse(canvasTransform->GetWorldMatrix());
	}

	Camera* pCamera = nullptr;
	for (Camera& camera : currentScene.GetArray<Camera>())
	{
		if (!currentScene.IsActive(camera) || camera.state == DELETED)
			continue;

		pCamera = &camera;
		break;
	}

	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
	{
		if (!currentScene.IsActive(Sprite) || Sprite.state == DELETED)
			continue;

		// Declarations for the things we need - SRT
		Entity& entity = currentScene.Get<Entity>(Sprite);
		if (!currentScene.IsActive(entity)) continue;

		Transform& t = currentScene.Get<Transform>(entity);

		const float d = (pCamera) ? glm::distance(t.GetGlobalTranslation(), pCamera->GetCameraPosition()) : t.GetGlobalTranslation().z;
		if(Sprite.WorldSpace)
			SortUserInterface(sortedUIWorldSpace, entity, d);
		else
			SortUserInterface(sortedUIScreenSpace, entity, d);
	}

	for (TextRenderer& text : currentScene.GetArray<TextRenderer>())
	{
		if (!currentScene.IsActive(text) || text.state == DELETED)
			continue;

		// Declarations for the things we need - SRT
		Entity& entity = currentScene.Get<Entity>(text);
		if (!currentScene.IsActive(entity)) continue;

		Transform& t = currentScene.Get<Transform>(entity);

		const float d = (pCamera) ? glm::distance(t.GetGlobalTranslation(), pCamera->GetCameraPosition()) : t.GetGlobalTranslation().z;

		if (text.worldSpace)
			SortUserInterface(sortedUIWorldSpace, entity, d);
		else
			SortUserInterface(sortedUIScreenSpace, entity, d);
	}
}

void UIRenderer::Exit()
{

}

void UIRenderer::UIDrawScreenSpace(BaseCamera& _camera)
{
	// Setups required for all UI
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_DEPTH_BUFFER_BIT);

	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	for (int i = 0; i < sortedUIScreenSpace.size(); ++i)
	{
		Entity& entity = sortedUIScreenSpace[i].first;
		if (entity.HasComponent<SpriteRenderer>())
		{
			RenderSprite2D(currentScene, _camera, entity.EUID());
		}
		if (entity.HasComponent<TextRenderer>())
		{
			TEXTSYSTEM.RenderScreenSpace(currentScene, _camera, entity.EUID(), canvasMatrix);
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
	for (int i = 0; i < sortedUIWorldSpace.size(); ++i)
	{
		Entity& entity = sortedUIWorldSpace[i].first;
		if (entity.HasComponent<SpriteRenderer>())
		{
			RenderSprite3D(currentScene, _camera, entity.EUID());
		}
		if (entity.HasComponent<TextRenderer>())
		{
			TEXTSYSTEM.RenderWorldSpace(currentScene, _camera, entity.EUID());
		}
	}

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
	scaleMatrix[0][0] = AR;

	for (int i = 0; i < sortedUIScreenSpace.size(); ++i)
	{
		Entity& entity = sortedUIScreenSpace[i].first;
		Transform& t = currentScene.Get<Transform>(entity);
		
		if (entity.HasComponent<SpriteRenderer>())
		{
			RenderSprite3D(currentScene, _camera, entity.EUID());
		}
		if (entity.HasComponent<TextRenderer>())
		{
			TEXTSYSTEM.RenderWorldSpace(currentScene, _camera, entity.EUID());
		}
	}

	glDisable(GL_BLEND);
}

void UIRenderer::RenderSprite2D(Scene& _scene, BaseCamera& _camera, const Engine::UUID& _euid)
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

	glUniform1f(glGetUniformLocation(shader.GetHandle(), "gammaCorrection"), RENDERER.getGamma());


	BaseTexture* pTexture = TextureManager.GetBaseTexture(Sprite.SpriteTexture);
	GLuint spriteTextureID = 0;

	if (pTexture)
	{
		spriteTextureID = pTexture->textureID;
		scaleMatrix[0][0] = pTexture->pixelDimension.x / 1000.f;
		scaleMatrix[1][1] = pTexture->pixelDimension.y / 1000.f;
		// SRT uniform
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(canvasMatrix * transform.GetWorldMatrix() * scaleMatrix)
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
			1, GL_FALSE, glm::value_ptr(canvasMatrix * transform.GetWorldMatrix())
		);

		glUniform1f(uniform1, false);
	}

	// Binding Texture - might be empty , above uniform will sort it
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, spriteTextureID);
	renderQuad(Renderer_quadVAO, Renderer_quadVBO);

	shader.UnUse();
}

void UIRenderer::RenderSprite3D(Scene& _scene, BaseCamera& _camera, const Engine::UUID& _euid)
{
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UI_WORLD);
	shader.Use();

	// Setting the projection here since all of them use the same projection
	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "view"),
		1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));

	SpriteRenderer& Sprite = _scene.Get<SpriteRenderer>(_euid);
	Transform& transform = _scene.Get<Transform>(_euid);

	glUniform1f(glGetUniformLocation(shader.GetHandle(), "RenderIcon"), false);
	glUniform1f(glGetUniformLocation(shader.GetHandle(), "AlphaScaler"), Sprite.AlphaMultiplier);

	// Setting bool to see if there is a sprite to render
	GLint uniform1 = glGetUniformLocation(shader.GetHandle(), "RenderSprite");
	glUniform1f(uniform1, false);

	glUniform1f(glGetUniformLocation(shader.GetHandle(), "gammaCorrection"), RENDERER.getGamma());

	BaseTexture* pTexture = TextureManager.GetBaseTexture(Sprite.SpriteTexture);
	GLuint spriteTextureID = 0;

	if (pTexture)
	{
		spriteTextureID = pTexture->textureID;
		scaleMatrix[0][0] = pTexture->pixelDimension.x / 1000.f;
		scaleMatrix[1][1] = pTexture->pixelDimension.y / 1000.f;
		// SRT uniform
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix() * scaleMatrix)
		);

		if (Sprite.SpriteTexture != 0)
			glUniform1f(uniform1, true);
	}
	else
	{
		// SRT uniform
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix())
		);
	}

	// Binding Texture - might be empty , above uniform will sort it
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, spriteTextureID);
	renderQuad(Renderer_quadVAO, Renderer_quadVBO);

	shader.UnUse();
}

void UIRenderer::SortUserInterface(std::vector<std::pair<Entity, float>>& _container, Entity& _entity, const float& _distance)
{
	bool been_inserted = false;
	std::pair<Entity, float> temp{ _entity, _distance };
	for (int i = 0; i < _container.size(); ++i)
	{
		if (_distance > _container[i].second)
		{
			_container.insert(_container.begin() + i, temp);
			been_inserted = true;
			break;
		}
	}
	if (!been_inserted)
	{
		_container.push_back(temp);
	}
}