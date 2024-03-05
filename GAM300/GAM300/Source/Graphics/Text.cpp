#include <Precompiled.h>
#include "GraphicsHeaders.h"
#include <ft2build.h>
#include "Scene/SceneManager.h"
#include FT_FREETYPE_H

void TextSystem::Init()
{
	//LoadFontAtlas("Assets/Fonts/Xolonium-Bold.font"); // decompile
	EVENTS.Subscribe(this, &TextSystem::CallbackFontAssetLoaded);
	
	/*for (auto& font : mFontContainer) {
		GenerateTextureAtlas(font.second);
	}*/
	glGenVertexArrays(1, &txtVAO);
	glGenBuffers(1, &txtVBO);

	glBindVertexArray(txtVAO);
	glBindBuffer(GL_ARRAY_BUFFER, txtVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);/**/


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

void TextSystem::GenerateTextureAtlas(const Engine::GUID<FontAsset>& _guid, TextSystem::FontCharacters& characters) {
	//const int atlasSize = 128*8152; // Adjust this as needed

	//// Create a blank texture atlas
	//unsigned int textureAtlas;
	//glGenTextures(1, &textureAtlas);
	//glBindTexture(GL_TEXTURE_2D, textureAtlas);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlasSize, atlasSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	//int xOffset = 0;
	//int yOffset = 0;

	//// Populate the texture atlas with individual character textures
	//for (auto& pair : characters) {
	//	TextSystem::Character& character = pair.second;

	//	// Copy the character's texture to the atlas

	//	//characters[pair.first];

	//	// Update the character's texture coordinates
	//	
	//	character.Texture = textureAtlas;
	//	character.Bearing.x = xOffset;
	//	character.Bearing.y = yOffset;

	//	// Move the offset for the next character
	//	xOffset += character.Size.x;
	//	glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, character.Size.x, character.Size.y, GL_RGBA, GL_UNSIGNED_BYTE, character.TextureData.data());
	//	//glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, character.Size.x, character.Size.y, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	//	std::cout << pair.first << " - it is working\n";
	//	// Check if we need to move to the next row
	//	if (xOffset + character.Size.x > atlasSize) {
	//		xOffset = 0;
	//		yOffset += character.Size.y;
	//	}

	//}

	//// Set texture parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glBindTexture(GL_TEXTURE_2D, 0);

	// Define some constants for padding between characters
	const int padding = 2;

	// Calculate total size required for the atlas
	int totalWidth = 0;
	int maxHeight = 0;
	for (auto& pair : characters) {
		TextSystem::Character& ch = pair.second;
		totalWidth += ch.Size.x + padding;
		maxHeight = std::max(maxHeight, ch.Size.y);
	}

	// Create the texture atlas
	GLuint atlasTextureID;
	glGenTextures(1, &atlasTextureID);
	glBindTexture(GL_TEXTURE_2D, atlasTextureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Set alignment to 1 byte

	// Allocate memory for the texture atlas
	std::vector<unsigned char> atlasData(totalWidth * maxHeight * 4, 0); // 4 components (RGBA)

	// Set texture parameters
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, totalWidth, maxHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlasData.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Fill the atlas with characters
	int xOffset = 0;

	for (auto& pair : characters) {
		TextSystem::Character& ch = pair.second;
		// Copy character texture data to the atlas
		for (int y = 0; y < ch.Size.y; ++y) {
			for (int x = 0; x < ch.Size.x; ++x) {
				int atlasIndex = ((y * totalWidth + xOffset + x));
				int textureIndex = (y * ch.Size.x + x);
				atlasData[atlasIndex] = ch.TextureData[textureIndex];
			}
		}
		ch.AtlasCoordsMin = { (float)(xOffset+ padding)/ (float)(totalWidth*4), 0.f };

		// Update character bearing
		//ch.Bearing.x = xOffset;
		ch.Bearing.y = ch.Size.y - ch.Bearing.y; // Adjust bearing based on top-left origin
		// Move xOffset to the next character
		xOffset += ch.Size.x + padding;

		ch.AtlasCoordsMax = { (float)(xOffset - padding) / (float)(totalWidth*4), (float)ch.Size.y / (maxHeight*4) };

	}


	// Update the texture atlas with the new data
	glBindTexture(GL_TEXTURE_2D, atlasTextureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, totalWidth, maxHeight, GL_RGBA, GL_UNSIGNED_BYTE, atlasData.data());

	// Update character advance for rendering
	//for (auto& pair : characters) {
	//	TextSystem::Character& ch = pair.second;
	//	//ch.Advance = ch.Size.x + padding;
	//}

	// Cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	mFontAtlasContainer.insert(std::make_pair(_guid, atlasTextureID));

}


//void TextSystem::RenderText(GLSLShader& s, std::string text, float x, float y, float scale, glm::vec3 color, BaseCamera& _camera, const Engine::GUID<FontAsset>& _guid)
//{
//	scale *= 0.001f; // hack bc it too big
//
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	// activate corresponding render state	
//	s.Use();
//	//s.SetUniform(,)
//	glUniform3f(glGetUniformLocation(s.GetHandle(), "textColor"), color.x, color.y, color.z);
//	glActiveTexture(GL_TEXTURE0);
//	glBindVertexArray(txtVAO);
//
//	glm::mat4 OrthoProjection = glm::ortho(-1.f, 1.f, -1.f, 1.f, -10.f, 10.f);
//	//s.SetUniform("projection", proj);
//	glUniformMatrix4fv(glGetUniformLocation(s.GetHandle(), "projection"),
//		1, GL_FALSE, glm::value_ptr(OrthoProjection));
//
//	for (const char& c : text) {
//		Character ch = mFontContainer.at(_guid).at(c);  // Use .at() to ensure the character exists in the map
//
//		float xpos = x + ch.Bearing.x * scale;
//		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
//
//		float w = ch.Size.x * scale;
//		float h = ch.Size.y * scale * 2.f;
//		
//		// Generate OpenGL texture from the loaded texture data ->> can optimise so it doesnt get texture every frame
//		GLuint texture;
//		glGenTextures(1, &texture);
//		glBindTexture(GL_TEXTURE_2D, texture);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ch.Size.x, ch.Size.y, 0, GL_RED, GL_UNSIGNED_BYTE, ch.TextureData.data());
//		glGenerateMipmap(GL_TEXTURE_2D);
//
//
//		// Update content of VBO memory with the new vertices
//		float vertices[4][4] = {
//			{ xpos,     ypos,       0.0f, 1.0f },
//			{ xpos,     ypos + h,   0.0f, 0.0f },
//			{ xpos + w, ypos,       1.0f, 1.0f },
//			{ xpos + w, ypos + h,   1.0f, 0.0f }
//		};
//
//		glBindBuffer(GL_ARRAY_BUFFER, txtVBO);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//		// Use the new texture for rendering
//		glBindTexture(GL_TEXTURE_2D, texture);
//
//		// Render quad
//		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//
//		// Delete the generated texture when it's no longer needed
//		glDeleteTextures(1, &texture);
//
//		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
//		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
//
//	}
//	glBindVertexArray(0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//	s.UnUse();
//}

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
	//allTextures.clear();

	float xoffset{ text.x }, yoffset { text.y };
	float fontSize { 0.001f * text.fontSize };
	float maxWidth { 0.01f * text.width };
	//bool newLine{ false };
	int iter{ 0 }; // forgive me im tireed and idk what im doing
	for (const char& c : text.text) {
		Character ch = mFontContainer.at(text.guid).at(c);  // Use .at() to ensure the character exists in the map

		float xpos, ypos;
		xpos = xoffset + ch.Bearing.x * fontSize;

		if (c == ' ')
		{
			int internalIter{ iter };
			char tempChar = text.text[iter + 1];
			float wordWidth{ xpos };
			while (tempChar != ' ')
			{
				wordWidth += mFontContainer.at(text.guid).at(tempChar).Size.x * fontSize;

				if (wordWidth > maxWidth + text.x)
				{
					float charHeight = mFontContainer.at(text.guid).at('A').Size.y;
					yoffset -= charHeight * fontSize * 2 * text.leading;
					xoffset = text.x;
					xpos = xoffset + ch.Bearing.x * fontSize;
					break;
				}

				++internalIter;

				if (internalIter >= text.text.size())
					break;

				tempChar = text.text[internalIter]; // move on
			}
		}

		//// if new character pos exceeds width -> change to if word exceeds, so if space character, check width until next space
		//if ((xpos > maxWidth + text.x) /*|| (c == '\\' && *std::next(&c) == 'n')*/)
		//{
		//	float charHeight = mFontContainer.at(text.guid).at('A').Size.y;
		//	yoffset -= charHeight * fontSize * 2 * text.leading;
		//	xoffset = text.x;
		//	xpos = xoffset + ch.Bearing.x * fontSize;

		//	//if (c == '\\' && *std::next(&c) == 'n') 
		//	//{
		//	//	newLine = true;
		//	//	continue;
		//	//}
		//}

		//if (newLine)
		//{
		//	newLine = false;
		//	continue;
		//}

		ypos = yoffset - (ch.Bearing.y * 2.f) * fontSize;
			
		float w = ch.Size.x * fontSize;
		float h = ch.Size.y * fontSize * 2.f;


		//if (!ch.Texture)
		//{
		//	glGenTextures(1, &ch.Texture);
		//	glBindTexture(GL_TEXTURE_2D, ch.Texture);
		//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ch.Size.x, ch.Size.y, 0, GL_RED, GL_UNSIGNED_BYTE, ch.TextureData.data());
		//	glGenerateMipmap(GL_TEXTURE_2D);

		//}

		//// Update content of VBO memory with the new vertices
		//float vertices[4][4] = {
		//	{ xpos,     ypos,       0.0f, 1.0f },
		//	{ xpos,     ypos + h,   0.0f, 0.0f },
		//	{ xpos + w, ypos,       1.0f, 1.0f },
		//	{ xpos + w, ypos + h,   1.0f, 0.0f }
		//};
		
		// Update content of VBO memory with the new vertices
		float vertices[4][4] = {
			{ xpos,     ypos,       ch.AtlasCoordsMin.x, ch.AtlasCoordsMax.y },
			{ xpos,     ypos + h,   ch.AtlasCoordsMin.x, ch.AtlasCoordsMin.y },
			{ xpos + w, ypos,       ch.AtlasCoordsMax.x, ch.AtlasCoordsMax.y },
			{ xpos + w, ypos + h,   ch.AtlasCoordsMax.x, ch.AtlasCoordsMin.y }
		};

		for (int i = 0; i < 4; ++i) {
			allVertices.insert(allVertices.end(), vertices[i], vertices[i] + 4);
		}
		//allTextures.push_back(ch.Texture);

		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		xoffset += (ch.Advance >> 6) * fontSize; // bitshift by 6 to get value in pixels (2^6 = 64)

		++iter;
	}

	// Update content of VBO memory with the new vertices and texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, txtVBO);
	glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(float), allVertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//// Use the new texture for rendering
	//for (size_t i = 0; i < allTextures.size(); ++i) {
	//	glBindTexture(GL_TEXTURE_2D, allTextures[i]);
	//	glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
	//}
	// Use the new texture for rendering

	glBindTexture(GL_TEXTURE_2D, mFontAtlasContainer.at(text.guid));
	for (size_t i = 0; i < text.text.size(); ++i) {
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

	GenerateTextureAtlas(_guid, Characters);
	mFontContainer.insert(std::make_pair(_guid, Characters));

	inFile.close();
}

void TextSystem::CallbackFontAssetLoaded(AssetLoadedEvent<FontAsset>* pEvent)
{
	AddFont(pEvent->asset.mFilePath, pEvent->asset.importer->guid);
}