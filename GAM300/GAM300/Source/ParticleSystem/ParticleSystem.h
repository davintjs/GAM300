#include "Scene/Components.h"  

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#define PARTICLE ParticleManager::Instance()

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
    Random random;
    
};

#endif


