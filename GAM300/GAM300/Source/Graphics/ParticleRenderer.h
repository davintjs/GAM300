#pragma once
#ifndef PARTICLERENDER_H
#define PARTICLERENDER_H

#include "GraphicStructsAndClass.h"
#include "Core/SystemInterface.h"
#include "BaseCamera.h"

#define PARTICLERENDER ParticleRenderer::Instance()

ENGINE_SYSTEM(ParticleRenderer) {
public:
	void Init();
	void Update(float dt);
	void Draw(BaseCamera & _camera);
	void SetupInstancedQuad();
	void Exit();
	std::vector<glm::mat4> particleSRT;
	GLuint quadVAO{};
};

#endif