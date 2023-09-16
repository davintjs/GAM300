#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "Core/SystemInterface.h"
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
	// int morestuff;

};

class Ray3D;

ENGINE_SYSTEM(GraphicsSystem)
{
public:
	void Init();
	void Update(float dt);
	void Draw();
	void Draw_Meshes(GLuint vaoid ,  unsigned int instance_count , 
		unsigned int prim_count , GLenum prim_type, LightProperties LightSource,
		Materials Mat);
	void Exit();

	bool Raycasting(Ray3D& _ray);
};
#endif // !GRAPHICS_SYSTEM_H