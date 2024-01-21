/*!***************************************************************************************
\file			ColourPicker.cpp
\project
\author         Euan Lim

\par			Course: GAM300
\date           05/11/2023

\brief
	This file contains the definitions to implementing colour picking

All content ? 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include "Scene/SceneManager.h"
#include "IOManager/InputSystem.h"
#include "IOManager/InputHandler.h"
#include "Core/Events.h"
#include "Editor/Editor.h"
#include "MeshManager.h"
#include "Texture/TextureManager.h"
#include "Editor/EditorCamera.h"


extern unsigned int Renderer_quadVAO;
extern unsigned int Renderer_quadVBO;


glm::vec2 windowPos;
glm::vec2 windowDimension;

void ColourPicker::Init()
{
	FRAMEBUFFER.CreateColorPicking(colorPickFBO, colorPickTex);
}

void ColourPicker::ColorPickingUIButton(BaseCamera& _camera)
{

	Scene& currentScene = SceneManager::Instance().GetCurrentScene();

	glm::mat4 OrthoProjection = glm::ortho(-1.f, 1.f, -1.f, 1.f, -10.f, 10.f);
	glm::mat4 IdentityMat = glm::mat4(1.f);
	glm::mat4 projToUse, viewToUse, srtToUse;

	glViewport(0, 0, 1600, 900);
	glBindFramebuffer(GL_FRAMEBUFFER, colorPickFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::COLOURPICKING);
	shader.Use();

	GLint uniform1 =
		glGetUniformLocation(shader.GetHandle(), "mode");
	glUniform1i(uniform1, 2);


	std::vector<Engine::UUID> EUID_Holder;

	int offset = 1;

	int index = 0;

	bool spriteToColourPick = false;
	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
	{
		//std::cout << "enter\n";
		if (Sprite.state == DELETED) continue;

		if (!Sprite.ColourPicked)
		{
;			continue;
		}
		//std::cout << "Passed\n";

		spriteToColourPick = true;
		Entity& entity = currentScene.Get<Entity>(Sprite);
		Transform& transform = currentScene.Get<Transform>(entity);

		int temp = index + offset;

		++index;
		
		EUID_Holder.emplace_back(entity.EUID());

		float r = (float)((temp & 0x000000FF) >> 0);
		float g = (float)((temp & 0x0000FF00) >> 8);
		float b = (float)((temp & 0x00FF0000) >> 16);

		// in game mode, only care about buttons
		glm::vec4 picking_color = glm::vec4 (r / 255.f, g / 255.f, b / 255.f, 1.f);

		glUniform4fv(glGetUniformLocation(shader.GetHandle(), "PickingColour")
		, 1, glm::value_ptr(picking_color));


		if (Sprite.WorldSpace) // 3D Space UI
		{
			// World Space
			std::cout << "WorldSpace\n";
			projToUse = _camera.GetProjMatrix();
			viewToUse = _camera.GetViewMatrix();
			srtToUse = transform.GetWorldMatrix();
		}
		else // Screen Space UI
		{
			// Screen Space
			std::cout << "ScreenSpace\n";
			projToUse = OrthoProjection;
			viewToUse = IdentityMat;
			srtToUse = transform.GetLocalMatrix();
		}

		GLuint spriteTextureID = TextureManager.GetTexture(Sprite.SpriteTexture);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spriteTextureID);

		GLint uniform2 =
			glGetUniformLocation(shader.GetHandle(), "isTexture");

		if (Sprite.SpriteTexture == 0)
		{
			
			glUniform1f(uniform2, false);
		}
		else
		{
			glUniform1f(uniform2, true);
		}

		DrawSprites(projToUse, viewToUse, srtToUse, shader);

	}

	shader.UnUse();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned char data[4];

	glm::vec2 mousepos = InputHandler::getMousePos();

#if defined(_BUILD)

	glReadPixels(mousepos.x, mousepos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

#else

	glm::vec2 true_mousepos;
	true_mousepos.y = mousepos.y;

	float width = (float)Application::GetWidth();
	float height = (float)Application::GetHeight();
	true_mousepos.x = mousepos.x - windowPos.x;
	true_mousepos.x = (true_mousepos.x / windowDimension.x) * 1600.f;

	true_mousepos.y = mousepos.y - (-(windowPos.y + windowDimension.y) + height);
	true_mousepos.y = (true_mousepos.y / windowDimension.y) * 900.f;
	//std::cout << "game : " << true_mousepos.x << " , " << true_mousepos.y << "\n";
	glReadPixels((GLint)true_mousepos.x, (GLint)true_mousepos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);


#endif // _BUILD

	int selectedID = data[0] +
		data[1] * 255 +
		data[2] * 255 * 255;

	if (spriteToColourPick && (selectedID > 0) && (selectedID != 13421772) )
	{
		index = selectedID - offset;
		if (index < EUID_Holder.size())
		{
			
			Engine::UUID EUID_Index = EUID_Holder[index];
		
			Tag& entity_tag = currentScene.Get<Tag>(EUID_Index);
			PRINT(entity_tag.name, "\n");
			std::cout << "from ColorPickingUIButton : " << entity_tag.name << "\n";
		}
		/*else
		{
			std::cout << "wtf\n";
		}*/
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT);

}


Engine::UUID ColourPicker::ColorPickingMeshs(BaseCamera& _camera)
{
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	

	glm::mat4 projToUse = _camera.GetProjMatrix(); 
	glm::mat4 viewToUse = _camera.GetViewMatrix();
	glm::mat4 srtToUse;

	glViewport(0, 0, 1600, 900);
	//glViewport(0, 0, EditorCam.GetViewportSize().x, EditorCam.GetViewportSize().y);

	glBindFramebuffer(GL_FRAMEBUFFER, colorPickFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::COLOURPICKING);
	shader.Use();
	GLint uniform1 =
		glGetUniformLocation(shader.GetHandle(), "mode");
	GLint uniform2 =
		glGetUniformLocation(shader.GetHandle(), "isTexture");

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(projToUse));

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "view"),
		1, GL_FALSE, glm::value_ptr(viewToUse));



	glUniform1i(uniform1, 0);
	glUniform1f(uniform2, false);

	std::vector<Engine::UUID> EUID_Holder;

	int offset = 1;
	int index = 0;
	bool meshToColourPick = false;

	for (MeshRenderer& mr : currentScene.GetArray<MeshRenderer>())
	{
		if (mr.state == DELETED) continue; 
		if (!meshToColourPick)
		{
			meshToColourPick = true;
		}

		Entity& entity = currentScene.Get<Entity>(mr);
		Transform& transform = currentScene.Get<Transform>(entity);

		int temp = index + offset;
		//std::cout << temp << "\n";

		++index;

		EUID_Holder.emplace_back(entity.EUID());

		float r = (float)((temp & 0x000000FF) >> 0);
		float g = (float)((temp & 0x0000FF00) >> 8);
		float b = (float)((temp & 0x00FF0000) >> 16);

		// in game mode, only care about buttons
		glm::vec4 picking_color = glm::vec4((r / 255.f), (g / 255.f) , (b / 255.f) , 1.f);

		glUniform4fv(glGetUniformLocation(shader.GetHandle(), "PickingColour")
			, 1, glm::value_ptr(picking_color));


		srtToUse = transform.GetWorldMatrix();

		if (MESHMANAGER.vaoMap.find(mr.meshID) == MESHMANAGER.vaoMap.end())
		{
			continue;
		}

		Mesh* currMesh = MESHMANAGER.DereferencingMesh(mr.meshID);

		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(srtToUse));

		glBindVertexArray(currMesh->vaoID);
		glDrawElements(currMesh->prim, currMesh->drawCounts, GL_UNSIGNED_INT, 0);

	}

	shader.UnUse();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned char data[4];

	glm::vec2 mousePosition = InputHandler::getMousePos();
#if defined(_BUILD)

	glReadPixels(mousePosition.x, mousePosition.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

#else

	

	mousePosition.y -= Application::GetHeight() - EditorCam.GetScenePosition().y - EditorCam.GetViewportSize().y;

	glm::vec2 mouseScenePosition = { mousePosition.x - EditorCam.GetScenePosition().x, mousePosition.y };

	glm::vec2 mp = mouseScenePosition / EditorCam.GetViewportSize();
	mp.x *= 1600.f;
	mp.y *= 900.f;

	glReadPixels((GLint)mp.x, (GLint)mp.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

#endif // _BUILD

	int selectedID = data[0] +
		data[1] * 256 +
		data[2] * 256 * 256;

	//std::cout << selectedID << "\n";

	if (meshToColourPick && (selectedID > 0) && (selectedID != 13421772))
	{
		index = selectedID - offset;
		if (index < EUID_Holder.size())
		{
			Engine::UUID EUID_Index = EUID_Holder[selectedID - offset];
			Tag& entity_tag = currentScene.Get<Tag>(EUID_Index);
			//PRINT(entity_tag.name, "\n");

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



			return EUID_Index;
		}
		
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return 0;

}
void ColourPicker::DrawSprites(glm::mat4 _projection , glm::mat4 _view , glm::mat4 _srt , GLSLShader& _shader)
{
	glUniformMatrix4fv(glGetUniformLocation(_shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(_projection));

	glUniformMatrix4fv(glGetUniformLocation(_shader.GetHandle(), "view"),
		1, GL_FALSE, glm::value_ptr(_view));

	glUniformMatrix4fv(glGetUniformLocation(_shader.GetHandle(), "SRT"),
		1, GL_FALSE, glm::value_ptr(_srt));

	renderQuad(Renderer_quadVAO, Renderer_quadVBO);

}