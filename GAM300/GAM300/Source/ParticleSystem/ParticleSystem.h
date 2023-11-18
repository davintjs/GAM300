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

class Random
{
public:
    Random()
    {
        srand(static_cast<unsigned int>(time(0)));
    }

    int NextInt(int min, int max)
    {
        return min + rand() % (max - min + 1);
    }

    float NextFloat1(float min, float max)
    {
        return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    }

    float NextFloat()
    {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }

    vec3 NextVector3(float min, float max)
    {
        float x = NextFloat() * (max - min) + min;
        float y = NextFloat() * (max - min) + min;
        float z = NextFloat() * (max - min) + min;
        return vec3(x, y, z);
    }
};


ENGINE_SYSTEM(ParticleManager){
public:
	void Init();
	void Update(float dt);
	void Exit();


};

#endif


