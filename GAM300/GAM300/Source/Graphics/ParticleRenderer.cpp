#include <Precompiled.h>
#include "ParticleRenderer.h"
#include "Graphics/MeshManager.h"
//#include "ParticleSystem/ParticleSystem.h"
#include "Scene/SceneManager.h"
#include "GraphicsHeaders.h"

void ParticleRenderer::Init() {

}

void ParticleRenderer::Update(float dt) {
    particleSRT.clear();
    Scene& currentScene = SceneManager::Instance().GetCurrentScene();
    for (ParticleComponent& particleComponent : currentScene.GetArray<ParticleComponent>()) {
        Entity& entity = currentScene.Get<Entity>(particleComponent);
        Transform& entityTransform = currentScene.Get<Transform>(entity);
        for (int i = 0; i < particleComponent.numParticles_; ++i) {
            //particleTransform.GetTranslation() += particleComponent.particles_[i].position;
            glm::mat4 scale = glm::mat4(1.f); // @desmond lemme know when scale is up
            glm::mat4 rotate = glm::mat4(1.f);// @desmond lemme know when rotation is up
            glm::mat4 translate = glm::mat4(
                glm::vec4(1, 0, 0, 0),
                glm::vec4(0, 1, 0, 0),
                glm::vec4(0, 0, 1, 0),
                glm::vec4(particleComponent.particles_[i].position, 1));/**/

            particleSRT.emplace_back(scale * rotate * translate);
        }
    }
    
}

void ParticleRenderer::Draw(BaseCamera& _camera) {
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

void ParticleRenderer::Exit() {

}