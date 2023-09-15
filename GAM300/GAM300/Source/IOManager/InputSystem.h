#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H
#include "Core/SystemInterface.h"

ENGINE_SYSTEM(InputSystem)
{
public:
	void Init();
	void Update(float dt);
	void Exit();
};
#endif // !INPUT_SYSTEM_H