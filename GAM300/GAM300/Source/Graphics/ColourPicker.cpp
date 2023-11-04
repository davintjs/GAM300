#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include "Scene/SceneManager.h"
#include "IOManager/InputSystem.h"
#include "IOManager/InputHandler.h"
#include "Core/Events.h"
#include "Editor/Editor.h"


extern unsigned int Renderer_quadVAO;
extern unsigned int Renderer_quadVBO;


glm::vec2 windowPos;
glm::vec2 windowDimension;

void ColourPicker::Init()
{
	FRAMEBUFFER.CreateColorPicking(colorPickFBO, colorPickTex);
}

void ColourPicker::ColorPickingUI(BaseCamera& _camera)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Scene& currentScene = SceneManager::Instance().GetCurrentScene();

	glm::mat4 OrthoProjection = glm::ortho(-1.f, 1.f, -1.f, 1.f, -10.f, 10.f);
	glm::mat4 IdentityMat = glm::mat4(1.f);
	glm::mat4 projToUse, viewToUse, srtToUse;

	glViewport(0, 0, 1600, 900);
	glBindFramebuffer(GL_FRAMEBUFFER, colorPickFBO);
	
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::COLOURPICKING);
	shader.Use();

	std::vector<Engine::UUID> EUID_Holder;

	int offset = 1;

	int index = 0;

	bool spriteToColourPick = false;
	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
	{
		if (!Sprite.ColourPicked)
		{
;			continue;
		}
		spriteToColourPick = true;
		Entity& entity = currentScene.Get<Entity>(Sprite);
		Transform& transform = currentScene.Get<Transform>(entity);

		int temp = index + offset;
		//std::cout << temp << "\n";

		++index;
		
		EUID_Holder.emplace_back(entity.EUID());

		float r = (temp & 0x000000FF) >> 0;
		float g = (temp & 0x0000FF00) >> 8;
		float b = (temp & 0x00FF0000) >> 16;

		// in game mode, only care about buttons
		glm::vec4 picking_color = glm::vec4 (r / 255.f, g / 255.f, b / 255.f, 1.f);

		glUniform4fv(glGetUniformLocation(shader.GetHandle(), "PickingColour")
		, 1, glm::value_ptr(picking_color));


		if (Sprite.WorldSpace) // 3D Space UI
		{
			projToUse = _camera.GetProjMatrix();
			viewToUse = _camera.GetViewMatrix();
			srtToUse = transform.GetWorldMatrix();
		}
		else // Screen Space UI
		{
			projToUse = OrthoProjection;
			viewToUse = IdentityMat;
			srtToUse = transform.GetLocalMatrix();
		}


		GLuint spriteTextureID = TextureManager.GetTexture(Sprite.SpriteTexture);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spriteTextureID);

		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "RenderSprite");

		if (Sprite.SpriteTexture == DEFAULT_ASSETS["None.dds"])
		{
			glUniform1f(uniform1, false);
		}
		else
		{
			glUniform1f(uniform1, true);
		}

		Draw(projToUse, viewToUse, srtToUse, shader);

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

	glReadPixels(true_mousepos.x, true_mousepos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);


#endif // _BUILD

	int selectedID = data[0] +
		data[1] * 256 +
		data[2] * 256 * 256;

	//std::cout << selectedID << "\n";

	if (spriteToColourPick && (selectedID > 0) && (selectedID != 13421772) )
	{
		Engine::UUID EUID_Index = EUID_Holder[selectedID - offset];
		Tag& entity_tag = currentScene.Get<Tag>(EUID_Index);
		//PRINT(entity_tag.name, "\n");
		//std::cout << entity_tag.name << "\n";


	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT);

}

void ColourPicker::Draw(glm::mat4 _projection , glm::mat4 _view , glm::mat4 _srt , GLSLShader& _shader)
{
	glUniformMatrix4fv(glGetUniformLocation(_shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(_projection));

	glUniformMatrix4fv(glGetUniformLocation(_shader.GetHandle(), "view"),
		1, GL_FALSE, glm::value_ptr(_view));

	glUniformMatrix4fv(glGetUniformLocation(_shader.GetHandle(), "SRT"),
		1, GL_FALSE, glm::value_ptr(_srt));

	renderQuad(Renderer_quadVAO, Renderer_quadVBO);

}