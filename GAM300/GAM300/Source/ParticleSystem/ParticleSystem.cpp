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
    for (ParticleComponent& particleComponent : currentScene.GetArray<ParticleComponent>()) {
        
        if (!currentScene.IsActive(particleComponent))      //use the same for Button 
            continue;
        Entity& entity = currentScene.Get<Entity>(particleComponent);   //use the same for button 
        if (!currentScene.IsActive(entity))
            continue;
        Transform& entityTransform = currentScene.Get<Transform>(entity);
        Transform& entityTransformParent = currentScene.Get<Transform>(entityTransform.parent);

        if (particleComponent.particles_.size() != particleComponent.numParticles_) { // initialize or resize
            particleComponent.particles_.resize(particleComponent.numParticles_);

            // Initialize the position, velocity, and acceleration of each particle
            for (int i = 0; i < particleComponent.numParticles_; i++) {

                particleComponent.particles_[i].position = particleComponent.isLocalSpace ? vec3{0.f,0.f,0.f} : entityTransform.GetGlobalTranslation();

                //particleComponent.particles_[i].position = entityTransform.GetGlobalTranslation();
                particleComponent.particles_[i].direction = random.NextVector3(-20.0f, 20.0f);
                particleComponent.particles_[i].direction = glm::normalize(particleComponent.particles_[i].direction);
                particleComponent.particles_[i].acceleration = 1.0f;
                particleComponent.particles_[i].lifetime = random.NextFloat1(0.0f, particleComponent.particleLifetime_);
                particleComponent.particles_[i].scale += dt * particleComponent.particleScaleRate_;
                particleComponent.particles_[i].speed = particleComponent.speed_;
                particleComponent.particles_[i].trails.pos.emplace_back(particleComponent.particles_[i].position);
                //particleComponent.particles_[i].trails.count = 1;
            }
        }

        if (particleComponent.particleEmissionRate_ > 0.0f) {
            for (int i = 0; i < particleComponent.numParticles_; i++) {
                if (particleComponent.particles_[i].lifetime <= 0.0f) { // when particle dies, reset
                    particleComponent.particles_[i].trails.pos.clear();
                    particleComponent.particles_[i].trails.count = 0;
                    particleComponent.particles_[i].position = particleComponent.isLocalSpace ? vec3{ 0.f,0.f,0.f } : entityTransform.GetGlobalTranslation();

                    //particleComponent.particles_[i].position = entityTransform.GetGlobalTranslation(); // to entity's position
                    particleComponent.particles_[i].direction = random.NextVector3(-20.0f, 20.0f);
                    particleComponent.particles_[i].direction = glm::normalize(particleComponent.particles_[i].direction);
                    particleComponent.particles_[i].acceleration = 1.0f;
                    particleComponent.particles_[i].scale = particleComponent.particleMinScale_;
                    particleComponent.particles_[i].speed = particleComponent.speed_;

                    particleComponent.particles_[i].lifetime = 0.0f;

                    if (particleComponent.particleLooping)
                    {
                        particleComponent.particles_[i].lifetime = random.NextFloat1(0.0f, particleComponent.particleLifetime_);
                    }
                }
            }
        }

        for (int i = 0; i < particleComponent.numParticles_; i++) { // update particles

            // speed += acceleration * dt
            // velocity = speed * direction

            if (particleComponent.noiseMovement > 1.f) {
                if (particleComponent.particles_[i].noiselifetime <= 0.f) {
                    vec3 noiseDirection = random.NextVector3(-20.0f, 20.0f);
                    noiseDirection = glm::normalize(noiseDirection);
                    float noise = particleComponent.noiseMovement / 100.f;
                    particleComponent.particles_[i].direction = glm::normalize(noise * noiseDirection + (1.f - noise) * particleComponent.particles_[i].direction);
                    if (particleComponent.noisefrequency >= 2.f ) {
                        particleComponent.particles_[i].noiselifetime = random.NextFloat1(10.f * particleComponent.particleLifetime_ / particleComponent.noisefrequency, particleComponent.particleLifetime_ -  10.f * particleComponent.particleLifetime_ / (particleComponent.noisefrequency - 1.f)) / 10.f;
                    }
                    else {
                        particleComponent.particles_[i].noiselifetime = particleComponent.particleLifetime_;
                    }
                }
                particleComponent.particles_[i].noiselifetime -= dt;
            }

            particleComponent.particles_[i].speed += particleComponent.particles_[i].acceleration * dt;
            particleComponent.particles_[i].velocity = particleComponent.particles_[i].direction * particleComponent.particles_[i].speed;
            particleComponent.particles_[i].position += particleComponent.particles_[i].velocity * dt;
            particleComponent.particles_[i].lifetime -= dt;
            particleComponent.particles_[i].scale += dt * particleComponent.particleScaleRate_;
            particleComponent.particles_[i].scale = glm::clamp(particleComponent.particles_[i].scale, particleComponent.particleMinScale_, particleComponent.particleMaxScale_);
            vec3 particlePos = particleComponent.particles_[i].position;
            if (particleComponent.isLocalSpace) {
                particlePos += entityTransform.GetGlobalTranslation();
            }

            if (!particleComponent.trailEnabled) {
                continue;
            }
            if (particleComponent.trailSize < 1) {
                particleComponent.trailSize = 1;
            }
            if (1 >= particleComponent.particles_[i].trails.count) {
                ++particleComponent.particles_[i].trails.count;
                particleComponent.particles_[i].trails.pos.emplace_back(particlePos);
            }
            else {
                // check if in the same ray
                
                glm::vec3 currPoint = particlePos;
                glm::vec3 endPoint = particleComponent.particles_[i].trails.pos[particleComponent.particles_[i].trails.count - 1 ];
                glm::vec3 startPoint = particleComponent.particles_[i].trails.pos[particleComponent.particles_[i].trails.count - 2 ];

                if (isParallel(endPoint - startPoint, currPoint - startPoint)) {
                    //endPoint = currPoint;
                    particleComponent.particles_[i].trails.pos[particleComponent.particles_[i].trails.count - 1] = particlePos;
                }
                else {
                    particleComponent.particles_[i].trails.pos.emplace_back(currPoint);
                    particleComponent.particles_[i].trails.count++;
                }
            }
            if (particleComponent.particles_[i].trails.count > (unsigned)particleComponent.trailSize) {
                particleComponent.particles_[i].trails.count--;
                particleComponent.particles_[i].trails.pos.pop_front();
            }
        }
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
    //test.length();
    /*if ((test.x < 0.01f) && (test.y < 0.01f) && (test.z < 0.01f) &&
        (test.x > -0.01f) && (test.y > -0.01f) && (test.z > -0.01f))*/
    //std::cout << glm::length(test) << std::endl;
    if (glm::length(test) < 0.00001f)
    {
        return true;
    }
    return false;
}

    