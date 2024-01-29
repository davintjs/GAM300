#include <Precompiled.h>
#include "GraphicsHeaders.h"
#include <ft2build.h>
#include FT_FREETYPE_H

void TextSystem::Init()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face;
	if (FT_New_Face(ft, "Assets/Fonts/opensansbold.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}
	else {
		FT_Set_Pixel_Sizes(face, 0, 48);
		if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

		for (unsigned char c = 0; c < 128; c++)
		{
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
			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				(unsigned int)face->glyph->advance.x
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0); //??
	}
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	

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
}

void TextSystem::RenderText(GLSLShader& s, std::string text, float x, float y, float scale, glm::vec3 color, BaseCamera& _camera)
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

	// iterate through all characters
	std::string::const_iterator c;

	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		float maxsize = std::max(xpos, ypos) + std::max(w, h);
		maxsize *= 2.f;

		// update VBO for each character
		float vertices[4][4] = {
			//{ xpos,     ypos + h,   0.0f, 0.0f },
			//{ xpos,     ypos,       0.0f, 1.0f },
			//{ xpos + w, ypos,       1.0f, 1.0f },

			//{ xpos,     ypos + h,   0.0f, 0.0f },
			//{ xpos + w, ypos,       1.0f, 1.0f },
			//{ xpos + w, ypos + h,   1.0f, 0.0f }

			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }

			//{ -1.f,     1.f,		0.0f, 1.0f },
			//{ -1.f,     -1.f,		0.0f, 0.0f },
			//{ 1.f,		1.f,       1.0f, 1.0f },
			//{ 1.f,		-1.f,		1.0f, 0.0f }

			//{ (xpos / maxsize) -1.f,			((ypos + h) / maxsize) -1.f,   0.0f, 0.0f },
			//{ (xpos / maxsize) -1.f,			(ypos / maxsize) -1.f,			0.0f, 1.0f },
			//{ ((xpos + w) / maxsize) -1.f,		(ypos / maxsize) -1.f,			1.0f, 1.0f },
			//{ ((xpos + w) / maxsize) -1.f,		((ypos + h) / maxsize) -1.f,   1.0f, 0.0f }
		};


		//float maxwh = std::max(w, h);
		//Vector3 translation_{ xpos / maxsize, ypos / maxsize, 1.f};
		//Vector3 rotation_{};
		//Vector3 scale_{ scale * w/maxwh, scale * h/maxwh, scale };
		//glm::mat4 rot = glm::toMat4(glm::quat(glm::vec3(rotation_)));
		//glm::mat4 srt = glm::translate(glm::mat4(1.0f), glm::vec3(translation_)) * rot *
		//	glm::scale(glm::mat4(1.0f), glm::vec3(scale_));

		//glUniformMatrix4fv(glGetUniformLocation(s.GetHandle(), "SRT"),
		//	1, GL_FALSE, glm::value_ptr(srt));


		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, txtVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
	RenderText(txtshader, "uwu owo @w@ ujtdfgxcg", 0.0f, 0.0f, 1.f, glm::vec3(0.5, 0.8f, 0.2f), _camera);
}