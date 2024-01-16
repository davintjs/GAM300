#ifndef TEXT_H
#define TEXT_H

//#include "Precompiled.h"
#include "glslshader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#define TEXTMANAGER TextManager::Instance()


SINGLETON(TextManager)
{
public:

	struct Character {
		unsigned int TextureID;  // ID handle of the glyph texture
		glm::ivec2   Size;       // Size of glyph
		glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
		unsigned int Advance;    // Offset to advance to next glyph
	};


	void RenderText(GLSLShader& s, std::string text, float x, float y, float scale, glm::vec3 color);

	unsigned int txtVAO, txtVBO;
	std::map<char, Character> Characters;
};

#endif // !TEXT_H