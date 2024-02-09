#include <Precompiled.h>
#include "GraphicsHeaders.h"
#include <ft2build.h>
#include "Scene/SceneManager.h"
#include FT_FREETYPE_H

void TextSystem::Init()
{
	//LoadFontAtlas("Assets/Fonts/Xolonium-Bold.font"); // decompile
	EVENTS.Subscribe(this, &TextSystem::CallbackFontAssetLoaded);
	

	glGenVertexArrays(1, &txtVAO);
	glGenBuffers(1, &txtVBO);

	glBindVertexArray(txtVAO);
	glBindBuffer(GL_ARRAY_BUFFER, txtVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TextSystem::Update(float dt)
{
	// Empty by design
}

void TextSystem::Exit()
{
	// Empty by design

	//glDeleteTextures(1, &texture);
}

void TextSystem::RenderText(GLSLShader& s, std::string text, float x, float y, float scale, glm::vec3 color, BaseCamera& _camera, const Engine::GUID<FontAsset>& _guid)
{
	scale *= 0.001f; // hack bc it too big

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// activate corresponding render state	
	s.Use();
	//s.SetUniform(,)
	glUniform3f(glGetUniformLocation(s.GetHandle(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(txtVAO);

	glm::mat4 OrthoProjection = glm::ortho(-1.f, 1.f, -1.f, 1.f, -10.f, 10.f);
	//s.SetUniform("projection", proj);
	glUniformMatrix4fv(glGetUniformLocation(s.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(OrthoProjection));

	for (const char& c : text) {
		Character ch = mFontContainer.at(_guid).at(c);  // Use .at() to ensure the character exists in the map

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale * 2.f;
		
		// Generate OpenGL texture from the loaded texture data ->> can optimise so it doesnt get texture every frame
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ch.Size.x, ch.Size.y, 0, GL_RED, GL_UNSIGNED_BYTE, ch.TextureData.data());
		glGenerateMipmap(GL_TEXTURE_2D);


		// Update content of VBO memory with the new vertices
		float vertices[4][4] = {
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		glBindBuffer(GL_ARRAY_BUFFER, txtVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Use the new texture for rendering
		glBindTexture(GL_TEXTURE_2D, texture);

		// Render quad
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Delete the generated texture when it's no longer needed
		glDeleteTextures(1, &texture);

		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)

	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	s.UnUse();
}

void TextSystem::Draw(BaseCamera& _camera)
{
	GLSLShader& txtshader = SHADER.GetShader(SHADERTYPE::TEXT);

	RenderText_WorldSpace(_camera);
	//RenderText_ScreeninWorldSpace(_camera);
	RenderText_ScreenSpace(_camera);

}

void TextSystem::RenderText_ScreenSpace(BaseCamera& _camera)
{
	// Initializing Variables

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_DEPTH_BUFFER_BIT);

	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	GLSLShader& txtshader = SHADER.GetShader(SHADERTYPE::TEXT);
	txtshader.Use();

	glm::mat4 OrthoProjection = glm::ortho(-1.f, 1.f, -1.f, 1.f, -10.f, 10.f);
	glUniformMatrix4fv(glGetUniformLocation(txtshader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(OrthoProjection));

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

	for (TextRenderer& text : currentScene.GetArray<TextRenderer>())
	{
		// Maybe need a worldspace checker?

		if (text.guid == 0)
			continue;
		
		Entity& entity = currentScene.Get<Entity>(text);
		if (!currentScene.IsActive(entity)) continue;

		Transform& transform = currentScene.Get<Transform>(entity);
		
		if (!transform.parent || _camera.GetCameraType() == CAMERATYPE::SCENE) continue;

		glm::mat4 canvasMatrix = glm::inverse(canvasTransform->GetWorldMatrix());

		glUniformMatrix4fv(glGetUniformLocation(txtshader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(canvasMatrix * transform.GetWorldMatrix()));

		glUniform1f(glGetUniformLocation(txtshader.GetHandle(), "AlphaScaler"),
			text.alpha);

		glUniform3f(glGetUniformLocation(txtshader.GetHandle(), "textColor"), text.r, text.g, text.b);

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(txtVAO);

		RenderTextFromString(text);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	txtshader.UnUse();

}


void TextSystem::RenderText_WorldSpace(BaseCamera& _camera)
{
	// Initializing Variables

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	GLSLShader& txtshader = SHADER.GetShader(SHADERTYPE::TEXT_WORLD);
	txtshader.Use();

	glUniformMatrix4fv(glGetUniformLocation(txtshader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));

	glUniformMatrix4fv(glGetUniformLocation(txtshader.GetHandle(), "view"),
		1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));

	for (TextRenderer& text : currentScene.GetArray<TextRenderer>()) 
	{
		// Maybe need a worldspace checker?

		if (text.guid == 0)
			continue;

		Entity& entity = currentScene.Get<Entity>(text);
		if (!currentScene.IsActive(entity)) continue;

		Transform& transform = currentScene.Get<Transform>(entity);

		if (_camera.GetCameraType() != CAMERATYPE::SCENE && transform.parent)
			continue;

		glUniformMatrix4fv(glGetUniformLocation(txtshader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix()));
		
		glUniform1f(glGetUniformLocation(txtshader.GetHandle(), "AlphaScaler"),
			text.alpha);

		glUniform3f(glGetUniformLocation(txtshader.GetHandle(), "textColor"), text.r, text.g, text.b);

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(txtVAO);

		RenderTextFromString(text);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}


	txtshader.UnUse();

}

void TextSystem::RenderTextFromString(TextRenderer const& text)
{
	// Clear previous data
	allVertices.clear();
	allTextures.clear();

	float xoffset = text.x;
	for (const char& c : text.text) {
		Character ch = mFontContainer.at(text.guid).at(c);  // Use .at() to ensure the character exists in the map

		float xpos = xoffset + ch.Bearing.x * (text.fontSize * 0.001f);
		float ypos = text.y - (ch.Size.y - ch.Bearing.y) * (text.fontSize * 0.001f);

		float w = ch.Size.x * (text.fontSize * 0.001f);
		float h = ch.Size.y * (text.fontSize * 0.001f) * 2.f;

		if (!ch.Texture)
		{
			glGenTextures(1, &ch.Texture);
			glBindTexture(GL_TEXTURE_2D, ch.Texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ch.Size.x, ch.Size.y, 0, GL_RED, GL_UNSIGNED_BYTE, ch.TextureData.data());
			glGenerateMipmap(GL_TEXTURE_2D);

		}

		// Update content of VBO memory with the new vertices
		float vertices[4][4] = {
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		for (int i = 0; i < 4; ++i) {
			allVertices.insert(allVertices.end(), vertices[i], vertices[i] + 4);
		}
		allTextures.push_back(ch.Texture);

		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		xoffset += (ch.Advance >> 6) * (text.fontSize * 0.001f); // bitshift by 6 to get value in pixels (2^6 = 64)

	}

	// Update content of VBO memory with the new vertices and texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, txtVBO);
	glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(float), allVertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Use the new texture for rendering
	for (size_t i = 0; i < allTextures.size(); ++i) {
		glBindTexture(GL_TEXTURE_2D, allTextures[i]);
		glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
	}

}


// Function to load font atlas from binary file
void TextSystem::AddFont(const std::filesystem::path& inputPath, const Engine::GUID<FontAsset>& _guid) {
	std::map<char, Character> Characters;
	std::string path = inputPath.string();
	std::replace(path.begin(), path.end(), '\\', '/');

	std::ifstream inFile(path, std::ios::binary);
	if (!inFile.is_open()) {
		std::cerr << "Failed to open font atlas file" << std::endl;
		return;
	}

	char c;
	Character character;
	while (inFile.read(reinterpret_cast<char*>(&c), sizeof(char)) &&
		inFile.read(reinterpret_cast<char*>(&character.Size), sizeof(glm::ivec2)) &&
		inFile.read(reinterpret_cast<char*>(&character.Bearing), sizeof(glm::ivec2)) &&
		inFile.read(reinterpret_cast<char*>(&character.Advance), sizeof(unsigned int))) {

		// Read texture data
		character.TextureData.resize(character.Size.x * character.Size.y);
		inFile.read(reinterpret_cast<char*>(character.TextureData.data()), character.TextureData.size());

		Characters.insert(std::pair<char, Character>(c, character));
	}
	//Characters.insert(std::pair<std::string, std::map<char, Character>>(c, haracter));
	mFontContainer.insert(std::make_pair(_guid, Characters));

	inFile.close();
}

void TextSystem::CallbackFontAssetLoaded(AssetLoadedEvent<FontAsset>* pEvent)
{
	AddFont(pEvent->asset.mFilePath, pEvent->asset.importer->guid);
}