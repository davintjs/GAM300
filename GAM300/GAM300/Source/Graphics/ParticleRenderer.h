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
	void SetupInstancedCylinder();
	void Exit();
	std::vector<glm::mat4> particleSRT;
	std::vector<glm::mat4> trailSRT;

	GLuint quadVAO{};
	GLuint quadSRTBuffer{};

	GLuint cylVAO{};
	GLuint cylSRTBuffer{};
	GLuint cylsize{};

	bool hasTexture{false};
	bool hasNormal{false};
	bool hasRoughness{false};
	bool hasMetallic{false};
	bool hasAO{false};
	bool hasEmission{false};
};

#endif