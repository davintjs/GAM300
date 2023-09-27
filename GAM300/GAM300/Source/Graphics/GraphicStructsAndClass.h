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
	// store guid also
	std::string current = "defaulttexture"; // to get dds
	std::unordered_map<std::string, Settings> mSettingsContainer; // GUID, <file name, GLuint>

};

static unsigned int InstancePropertyCount = 1;

struct InstanceProperties
{
	// instanced stuff
	unsigned int VAO;
	unsigned int debugVAO;
	// rmb to convert everything to AOS
	unsigned int entitySRTbuffer;
	glm::mat4 entitySRT[EnitityInstanceLimit];
	
	// -------------- BLINN PHONG --------------------------
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

	// -------------- PBR --------------------------
	// USES Albedo Buffer from above too
	// 
	// make into individual buffers
	//unsigned int AlbedoBuffer;
	//glm::vec4 Albedo[EnitityInstanceLimit]; // This means colour for now

	unsigned int Metal_Rough_AO_Texture_Buffer;
	glm::vec3 M_R_A_Texture[EnitityInstanceLimit];

	unsigned int Metal_Rough_AO_Texture_Constant;
	glm::vec3 M_R_A_Constant[EnitityInstanceLimit]{ glm::vec3(1.f,1.f,1.f) };





	unsigned int textureIndexBuffer;
	glm::vec2 textureIndex[EnitityInstanceLimit]; // (texture index, normal map index)

	unsigned int drawCount = 0;
	unsigned int iter = 0;
	unsigned int texture[32];// max 32 dds only
	unsigned int textureCount = 0;
	
	void BatchTexture(std::string texture);
};