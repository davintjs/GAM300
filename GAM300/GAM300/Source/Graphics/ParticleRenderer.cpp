#include <Precompiled.h>
#include "ParticleRenderer.h"
#include "Graphics/MeshManager.h"
//#include "ParticleSystem/ParticleSystem.h"
#include "Scene/SceneManager.h"
#include "GraphicsHeaders.h"



void ParticleRenderer::Init() {
    SetupInstancedQuad();
}

void ParticleRenderer::Update(float dt) {
    particleSRT.clear();
    Scene& currentScene = SceneManager::Instance().GetCurrentScene();
    for (ParticleComponent& particleComponent : currentScene.GetArray<ParticleComponent>()) {
        Entity& entity = currentScene.Get<Entity>(particleComponent);
        Transform& entityTransform = currentScene.Get<Transform>(entity);
        for (int i = 0; i < particleComponent.numParticles_; ++i) {
            //particleTransform.GetTranslation() += particleComponent.particles_[i].position;
            glm::mat4 scale = glm::mat4(1.f) * particleComponent.particles_[i].scale;
            scale[3] = glm::vec4(0, 0, 0, 1);
            glm::mat4 rotate = glm::mat4(1.f);// @desmond lemme know when rotation is up

            /*if (_2dParticles) {
                rotate = desmond's rotation around y = 0; // in shader, set cam rotation around Y to 0;
            }*/

            glm::mat4 translate = glm::mat4(
                glm::vec4(1, 0, 0, 0),
                glm::vec4(0, 1, 0, 0),
                glm::vec4(0, 0, 1, 0),
                glm::vec4(particleComponent.particles_[i].position, 1));/**/

            //particleSRT.emplace_back(scale * rotate * translate);
            particleSRT.emplace_back(translate * rotate * scale);
        }
    }
    
}

void ParticleRenderer::Draw(BaseCamera& _camera) {
    Scene& currentScene = SceneManager::Instance().GetCurrentScene();
    GLuint vao = MESHMANAGER.DereferencingMesh(ASSET_CUBE)->vaoID; // for now particles are all cubes
    InstanceProperties& prop = MESHMANAGER.instanceProperties->find(vao)->second;
    for (ParticleComponent& particleComponent : currentScene.GetArray<ParticleComponent>()) {
        
        hasTexture = particleComponent.ParticleTexture.longInt[0] ? true : false;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureManager.GetTexture(particleComponent.ParticleTexture));

        if (particleComponent.is2D) {
            glBindBuffer(GL_ARRAY_BUFFER, quadSRTBuffer);
        }
        else {
            glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
        }
        glBufferSubData(GL_ARRAY_BUFFER, 0, (particleComponent.numParticles_) * sizeof(glm::mat4), particleSRT.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        GLSLShader shader = SHADER.GetShader(SHADERTYPE::PARTICLES);
        shader.Use();

        GLint perspective =
            glGetUniformLocation(shader.GetHandle(), "persp_projection");
        GLint view =
            glGetUniformLocation(shader.GetHandle(), "View");
        GLint campos =
            glGetUniformLocation(shader.GetHandle(), "camPos");
        GLint _2d =
            glGetUniformLocation(shader.GetHandle(), "is2D");
        GLint texture =
            glGetUniformLocation(shader.GetHandle(), "hasTexture");
        
        glUniformMatrix4fv(perspective, 1, GL_FALSE,
            glm::value_ptr(_camera.GetProjMatrix()));
        glUniformMatrix4fv(view, 1, GL_FALSE,
            glm::value_ptr(_camera.GetViewMatrix()));
        glUniform3fv(campos, 1,
            glm::value_ptr(_camera.GetCameraPosition()));
        glUniform1i(_2d, particleComponent.is2D);
        glUniform1i(texture, hasTexture);

        if (particleComponent.is2D) {
            glBindVertexArray(quadVAO);
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, particleComponent.numParticles_);
        }
        else {
            glBindVertexArray(vao);
            glDrawElementsInstanced(GL_TRIANGLES, prop.drawCount, GL_UNSIGNED_INT, 0, particleComponent.numParticles_);
        }
        glBindVertexArray(0);

        shader.UnUse();
    }
}

void ParticleRenderer::SetupInstancedQuad() {
    float quadVertices[] = {
        // positions            // texture Coords
        -1.f, -1.f,	0.f,    	0.f, 1.f,
         1.f, -1.f,	0.f,    	1.f, 1.f,
         1.f,  1.f,	0.f,    	1.f, 0.f,
        -1.f,  1.f,	0.f,    	0.f, 0.f
    };
    unsigned int indices[] = {
        0,1,2,
        2,3,0 
    };
    GLuint quadVBO, ebo;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &ebo);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0); // unbind vao
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind ebo
    //GL_TRIANGLE_STRIP


    // instance rendering setup
    glGenBuffers(1, &quadSRTBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quadSRTBuffer);
    glBufferData(GL_ARRAY_BUFFER, EntityRenderLimit * sizeof(glm::mat4), particleSRT.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadSRTBuffer);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
    glBindVertexArray(0);

}

void ParticleRenderer::Exit() {

}