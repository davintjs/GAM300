#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "Core/SystemInterface.h"

ENGINE_SYSTEM(GraphicsSystem)
{
public:
	void Init();
	void Update();
	void Exit();
};
#endif // !GRAPHICS_SYSTEM_H