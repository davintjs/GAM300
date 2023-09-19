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

struct Materials
{
	glm::vec4 Albedo;
	glm::vec4 Specular;
	glm::vec4 Diffuse;
	glm::vec4 Ambient;
	float Shininess;
};

static unsigned int InstancePropertyCount = 1;

struct InstanceProperties
{
	unsigned int VAO;
	// rmb to convert everything to AOS
	unsigned int entitySRTbuffer;
	glm::mat4 entitySRT[EnitityInstanceLimit];
	



	unsigned int drawCount = 0;
	unsigned int iter = 0;
};