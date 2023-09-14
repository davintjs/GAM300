#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "Core/SystemInterface.h"

#define EntityRenderLimit 10000

ENGINE_SYSTEM(GraphicsSystem)
{
public:
	void Init();
	void Update(float dt);
	void Exit();
};
#endif // !GRAPHICS_SYSTEM_H