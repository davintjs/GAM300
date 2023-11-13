#include "Scene/Components.h"  

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#define PARTICLE ParticleManager::Instance()

//class Particle
//{
//public: 
//	Particle() {}
//	Particle(const vec3& position, const vec3& velocity, const vec3& acceleration, float lifetime)
//		: position(position), velocity(velocity), acceleration(acceleration), lifetime(lifetime) {}
//	vec3 position;
//	vec3 velocity;
//	vec3 acceleration;
//	float lifetime;
//};

ENGINE_SYSTEM(ParticleManager){
public:
	void Init();
	void Update(float dt);
	void Render(BaseCamera & _camera);
	void Exit();
	std::vector<glm::mat4> particleSRT;

};

#endif


