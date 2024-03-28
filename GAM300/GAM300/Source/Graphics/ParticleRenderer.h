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
	void Draw2D(BaseCamera & _camera);
	bool compareParticles(const glm::mat4 & particle1, const glm::mat4 & particle2, const glm::vec3 & cameraPosition);
	void SetupInstancedQuad();
	void SetupInstancedCylinder();
	void Exit();
	std::vector<glm::mat4> particleSRT;
	std::vector<glm::vec2> particleLifetimes;
	std::vector<glm::mat4> trailSRT;

	GLuint quadVAO{};
	GLuint quadSRTBuffer{};
	GLuint lifetimeFor2D{};
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