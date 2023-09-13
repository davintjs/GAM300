#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "Core/SystemInterface.h"
#include "glm/mat4x4.hpp"
#include "GL/glew.h"


#define EntityRenderLimit 10000

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


struct InstanceProperties
{
	// rmb to convert everything to AOS
	glm::mat4 entitySRT;
	// int morestuff;

};



ENGINE_SYSTEM(GraphicsSystem)
{
public:
	void Init();
	void Update(float dt);
	void Draw();
	void Draw_Meshes(GLuint vaoid ,  unsigned int instance_count , 
		unsigned int prim_count , GLenum prim_type, LightProperties LightSource);
	void Exit();
};
#endif // !GRAPHICS_SYSTEM_H