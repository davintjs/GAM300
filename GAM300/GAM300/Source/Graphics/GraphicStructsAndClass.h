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

struct Materials
{
	// Slam Texture Pointer / Reference whatever GUID magic idk in here
	// PADDING IS VERY IMPORTANT THEOPHELIA KUN


	glm::vec4 Albedo; // This means colour for now
	glm::vec4 Specular;
	glm::vec4 Diffuse;
	glm::vec4 Ambient;
	float Shininess;
};

static unsigned int InstancePropertyCount = 1;

struct InstanceProperties
{
	// instanced stuff
	unsigned int VAO;
	// rmb to convert everything to AOS
	unsigned int entitySRTbuffer;
	glm::mat4 entitySRT[EnitityInstanceLimit];
	
	unsigned int entityMATbuffer;
	Materials entityMAT[EnitityInstanceLimit];


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