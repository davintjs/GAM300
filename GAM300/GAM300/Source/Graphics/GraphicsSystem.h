#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "Core/SystemInterface.h"

#define EntityRenderLimit 10000

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
	void Exit();
};
#endif // !GRAPHICS_SYSTEM_H