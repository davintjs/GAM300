#pragma once
#include "glm/mat4x4.hpp"
#include "GL/glew.h"

#define EntityRenderLimit 200
#define EnitityInstanceLimit 200


// Naive Solution
struct trans_mats
{
	glm::mat4 transformation_mat[EntityRenderLimit];
	int index = 0;
};

struct LightProperties
{
	glm::vec3 lightpos;
	glm::vec3 lightColor;
};


//struct Albedo
//{
//	Texture pointer;
//	glm::vec4 Colour;
//};


struct Settings
{
	glm::vec4 Albedo; // This means colour for now
	glm::vec4 Specular;
	glm::vec4 Diffuse;
	glm::vec4 Ambient;
	float Shininess;
	std::string normalmap;
};

struct Materials 
{
	// Slam Texture Pointer / Reference whatever GUID magic idk in here
	// PADDING IS VERY IMPORTANT THEOPHELIA KUN
	std::string current = "defaulttexture"; // to get dds
	std::unordered_map<std::string, Settings> mSettingsContainer; // GUID, <file name, GLuint>

};

static unsigned int InstancePropertyCount = 1;

struct InstanceProperties
{
	// instanced stuff
	unsigned int VAO;
	// rmb to convert everything to AOS
	unsigned int entitySRTbuffer;
	glm::mat4 entitySRT[EnitityInstanceLimit];
	
	//unsigned int entityMATbuffer;
	//Materials entityMAT[EnitityInstanceLimit];

	// make into individual buffers
	unsigned int AlbedoBuffer;
	glm::vec4 Albedo[EnitityInstanceLimit]; // This means colour for now

	unsigned int SpecularBuffer;
	glm::vec4 Specular[EnitityInstanceLimit];

	unsigned int DiffuseBuffer;
	glm::vec4 Diffuse[EnitityInstanceLimit];

	unsigned int AmbientBuffer;
	glm::vec4 Ambient[EnitityInstanceLimit];

	unsigned int ShininessBuffer;
	float Shininess[EnitityInstanceLimit];


	unsigned int drawCount = 0;
	unsigned int iter = 0;
	unsigned int texture[32];// max 32 dds only
	unsigned int textureCount = 0;
	// buffers and corresponding arrays
	unsigned int entitySRTbuffer;
	glm::mat4 entitySRT[EnitityInstanceLimit];

	unsigned int textureIndexBuffer;
	unsigned int textureIndex[EnitityInstanceLimit];
	
};