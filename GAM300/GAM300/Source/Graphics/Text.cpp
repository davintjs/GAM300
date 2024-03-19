#include <Precompiled.h>
#include "GraphicsHeaders.h"
#include <ft2build.h>
#include "Scene/SceneManager.h"
#include FT_FREETYPE_H

#define AR 1.777778f

void TextSystem::Init()
{
	//LoadFontAtlas("Assets/Fonts/Xolonium-Bold.font"); // decompile
	EVENTS.Subscribe(this, &TextSystem::CallbackFontAssetLoaded);

	glGenVertexArrays(1, &txtVAO);
	glGenBuffers(1, &txtVBO);

	glBindVertexArray(txtVAO);
	glBindBuffer(GL_ARRAY_BUFFER, txtVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(float), vertices, GL_DYNAMIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribDivisor(0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TextSystem::Update(float dt)
{
	
}

void TextSystem::Exit()
{

}

void TextSystem::GenerateTextureAtlas(const Engine::GUID<FontAsset>& _guid, TextSystem::FontCharacters& characters) {

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

	// Cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	mFontAtlasContainer.insert(std::make_pair(_guid, atlasTextureID));

}

void TextSystem::RenderScreenSpace(Scene& _scene, BaseCamera& _camera, const Engine::UUID& _euid, const glm::mat4& _canvasMtx)
{
	GLSLShader& txtshader = SHADER.GetShader(SHADERTYPE::TEXT);
	txtshader.Use();

	glm::mat4 OrthoProjection = glm::ortho(-1.f * AR, 1.f * AR, -1.f, 1.f, -10.f, 10.f);
	glUniformMatrix4fv(glGetUniformLocation(txtshader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(OrthoProjection));

	TextRenderer& text = _scene.Get<TextRenderer>(_euid);
	Transform& transform = _scene.Get<Transform>(_euid);

	glUniformMatrix4fv(glGetUniformLocation(txtshader.GetHandle(), "SRT"),
		1, GL_FALSE, glm::value_ptr(_canvasMtx * transform.GetWorldMatrix()));

	glUniform1f(glGetUniformLocation(txtshader.GetHandle(), "AlphaScaler"),
		text.color.w);

	glUniform3f(glGetUniformLocation(txtshader.GetHandle(), "textColor"), text.color.x, text.color.y, text.color.z);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(txtVAO);

	//RenderTextFromString(text);
	RenderText(text);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	txtshader.UnUse();
}


void TextSystem::RenderWorldSpace(Scene& _scene, BaseCamera& _camera, const Engine::UUID& _euid)
{
	GLSLShader& txtshader = SHADER.GetShader(SHADERTYPE::TEXT_WORLD);
	txtshader.Use();

	glUniformMatrix4fv(glGetUniformLocation(txtshader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));

	glUniformMatrix4fv(glGetUniformLocation(txtshader.GetHandle(), "view"),
		1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));

	TextRenderer& text = _scene.Get<TextRenderer>(_euid);
	Transform& transform = _scene.Get<Transform>(_euid);

	glUniformMatrix4fv(glGetUniformLocation(txtshader.GetHandle(), "SRT"),
		1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix()));

	glUniform1f(glGetUniformLocation(txtshader.GetHandle(), "AlphaScaler"),
		text.color.w);

	glUniform3f(glGetUniformLocation(txtshader.GetHandle(), "textColor"), text.color.x, text.color.y, text.color.z);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(txtVAO);

	//RenderTextFromString(text);
	RenderText(text);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);


	txtshader.UnUse();
}

void TextSystem::RenderText(TextRenderer& text)
{
	allVertices.clear();

	unsigned int currentLine = 1;
	float x = text.x, width, currentWidth, newLineWidth;
	width = currentWidth = newLineWidth = 0.f;
	float y = text.y;
	float scale = text.fontSize * 0.001f;
	float baseHeight = mFontContainer.at(text.guid).at('A').Size.y * scale;

	if (text.text.size() != text.charCount)
	{
		float tempWidth = 0.f;
		text.charCount = text.text.size();
		std::string::const_iterator c;
		for (c = text.text.begin(); c != text.text.end(); c++)
		{
			Character& ch = mFontContainer.at(text.guid).at(*c);
			tempWidth += (ch.Advance >> 6) * scale;
		}

		if (!text.wrapping)
		{
			text.width = tempWidth;
			text.height = baseHeight;
		}
		else
			text.height = baseHeight * text.lineCount;
	}

	// iterate through all characters
	std::string::const_iterator c, nextc;
	for (c = text.text.begin(); c != text.text.end(); c++)
	{
		Character& ch = mFontContainer.at(text.guid).at(*c);

		float xpos = x + ch.Bearing.x * scale;

		float ypos = y - (ch.Bearing.y * 2.f) * scale;
		if(*c == 'p')
			ypos = y - (ch.Bearing.y) * scale;

		float nextChWidth = 0.f;
		bool updateNewLine = false;

		nextc = c;
		nextc++;
		currentWidth += (ch.Advance >> 6) * scale;
		if (nextc != text.text.end())
		{
			Character& nextCh = mFontContainer.at(text.guid).at(*nextc);
			nextChWidth = (nextCh.Advance >> 6) * scale;

			if (currentWidth + nextChWidth > text.width)
			{
				updateNewLine = true;
				newLineWidth = 0.f;
				std::string::const_iterator cit;
				for (cit = c; cit != text.text.end(); cit++)
				{
					Character& ch = mFontContainer.at(text.guid).at(*cit);
					newLineWidth += (ch.Advance >> 6) * scale;
				}
			}
		}

		if (text.centerAlign && text.wrapping)
		{
			xpos -= text.width * 0.5f;
			ypos -= baseHeight * currentLine;

			if (text.lineCount == currentLine)
			{
				xpos = x + ch.Bearing.x * scale - newLineWidth * 0.5f;
			}
		}
		else if (text.centerAlign)
		{
			xpos -= text.width * 0.5f;
		}
		else if (text.wrapping)
		{
			ypos -= baseHeight * currentLine;
		}

		if (updateNewLine)
		{
			x = text.x - (ch.Advance >> 6) * scale;
			currentWidth = 0.f;
			currentLine++;
		}

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos,       ch.AtlasCoordsMin.x, ch.AtlasCoordsMax.y },
			{ xpos,     ypos,       ch.AtlasCoordsMin.x, ch.AtlasCoordsMax.y },
			{ xpos,     ypos + h,   ch.AtlasCoordsMin.x, ch.AtlasCoordsMin.y },
			{ xpos + w, ypos,       ch.AtlasCoordsMax.x, ch.AtlasCoordsMax.y },
			{ xpos + w, ypos + h,   ch.AtlasCoordsMax.x, ch.AtlasCoordsMin.y },
			{ xpos + w, ypos + h,   ch.AtlasCoordsMax.x, ch.AtlasCoordsMin.y }
		};
		for (int i = 0; i < 6; ++i)
			allVertices.insert(allVertices.end(), vertices[i], vertices[i] + 4);

		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}

	text.lineCount = currentLine;

	// Update content of VBO memory with the new vertices and texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, txtVBO);
	glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(float), allVertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, mFontAtlasContainer.at(text.guid));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, text.text.size() * 6);
}

void TextSystem::RenderTextFromString(TextRenderer const& text)
{
	// Clear previous data
	allVertices.clear();
	//allTextures.clear();

	float xoffset{ text.x }, yoffset { text.y };
	float fontSize { 0.001f * text.fontSize };
	float maxWidth { 0.01f * text.width };
	int iter{ 0 }, newlineIter{ 0 };
	float charHeight = mFontContainer.at(text.guid).at('A').Size.y;
	bool newlinefound{ false };

	for (const char& c : text.text) {
		Character ch = mFontContainer.at(text.guid).at(c);  // Use .at() to ensure the character exists in the map

		float xpos, ypos;
		xpos = xoffset + ch.Bearing.x * fontSize;

		//if (c == ' ')
		//{
		//	int internalIter{ iter };
		//	char tempChar = text.text[iter + 1];
		//	float wordWidth{ xpos };
		//	while (tempChar != ' ')
		//	{
		//		wordWidth += mFontContainer.at(text.guid).at(tempChar).Size.x * fontSize;

		//		if (wordWidth > maxWidth + text.x)
		//		{
		//			yoffset -= charHeight * fontSize * 2 * text.leading;
		//			xoffset = text.x;
		//			xpos = xoffset + ch.Bearing.x * fontSize;
		//			break;
		//		}

		//		++internalIter;

		//		if (internalIter >= text.text.size())
		//			break;

		//		tempChar = text.text[internalIter]; // move on
		//	}
		//}


		if (iter == newlineIter)
		{
			yoffset -= charHeight * fontSize * 2;
			//xoffset = text.x;
			//xpos = xoffset + ch.Bearing.x * fontSize;

			if (!text.centerAlign)
			{
				xoffset = text.x;
				xpos = xoffset + ch.Bearing.x * fontSize;
			}

			newlinefound = false;

			char internalC = text.text[iter];
			if (internalC == ' ')
			{
				++iter;
				continue;
			}
		}


		if (!newlinefound)
		{
			int internalIter{ iter };
			float currlineWidth{ 0.f };
			while (internalIter < text.text.size())
			{

				int wordIter{ internalIter };
				char tempChar = text.text[internalIter];
				float wordWidth{ 0.f };
				while (tempChar != ' ')
				{
					wordWidth += mFontContainer.at(text.guid).at(tempChar).Size.x * fontSize;

					if (wordWidth + currlineWidth >= maxWidth)
					{
						// new xpos/offset
						newlinefound = true;
						newlineIter = internalIter;
						currlineWidth -= mFontContainer.at(text.guid).at(' ').Size.x * fontSize;
						break;
					}

					++wordIter;

					if (wordIter >= text.text.size()) // if next letter out of bounds
					{
						// new xpos/offset
						currlineWidth += wordWidth;
						newlinefound = true; 
						break;
					}

					tempChar = text.text[wordIter]; // move on
				}
				//}
				if (newlinefound)
				{
					if (text.centerAlign)
					{
						xoffset = text.x + (maxWidth - currlineWidth) * 0.5f;
						xpos = xoffset + ch.Bearing.x * fontSize;
					}
					break;
				}

				char internalC = text.text[internalIter];
				////currlineWidth += mFontContainer.at(text.guid).at(internalC).Size.x * fontSize;
				if (internalC == ' ')
				{
					currlineWidth += mFontContainer.at(text.guid).at(' ').Size.x * fontSize;
				}

				currlineWidth += wordWidth;
				internalIter = wordIter;

				++internalIter;
			}

		}

		ypos = yoffset - (ch.Bearing.y * 2.f) * fontSize;
			
		float w = ch.Size.x * fontSize;
		float h = ch.Size.y * fontSize * 2.f;
		
		// Update content of VBO memory with the new vertices
		float vertices[6][4] = {
			{ xpos,     ypos,       ch.AtlasCoordsMin.x, ch.AtlasCoordsMax.y },
			{ xpos,     ypos,       ch.AtlasCoordsMin.x, ch.AtlasCoordsMax.y },
			{ xpos,     ypos + h,   ch.AtlasCoordsMin.x, ch.AtlasCoordsMin.y },
			{ xpos + w, ypos,       ch.AtlasCoordsMax.x, ch.AtlasCoordsMax.y },
			{ xpos + w, ypos + h,   ch.AtlasCoordsMax.x, ch.AtlasCoordsMin.y },
			{ xpos + w, ypos + h,   ch.AtlasCoordsMax.x, ch.AtlasCoordsMin.y }
		};
		for (int i = 0; i < 6; ++i) {

			allVertices.insert(allVertices.end(), vertices[i], vertices[i] + 4);

		}
		xoffset += (ch.Advance >> 6) * fontSize; // bitshift by 6 to get value in pixels (2^6 = 64)

		++iter;
	}

	// Update content of VBO memory with the new vertices and texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, txtVBO);
	glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(float), allVertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);/**/


	glBindTexture(GL_TEXTURE_2D, mFontAtlasContainer.at(text.guid));
	/*for (size_t i = 0; i < text.text.size(); ++i) {
		glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
	}*/
	unsigned int sz = text.text.size();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, text.text.size() * 6);

	//glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, text.text.size()*6, text.text.size());
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