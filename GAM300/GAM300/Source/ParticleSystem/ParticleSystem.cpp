#include "Precompiled.h"
#include "Scene/SceneManager.h"
#include "ParticleSystem.h"
#include "Graphics/MeshManager.h"
#include "Scene/Components.h" 
#include "Graphics/GraphicsHeaders.h"





void ParticleManager::Init()
{

    
}


void ParticleManager::Update(float dt)
{
    Scene& currentScene = SceneManager::Instance().GetCurrentScene();
    for (ParticleComponent& particleComponent : currentScene.GetArray<ParticleComponent>()) {
        
        Entity& entity = currentScene.Get<Entity>(particleComponent);
        Transform& entityTransform = currentScene.Get<Transform>(entity);
        float elapsedTimeSinceLastEmission = dt;
        float timeBetweenEmissions = 1.0f / particleComponent.particleEmissionRate_;
        Random random; 

        if (particleComponent.particles_.empty()) { // initialize
            particleComponent.particles_.resize(particleComponent.numParticles_);
            // Initialize the position, velocity, and acceleration of each particle
            for (int i = 0; i < particleComponent.numParticles_; i++) {
                particleComponent.particles_[i].position = entityTransform.GetTranslation();
                //particleComponent.particles_[i].direction = glm::vec3(0.f, 1.f, 0.f); // @desmond randomize this
                particleComponent.particles_[i].direction = random.NextVector3(0.0f, 1.0f); 
                particleComponent.particles_[i].velocity = glm::vec3(0.f, 1.f, 0.f);
                particleComponent.particles_[i].acceleration = 5.0f;
                //particleComponent.particles_[i].lifetime = particleComponent.particleLifetime_;
                particleComponent.particles_[i].lifetime = random.NextFloat1(1.0f, particleComponent.particleLifetime_);
                particleComponent.particles_[i].scale += dt * particleComponent.particleScaleRate_;
                particleComponent.particles_[i].speed = particleComponent.speed_;
            }
        }

        for (int i = 0; i < particleComponent.numParticles_; i++) {

            // speed += acceleration * dt
            // velocity = speed * direction

            particleComponent.particles_[i].speed += particleComponent.particles_[i].acceleration * dt;
            particleComponent.particles_[i].velocity = particleComponent.particles_[i].direction * particleComponent.particles_[i].speed;
            particleComponent.particles_[i].position += particleComponent.particles_[i].velocity * dt;
            particleComponent.particles_[i].lifetime -= dt;

            particleComponent.particles_[i].scale += dt * particleComponent.particleScaleRate_;
            particleComponent.particles_[i].scale = glm::clamp(particleComponent.particles_[i].scale, particleComponent.particleMinScale_, particleComponent.particleMaxScale_);

        }
        // Handle particle collisions
        // Emit new particles
        if (particleComponent.particleEmissionRate_ > 0.0f) { // idk how emmision rate works, @desmond your turn to do this
            for (int i = 0; i < particleComponent.numParticles_; i++) {
                if (particleComponent.particles_[i].lifetime <= 0.0f) {

                    // randomize vec3 direction (normalized)
                    //      can use glm::normalize() after calculating a random direction
                    
                    particleComponent.particles_[i].position = entityTransform.GetTranslation(); // to entity's position
                    particleComponent.particles_[i].velocity = glm::vec3(0.f, 0.f, 0.f);
                    //particleComponent.particles_[i].direction = glm::vec3(0.f, 1.f, 0.f); // @desmond randomize this
                    particleComponent.particles_[i].direction = random.NextVector3(0.0f, 1.0f);
                    particleComponent.particles_[i].acceleration = 5.0f;
                    //particleComponent.particles_[i].lifetime = particleComponent.particleLifetime_; // maybe randomize lifetime also, max lifetime should be particleComponent.particleLifetime_
                    particleComponent.particles_[i].lifetime = random.NextFloat1(1.0f, particleComponent.particleLifetime_); 
                    particleComponent.particles_[i].scale = particleComponent.particleMinScale_;
                    particleComponent.particles_[i].speed = particleComponent.speed_;

                }
            }
        }


        //while (elapsedTimeSinceLastEmission >= timeBetweenEmissions) 
        //{
        //    elapsedTimeSinceLastEmission -= timeBetweenEmissions;

        //    // Emit a new particle
        //    for (int i = 0; i < particleComponent.numParticles_; i++) 
        //    {
        //        if (particleComponent.particles_[i].lifetime <= 0.0f)
        //        {
        //            particleComponent.particles_[i].position = entityTransform.GetTranslation();
        //            particleComponent.particles_[i].velocity = glm::vec3(1.0f, 0.f, 0.f);
        //            particleComponent.particles_[i].acceleration = glm::vec3(0.0f);
        //            particleComponent.particles_[i].lifetime = particleComponent.particleLifetime_;
        //            particleComponent.particles_[i].scale = particleComponent.particleMinScale_;
        //        }
        //    }
        //}
    }
}

void ParticleManager::Exit()
{
    // Delete the particle system.
    //delete particleSystem;
}



    