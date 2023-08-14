#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H
#include "Core/SystemInterface.h"

ENGINE_SYSTEM(InputSystem)
{
	void Init();
	void Update();
	void Exit();
};
#endif // !INPUT_SYSTEM_H