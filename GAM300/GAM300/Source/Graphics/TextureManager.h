#pragma once

#include "../Core/SystemInterface.h"
#include "../gli-master/gli/gli.hpp"
#include "glslshader.h"

#define TextureManager Texture_Manager::Instance()

SINGLETON(Texture_Manager)
{
public:
	void Init();
	void Update(float dt);
	void Exit();

	void AddTexture(char const* Filename, std::string GUID);
	GLuint& GetTexture(std::string GUID);

private:

	std::unordered_map<std::string, std::pair<std::string, unsigned int>> mTextureContainer; // GUID, <file name, GLuint>
	
	GLuint CreateTexture(char const* Filename);
};

