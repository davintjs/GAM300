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
        vec3 origin = particleComponent.isLocalSpace ? vec3{ 0.f,0.f,0.f } : entityTransform.GetGlobalTranslation();
        bool enableDirection = glm::length(vec3(particleComponent.direction)) > 0.1f ? true : false;

        if (particleComponent.particles_.size() != particleComponent.numParticles_) { // initialize or resize
            particleComponent.particles_.resize(particleComponent.numParticles_);


            // Initialize the position, velocity, and acceleration of each particle
            for (int i = 0; i < particleComponent.numParticles_; i++) {

                particleComponent.particles_[i].position = origin;
                // This is the random spawning within a area
                if (particleComponent.spawnFromShape)
                {
                    particleComponent.particles_[i].position.x = random.NextFloat1(particleComponent.particles_[i].position.x - particleComponent.fromShapeAxis.x, particleComponent.particles_[i].position.x + particleComponent.fromShapeAxis.x);
                    particleComponent.particles_[i].position.y = random.NextFloat1(particleComponent.particles_[i].position.y - particleComponent.fromShapeAxis.y, particleComponent.particles_[i].position.y + particleComponent.fromShapeAxis.y);
                    particleComponent.particles_[i].position.z = random.NextFloat1(particleComponent.particles_[i].position.z - particleComponent.fromShapeAxis.z, particleComponent.particles_[i].position.z + particleComponent.fromShapeAxis.z);
                }
                //particleComponent.particles_[i].position = entityTransform.GetGlobalTranslation();
                particleComponent.particles_[i].direction = random.NextVector3(-20.0f, 20.0f);
                particleComponent.particles_[i].direction = enableDirection ? vec3(particleComponent.direction) : random.NextVector3(-20.0f, 20.0f);
                particleComponent.particles_[i].direction = glm::normalize(particleComponent.particles_[i].direction);
                particleComponent.particles_[i].acceleration = particleComponent.acceleration_;
                particleComponent.particles_[i].lifetime = random.NextFloat1(0.0f, particleComponent.particleLifetime_);
                particleComponent.particles_[i].scale += dt * particleComponent.particleScaleRate_;
                particleComponent.particles_[i].speed = particleComponent.speed_;
                particleComponent.particles_[i].trails.pos.emplace_back(particleComponent.particles_[i].position);
                //particleComponent.particles_[i].trails.count = 1;
            }
        }

        //if (particleComponent.particleEmissionRate_ > 0.0f) {
            
            for (int i = 0; i < particleComponent.numParticles_; i++) {
                
                
                if (particleComponent.particles_[i].lifetime <= 0.0f) { // when particle dies, reset
                    particleComponent.particles_[i].trails.pos.clear();
                    particleComponent.particles_[i].trails.count = 0;
                    particleComponent.particles_[i].position = origin;
                    if (particleComponent.spawnFromShape)
                    {
                        particleComponent.particles_[i].position.x = random.NextFloat1(particleComponent.particles_[i].position.x - particleComponent.fromShapeAxis.x, particleComponent.particles_[i].position.x + particleComponent.fromShapeAxis.x);
                        particleComponent.particles_[i].position.y = random.NextFloat1(particleComponent.particles_[i].position.y - particleComponent.fromShapeAxis.y, particleComponent.particles_[i].position.y + particleComponent.fromShapeAxis.y);
                        particleComponent.particles_[i].position.z = random.NextFloat1(particleComponent.particles_[i].position.z - particleComponent.fromShapeAxis.z, particleComponent.particles_[i].position.z + particleComponent.fromShapeAxis.z);
                    }
                    //particleComponent.particles_[i].position = entityTransform.GetGlobalTranslation(); // to entity's position
                    particleComponent.particles_[i].direction = enableDirection ? vec3(particleComponent.direction) : random.NextVector3(-20.0f, 20.0f);
                    particleComponent.particles_[i].direction = glm::normalize(particleComponent.particles_[i].direction);
                    particleComponent.particles_[i].acceleration = particleComponent.acceleration_;
                    particleComponent.particles_[i].scale = particleComponent.particleMinScale_;
                    particleComponent.particles_[i].speed = particleComponent.speed_;

                    particleComponent.particles_[i].lifetime = 0.0f;

                    if (particleComponent.particleLooping)
                    {
                        particleComponent.particles_[i].lifetime = random.NextFloat1(0.0f, particleComponent.particleLifetime_);
                    }
                }
            }
        //}

        const float noiseMovementFactor = particleComponent.noiseMovement / 100.f;

        for (int i = 0; i < particleComponent.numParticles_; i++) {
            
            Particle& particle = particleComponent.particles_[i];
            Trail& trails = particle.trails;
            if (particleComponent.speed_ != 0) {
                if (particleComponent.noiseMovement > 1.f) {
                    if (particle.noiselifetime <= 0.f) {

                        if (!enableDirection || particle.position == origin) {
                            // Handle case where enableDirection is false or particle.position == origin
                            // Assuming some default behavior here
                            vec3 noiseDirection = glm::normalize(random.NextVector3(-20.0f, 20.0f));
                            particle.direction = glm::normalize(noiseMovementFactor * noiseDirection + (1.f - noiseMovementFactor) * particle.direction);
                        }
                        else {
                            vec3 particlePosMinusOrigin = particle.position - origin;
                            vec3 normalizedParticlePosMinusOrigin = glm::normalize(particlePosMinusOrigin);
                            vec3 normalizedParticleComponentDirection = glm::normalize(vec3(particleComponent.direction));

                            float dotProduct = glm::dot(normalizedParticlePosMinusOrigin, normalizedParticleComponentDirection);
                            float threshold = cosf(particleComponent.angle * 0.0174533f); // Precomputed constant

                            if (dotProduct < threshold) {
                                particle.direction = vec3(particleComponent.direction);
                            }
                            else {
                                vec3 noiseDirection = glm::normalize(random.NextVector3(-20.0f, 20.0f));
                                particle.direction = glm::normalize(noiseMovementFactor * noiseDirection + (1.f - noiseMovementFactor) * particle.direction);
                            }
                        }


                        if (particleComponent.noisefrequency >= 1.f) {
                            particle.noiselifetime = random.NextFloat1(10.f * particleComponent.particleLifetime_ / particleComponent.noisefrequency, particleComponent.particleLifetime_ - 12.5f * particleComponent.particleLifetime_ / (particleComponent.noisefrequency)) / 10.f;
                        }
                        else {
                            particle.noiselifetime = particleComponent.particleLifetime_;
                        }

                    }
                    particle.noiselifetime -= dt;
                }
            }

            particle.speed += particle.acceleration * dt;
            particle.velocity = particle.direction * particle.speed;
            particle.position += particle.velocity * dt;
            particle.lifetime -= dt;
            particle.scale += dt * particleComponent.particleScaleRate_;
            particle.scale = glm::clamp(particle.scale, particleComponent.particleMinScale_, particleComponent.particleMaxScale_);

            vec3 particlePos = particle.position;
            if (particleComponent.isLocalSpace) {
                particlePos += entityTransform.GetGlobalTranslation();
            }

           

            if (!particleComponent.trailEnabled) {
                continue;
            }

            if (!particleComponent.is2D) {
                const size_t trailCount = trails.count;

                if (1 >= trailCount) {
                    ++trails.count;
                    trails.pos.emplace_back(particlePos);
                    continue;
                }
                const glm::vec3& endPoint = trails.pos[trailCount - 1];
                const glm::vec3& startPoint = trails.pos[trailCount - 2];

                if (isParallel(endPoint - startPoint, particlePos - startPoint)) {
                    trails.pos[trailCount - 1] = particlePos;
                }
                else {
                    trails.pos.emplace_back(particlePos);
                    trails.count++;
                }
                if (particleComponent.trailSize > 0 && trailCount > static_cast<size_t>(particleComponent.trailSize)) {
                    trails.count--;
                    trails.pos.pop_front();
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

bool isParallel(const glm::vec3& v1, const glm::vec3& v2)
{
    glm::vec3 test = glm::cross(v1, v2);
    //test.length();
    /*if ((test.x < 0.01f) && (test.y < 0.01f) && (test.z < 0.01f) &&
        (test.x > -0.01f) && (test.y > -0.01f) && (test.z > -0.01f))*/
    //std::cout << glm::length(test) << std::endl;
    if (glm::length(test) < 0.01f)
    {
        return true;
    }
    return false;
}

    