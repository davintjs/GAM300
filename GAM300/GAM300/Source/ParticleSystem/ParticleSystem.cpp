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

    particleSRT.clear();
    Scene& currentScene = SceneManager::Instance().GetCurrentScene();
    GLuint vao = MESHMANAGER.DereferencingMesh(ASSET_CUBE)->vaoID; // for now particles are all cubes
    InstanceProperties& prop = MESHMANAGER.instanceProperties->find(vao)->second;

    for (ParticleComponent& particleComponent : currentScene.GetArray<ParticleComponent>()) {
        if (particleComponent.numParticles_ >= 0) {
            particleComponent.Initialize(particleComponent.numParticles_, particleComponent.particleLifetime_, particleComponent.particleEmissionRate_);
        }
        particleComponent.Update(dt);
        Entity& entity = currentScene.Get<Entity>(particleComponent);
        Transform& entityTransform = currentScene.Get<Transform>(entity);
        Transform particleTransform = entityTransform;
        for (int i = 0; i < particleComponent.numParticles_; ++i) {
            particleTransform.GetTranslation() += particleComponent.particles_[i].position;
            particleSRT.emplace_back(particleTransform.GetWorldMatrix());/**/
        }
    }
}

void ParticleManager::Render(BaseCamera& _camera)
{
    Scene& currentScene = SceneManager::Instance().GetCurrentScene();
    GLuint vao = MESHMANAGER.DereferencingMesh(ASSET_CUBE)->vaoID; // for now particles are all cubes
    InstanceProperties& prop = MESHMANAGER.instanceProperties->find(vao)->second;
    for (ParticleComponent& particleComponent : currentScene.GetArray<ParticleComponent>()) {
        glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, (particleComponent.numParticles_) * sizeof(glm::mat4), particleSRT.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        GLSLShader shader = SHADER.GetShader(SHADERTYPE::PARTICLES);
        shader.Use();

        GLint uniform1 =
            glGetUniformLocation(shader.GetHandle(), "persp_projection");
        GLint uniform2 =
            glGetUniformLocation(shader.GetHandle(), "View");
        GLint uniform3 =
            glGetUniformLocation(shader.GetHandle(), "camPos");

        glUniformMatrix4fv(uniform1, 1, GL_FALSE,
            glm::value_ptr(_camera.GetProjMatrix()));
        glUniformMatrix4fv(uniform2, 1, GL_FALSE,
            glm::value_ptr(_camera.GetViewMatrix()));
        glUniform3fv(uniform3, 1,
            glm::value_ptr(_camera.GetCameraPosition()));

        glBindVertexArray(prop.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, prop.drawCount, GL_UNSIGNED_INT, 0, particleComponent.numParticles_);
        glBindVertexArray(0);

        shader.UnUse();
    }
}

void ParticleManager::Exit()
{
    // Delete the particle system.
    //delete particleSystem;
}

    