/*!***************************************************************************************
\file			TextureCompiler.h
\project
\author         Euphrasia Theophelia Tan Ee Mun

\par			Course: GAM300
\date           08/12/2023

\brief
	This file contains the declarations of the following:
	1. Loading of Texture files
	2. Creating a dds version of the file

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "FontCompiler.h"

#include <ft2build.h>
#include FT_FREETYPE_H

void LoadFont(const std::filesystem::path& _filePath) {
	// existing code to initialize FreeType, load font face, etc.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face;
	const char* fontPath = _filePath.string().c_str();
	if (FT_New_Face(ft, fontPath, 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}
	else {
		FT_Set_Pixel_Sizes(face, 0, 256);
		if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction


		std::map<char, TextSystem::Character> tempCharacters;

		for (unsigned char c = 0; c < 128; c++) {
			// load character glyph, generate texture, and store in Characters map

			// load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Store character with texture data in Characters map
			std::vector<unsigned char> textureData(
				face->glyph->bitmap.buffer,
				face->glyph->bitmap.buffer + face->glyph->bitmap.width * face->glyph->bitmap.rows
			);

			TextSystem::Character character = {
				std::move(textureData),
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};

			tempCharacters.insert(std::pair<char, TextSystem::Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0); //??

		//const char* outputPath = "Assets/Fonts/opensansbold.bin";
		std::filesystem::path ttfPath{ _filePath };
		ttfPath.replace_extension("font");

		// Save Characters map to binary file
		std::ofstream outFile(ttfPath, std::ios::binary);
		for (const auto& pair : tempCharacters) {
			outFile.write(reinterpret_cast<const char*>(&pair.first), sizeof(char));
			outFile.write(reinterpret_cast<const char*>(&pair.second.Size), sizeof(glm::ivec2));
			outFile.write(reinterpret_cast<const char*>(&pair.second.Bearing), sizeof(glm::ivec2));
			outFile.write(reinterpret_cast<const char*>(&pair.second.Advance), sizeof(unsigned int));
			outFile.write(reinterpret_cast<const char*>(pair.second.TextureData.data()), pair.second.TextureData.size());
		}
		outFile.close();

	}
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}