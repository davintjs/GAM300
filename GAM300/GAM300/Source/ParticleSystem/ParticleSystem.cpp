#include "Precompiled.h"
#include "Scene/SceneManager.h"
#include "ParticleSystem.h"
#include "Graphics/MeshManager.h"
#include "Scene/Components.h" 
#include "Graphics/GraphicsHeaders.h"

bool isParallel(const glm::vec3& v1, const glm::vec3& v2);

void ParticleManager::Init()
{
    
}


void ParticleManager::Update(float dt)
{
    Scene& currentScene = SceneManager::Instance().GetCurrentScene();
    //int counter = 0;
    for (ParticleComponent& particleComponent : currentScene.GetArray<ParticleComponent>()) {
        
        if (!currentScene.IsActive(particleComponent))
            continue;
        Entity& entity = currentScene.Get<Entity>(particleComponent);
        if (!currentScene.IsActive(entity))
            continue;
        Transform& entityTransform = currentScene.Get<Transform>(entity);
        /*float elapsedTimeSinceLastEmission = dt;
        float timeBetweenEmissions = 1.0f / particleComponent.particleEmissionRate_;*/
        

        if (particleComponent.particles_.size() != particleComponent.numParticles_) { // initialize
            particleComponent.particles_.resize(particleComponent.numParticles_);

            //random.resize(random.size() + particleComponent.numParticles_);
            
            // Initialize the position, velocity, and acceleration of each particle
            for (int i = 0; i < particleComponent.numParticles_; i++) {
                particleComponent.particles_[i].position = entityTransform.GetTranslation();
                particleComponent.particles_[i].direction = random.NextVector3(-20.0f, 20.0f);
                particleComponent.particles_[i].direction = glm::normalize(particleComponent.particles_[i].direction);
                particleComponent.particles_[i].acceleration = 1.0f;
                //particleComponent.particles_[i].lifetime = particleComponent.particleLifetime_;
                particleComponent.particles_[i].lifetime = random.NextFloat1(0.0f, particleComponent.particleLifetime_);
                particleComponent.particles_[i].scale += dt * particleComponent.particleScaleRate_;
                particleComponent.particles_[i].speed = particleComponent.speed_;
                particleComponent.particles_[i].trails.pos.emplace_back(particleComponent.particles_[i].position);
                //particleComponent.particles_[i].trails.count = 1;
            }
        }
        //particleComponent.particleLooping = true;

        
        // Handle particle collisions
        // Emit new particles
  
        //if (particleComponent.particleEmissionRate_ > 0.0f) { // idk how emmision rate works, @desmond your turn to do this
        for (int i = 0; i < particleComponent.numParticles_; i++) {
            if (particleComponent.particles_[i].lifetime <= 0.0f) {
                // randomize vec3 direction (normalized)
                //      can use glm::normalize() after calculating a random direction
                particleComponent.particles_[i].trails.pos.clear();
                particleComponent.particles_[i].trails.count = 0;
                if (particleComponent.particleLooping == true)
                {
                    particleComponent.particles_[i].position = entityTransform.GetTranslation(); // to entity's position
                    //particleComponent.particles_[i].direction = glm::vec3(0.f, 1.f, 0.f); // @desmond randomize this
                    particleComponent.particles_[i].direction = random.NextVector3(-20.0f, 20.0f);
                    particleComponent.particles_[i].direction = glm::normalize(particleComponent.particles_[i].direction);
                    particleComponent.particles_[i].acceleration = 1.0f;
                    particleComponent.particles_[i].lifetime = random.NextFloat1(0.0f, particleComponent.particleLifetime_);
                    particleComponent.particles_[i].scale = particleComponent.particleMinScale_;
                    particleComponent.particles_[i].speed = particleComponent.speed_;
                    /*std::cout
                        << particleComponent.particles_[i].direction.x << ", "
                        << particleComponent.particles_[i].direction.y << ", "
                        << particleComponent.particles_[i].direction.z << "\n";*/
                }
                else if (particleComponent.particleLooping == false)
                {
                    //particleComponent.particleEmissionRate_ = particleComponent.desiredLifetime / 1.0f; 
                    particleComponent.particleEmissionRate_ = 1.0f;
                    particleComponent.particles_[i].position = entityTransform.GetTranslation(); // to entity's position
                    particleComponent.particles_[i].direction = random.NextVector3(-20.0f, 20.0f);
                    particleComponent.particles_[i].direction = glm::normalize(particleComponent.particles_[i].direction);
                    particleComponent.particles_[i].acceleration = 1.0f;
                    particleComponent.particles_[i].lifetime -= dt;
                    particleComponent.particles_[i].scale = particleComponent.particleMinScale_;
                    particleComponent.particles_[i].speed = particleComponent.speed_;
                    /*if (particleComponent.particles_[i].lifetime <= 0)
                    {
                        particleComponent.particles_[i].lifetime = 0.0f; 
                    }*/
                    particleComponent.particles_[i].lifetime = 0.0f;
                }
                //else if (particleLooping == false)
                //{
                //    particleComponent.particles_[i].position = entityTransform.GetTranslation(); // to entity's position
                //    //particleComponent.particles_[i].direction = glm::vec3(0.f, 1.f, 0.f); // @desmond randomize this
                //    particleComponent.particles_[i].direction = random[i].NextVector3(-20.0f, 20.0f);
                //    particleComponent.particles_[i].direction = glm::normalize(particleComponent.particles_[i].direction);
                //    particleComponent.particles_[i].acceleration = 1.0f;
                //    particleComponent.particles_[i].lifetime = random[i].NextFloat1(0.0f, particleComponent.particleLifetime_);
                //    particleComponent.particles_[i].scale = particleComponent.particleMinScale_;
                //    particleComponent.particles_[i].speed = particleComponent.speed_;
                //}

            }
        }
        //}

        for (int i = 0; i < particleComponent.numParticles_; i++) {

            // speed += acceleration * dt
            // velocity = speed * direction

            particleComponent.particles_[i].speed += particleComponent.particles_[i].acceleration * dt;
            particleComponent.particles_[i].velocity = particleComponent.particles_[i].direction * particleComponent.particles_[i].speed;
            particleComponent.particles_[i].position += particleComponent.particles_[i].velocity * dt;
            particleComponent.particles_[i].lifetime -= dt;

            particleComponent.particles_[i].scale += dt * particleComponent.particleScaleRate_;
            particleComponent.particles_[i].scale = glm::clamp(particleComponent.particles_[i].scale, particleComponent.particleMinScale_, particleComponent.particleMaxScale_);

            if (1 >= particleComponent.particles_[i].trails.count) {
                ++particleComponent.particles_[i].trails.count;
                particleComponent.particles_[i].trails.pos.emplace_back(particleComponent.particles_[i].position);
            }
            else {
                // check if in the same ray
                
                glm::vec3 currPoint = particleComponent.particles_[i].position;
                glm::vec3 endPoint = particleComponent.particles_[i].trails.pos[particleComponent.particles_[i].trails.count - 1 ];
                glm::vec3 startPoint = particleComponent.particles_[i].trails.pos[particleComponent.particles_[i].trails.count - 2 ];

                // if same ray
                if (isParallel(endPoint - startPoint, currPoint - startPoint)) {
                    //endPoint = currPoint;
                    particleComponent.particles_[i].trails.pos[particleComponent.particles_[i].trails.count - 1] = particleComponent.particles_[i].position;
                }
                else {
                    particleComponent.particles_[i].trails.pos.emplace_back(currPoint);
                    particleComponent.particles_[i].trails.count++;
                }
            }
        }
        //counter += particleComponent.numParticles_;
    }
}

void ParticleManager::Exit()
{
    // Delete the particle system.
    //delete particleSystem;
}

bool isParallel(const glm::vec3& v1, const glm::vec3& v2)
{
    glm::vec3 test = glm::cross(v1, v2);
    if ((test.x < 0.01f) && (test.y < 0.01f) && (test.z < 0.01f) &&
        (test.x > -0.01f) && (test.y > -0.01f) && (test.z > -0.01f))
    {
        return true;
    }
    return false;
}

    