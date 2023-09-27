#pragma once

#include "Core/SystemInterface.h"

ENGINE_SYSTEM(AudioSystem) {
public:
	void Init();
	void Update(float dt);
	void Exit();
};