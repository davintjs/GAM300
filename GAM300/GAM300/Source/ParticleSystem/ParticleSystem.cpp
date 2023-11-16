#include "Precompiled.h"
#include "Scene/SceneManager.h"
#include "ParticleSystem.h"
#include "Graphics/MeshManager.h"
#include "Scene/Components.h" 
#include "Graphics/GraphicsHeaders.h"



void ParticleManager::Init()
{
    // Create a particle system.
    //particleSystem = new ParticleSystem();  
    //// Set the particle system's properties.
    //particleSystem->Initialize(1000, 1.0f, 10.0f);
    
}


void ParticleManager::Update(float dt)
{
    // Update the particle system.
    //particleSystem->Update(dt);
    //// Render the particle system.
    //particleSystem->Render();

    Scene& currentScene = SceneManager::Instance().GetCurrentScene();
    for (ParticleComponent& particleComponent : currentScene.GetArray<ParticleComponent>()) {
        
        //particleComponent.Update(dt);
        Entity& entity = currentScene.Get<Entity>(particleComponent);
        Transform& entityTransform = currentScene.Get<Transform>(entity);

        if (particleComponent.particles_.empty()) { // initialize
            particleComponent.particles_.resize(particleComponent.numParticles_);
            // Initialize the position, velocity, and acceleration of each particle
            for (int i = 0; i < particleComponent.numParticles_; i++) {
                particleComponent.particles_[i].position = entityTransform.GetTranslation();
                particleComponent.particles_[i].velocity = glm::vec3(1.0f, 0.f, 0.f); // @kk changed this to test, pls do change back
                particleComponent.particles_[i].acceleration = glm::vec3(0.0f);
                particleComponent.particles_[i].lifetime = particleComponent.particleLifetime_;
            }
        }

        for (int i = 0; i < particleComponent.numParticles_; i++) {
            particleComponent.particles_[i].position += particleComponent.particles_[i].velocity * dt;
            particleComponent.particles_[i].velocity += particleComponent.particles_[i].acceleration * dt;
            particleComponent.particles_[i].lifetime -= dt;
        }
        // Handle particle collisions
        // Emit new particles
        if (particleComponent.particleEmissionRate_ > 0.0f) { // idk how emmision rate works, @desmond your turn to do this
            for (int i = 0; i < particleComponent.numParticles_; i++) {
                if (particleComponent.particles_[i].lifetime <= 0.0f) { // reset, same as line 38, maybe can optimize
                    particleComponent.particles_[i].position = entityTransform.GetTranslation(); // to entity's position
                    //particles_[i].velocity = glm::vec3(0.0f); // shouldnt be 0
                    //particleComponent.particles_[i].acceleration = glm::vec3(0.0f); // shouldnt be 0?
                    particleComponent.particles_[i].lifetime = particleComponent.particleLifetime_;
                }
            }
        }
    }
}

void ParticleManager::Exit()
{
    // Delete the particle system.
    //delete particleSystem;
}

    