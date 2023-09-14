#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "Core/SystemInterface.h"
#include "glm/mat4x4.hpp"

#define EntityRenderLimit 10000

struct InstanceProperties
{
	// rmb to convert everything to AOS
	GLuint entitySRT;
	glm::mat4 entitySRT[EntityRenderLimit];
	// int morestuff;

};

ENGINE_SYSTEM(GraphicsSystem)
{
public:
	void Init();
	void Update(float dt);
	void Draw();
	void Exit();
};
#endif // !GRAPHICS_SYSTEM_H