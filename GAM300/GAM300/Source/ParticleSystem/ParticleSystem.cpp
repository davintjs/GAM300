#include "Precompiled.h"
#include "Scene/SceneManager.h"
#include "ParticleSystem.h"
#include "Graphics/MeshManager.h"
#include "Scene/Components.h" 




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
    GLuint vao = MeshManager.DereferencingMesh(DEFAULT_ASSETS["Cube.geom"])->vaoID; // for now particles are all cubes
    InstanceProperties& prop = MeshManager.instanceProperties->find(vao)->second;
    /*for (ParticleComponent& partygirls : currentScene.GetArray<ParticleComponent>()) {
        partygirls.Update(dt);
        Entity& entity = currentScene.Get<Entity>(partygirls);
        Transform& entityTransform = currentScene.Get<Transform>(entity);
        Transform particleTransform = entityTransform;
        particleTransform.GetTranslation() += partygirls.particles_->position;
        particleSRT.emplace_back(particleTransform);
    }*/
}

void ParticleManager::Render()
{
    //Scene& currentScene = SceneManager::Instance().GetCurrentScene();
    //GLuint vao = MeshManager.DereferencingMesh(DEFAULT_ASSETS["Cube.geom"])->vaoID; // for now particles are all cubes
    //InstanceProperties& prop = MeshManager.instanceProperties->find(vao)->second;
    //for (ParticleComponent& partygirls : currentScene.GetArray<ParticleComponent>()) {
    //    glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
    //    glBufferSubData(GL_ARRAY_BUFFER, 0, (partygirls.numParticles_) * sizeof(glm::mat4), particleSRT.data());
    //    glBindBuffer(GL_ARRAY_BUFFER, 0);


    //}
}

void ParticleManager::Exit()
{
    // Delete the particle system.
    //delete particleSystem;
}

    