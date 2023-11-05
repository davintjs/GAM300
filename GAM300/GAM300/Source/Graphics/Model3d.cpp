/*!***************************************************************************************
\file			Model3d.cpp
\project
\author         Lian Khai Kiat, Euan Lim, Theophelia Tan

\par			Course: GAM300
\date           28/09/2023

\brief
    This file contains the definitions of
    1. temporary stucts and classes used for testing
    2. Model class to create a model by abstacting openGL functions

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include <glm/gtc/type_ptr.hpp>

#include "Model3d.h"
#include "Editor/EditorCamera.h"

#include "GraphicsSystem.h"
#include "TextureManager.h"
#include "MeshManager.h"

//extern EditorCamera E_Camera;

bool mesh_1, mesh_2, mesh_3, mesh_4 = false;

const std::string shaderPath = "GAM300/Shaders";

// should move somewhere else...
void Model::draw() {

    /*if (InputHandler::isKeyButtonPressed(GLFW_KEY_1))
    {
        mesh_1 = !mesh_1;
    }if (InputHandler::isKeyButtonPressed(GLFW_KEY_2))
    {
        mesh_2 = !mesh_2;
    }if (InputHandler::isKeyButtonPressed(GLFW_KEY_3))
    {
        mesh_3 = !mesh_3;
    }if (InputHandler::isKeyButtonPressed(GLFW_KEY_4))
    {
        mesh_4 = !mesh_4;
    }*/


    glEnable(GL_DEPTH_TEST); // might be sus to place this here

    glm::mat4 scaling_mat(
        glm::vec4(0.1f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 0.1f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 0.1f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)

    );

    glm::mat4 rotation_mat(
        glm::vec4(cos(90.f), 0.f, -sin(90.f), 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(sin(90.f), 0.f, cos(90.f), 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    );
    //glm::mat3 translation_mat = glm::mat3(1.f);

    glm::mat4 translation_mat(
        glm::vec4(1.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    );
    glm::mat4 SRT = translation_mat * rotation_mat * scaling_mat;
    

    shader.Use();
    // UNIFORM VARIABLES ----------------------------------------
    // Persp Projection
    GLint uniform_var_loc1 =
        glGetUniformLocation(this->shader.GetHandle(),
            "persp_projection");
    glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE,
        glm::value_ptr(EditorCam.GetProjMatrix()));
    GLint uniform_var_loc2 =
        glGetUniformLocation(this->shader.GetHandle(),
            "View");
    glUniformMatrix4fv(uniform_var_loc2, 1, GL_FALSE,
        glm::value_ptr(EditorCam.GetViewMatrix()));

    // Scuffed SRT
    GLint uniform_var_loc3 =
        glGetUniformLocation(this->shader.GetHandle(),
            "SRT");
    glUniformMatrix4fv(uniform_var_loc3, 1, GL_FALSE,
        glm::value_ptr(SRT));

    // test
    //glActiveTexture(GL_TEXTURE0);
    texturebuffer = TextureManager.GetTexture("Assets/TD_Checker_Base_Color.dds");
    glBindTexture(GL_TEXTURE_2D, texturebuffer);
    glUniform1i(glGetUniformLocation(shader.GetHandle(), "myTextureSampler"), 0);
    

    //glBindVertexArray(vaoid);
    //if(mesh_1)
    //{
    //    glBindVertexArray(FBX_vaoid[1]);
    //    glDrawElements(prim, FBX_drawcount[1], GL_UNSIGNED_INT, nullptr);
    //    glBindVertexArray(0);
   
    //    //glBindVertexArray(0);
    //}  
    //if (mesh_2)
    //{
    //    glBindVertexArray(FBX_vaoid[2]);

    //    glDrawElements(prim, FBX_drawcount[2], GL_UNSIGNED_INT, nullptr);
    //    glBindVertexArray(0);

    //    //glBindVertexArray(0);
    //}
    //if (mesh_3)
    //{
    //    glBindVertexArray(FBX_vaoid[3]);

    //    glDrawElements(prim, FBX_drawcount[3], GL_UNSIGNED_INT, nullptr);
    //    glBindVertexArray(0);

    //    //glBindVertexArray(0);
    //}
    //if (mesh_4)
    //{

    mesh_4 = true;
    glBindVertexArray(FBX_vaoid[0]);

    glDrawElements(prim, FBX_drawcount[0], GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(vaoid);
    //glDrawElements(prim, drawcount, GL_UNSIGNED_INT, nullptr);

    // unbind and free stuff
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    shader.UnUse();

    //debugAABB_draw(SRT);
}

void Model::instanceDraw(int entitycount) {
    glEnable(GL_DEPTH_TEST); // might be sus to place this here

    shader.Use();
    // UNIFORM VARIABLES ----------------------------------------
    // Persp Projection
    GLint uniform1 =
        glGetUniformLocation(this->shader.GetHandle(), "persp_projection");
    GLint uniform2 =
        glGetUniformLocation(this->shader.GetHandle(), "View");
    // Scuffed SRT
    // srt not uniform
    /*GLint uniform3 =
        glGetUniformLocation(this->shader.GetHandle(), "SRT");*/

    glUniformMatrix4fv(uniform1, 1, GL_FALSE,
        glm::value_ptr(EditorCam.GetProjMatrix()));
    glUniformMatrix4fv(uniform2, 1, GL_FALSE,
        glm::value_ptr(EditorCam.GetViewMatrix()));

    glBindVertexArray(vaoid);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, entitycount);
    glBindVertexArray(0);

        //glBindVertexArray(0);
    //}
    shader.UnUse();

}

void Model::instance_cubeinit()
{

    //float vertices[] = {
    //    // positions            // Normals              // Tangents             // Texture Coords   // Colors
    //    // FRONT FACE //
    //    -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         1.0f, 0.0f, 0.0f, 1.0f, // Vertex 0
    //     1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 0.0f,         0.0f, 1.0f, 0.0f, 1.0f, // Vertex 1
    //     1.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         0.0f, 0.0f, 1.0f, 1.0f, // Vertex 2
    //                                                                                                                 
    //     1.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         0.0f, 0.0f, 1.0f, 1.0f, // Vertex 2
    //    -1.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,         1.0f, 1.0f, 0.0f, 1.0f, // Vertex 3
    //    -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         1.0f, 0.0f, 0.0f, 1.0f, // Vertex 0
    //    // FRONT FACE END //                                                                                         
    //                                                                                                                 
    //    // BACK FACE //                                                                                              
    //    -1.0f, -1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         0.0f, 0.0f, 1.0f, 1.0f, // Vertex 4
    //     1.0f, -1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 0.0f,         0.0f, 1.0f, 1.0f, 1.0f, // Vertex 5
    //     1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         1.0f, 0.0f, 1.0f, 1.0f, // Vertex 6
    //                                                                                                                 
    //     1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         1.0f, 0.0f, 1.0f, 1.0f, // Vertex 6
    //    -1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,         0.5f, 0.5f, 0.5f, 1.0f,  // Vertex 7
    //    -1.0f, -1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         0.0f, 0.0f, 1.0f, 1.0f, // Vertex 4
    //    // BACK FACE END //                                                                                          
    //                                                                                                                 
    //    // RIGHT FACE //                                                                                             
    //     1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 0.0f,         0.0f, 1.0f, 0.0f, 1.0f, // Vertex 1
    //     1.0f, -1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 0.0f,         0.0f, 1.0f, 1.0f, 1.0f, // Vertex 5
    //     1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         1.0f, 0.0f, 1.0f, 1.0f, // Vertex 6
    //                                                                                                                 
    //     1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         1.0f, 0.0f, 1.0f, 1.0f, // Vertex 6
    //     1.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         0.0f, 0.0f, 1.0f, 1.0f, // Vertex 2
    //     1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 0.0f,         0.0f, 1.0f, 0.0f, 1.0f, // Vertex 1
    //    // RIGHT FACE END //                                                                                         
    //                                                                                                                 
    //    // LEFT FACE //                                                                                              
    //    -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         1.0f, 0.0f, 0.0f, 1.0f, // Vertex 0
    //    -1.0f, -1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         0.0f, 0.0f, 1.0f, 1.0f, // Vertex 4
    //    -1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,         0.5f, 0.5f, 0.5f, 1.0f,  // Vertex 7
    //                                                                                                                 
    //    -1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,         0.5f, 0.5f, 0.5f, 1.0f,  // Vertex 7
    //    -1.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,         1.0f, 1.0f, 0.0f, 1.0f, // Vertex 3
    //    -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         1.0f, 0.0f, 0.0f, 1.0f, // Vertex 0
    //    // LEFT FACE END //                                                                               
    //                                                                                                      
    //    // TOP FACE //                                                                                    
    //    -1.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,         1.0f, 1.0f, 0.0f, 1.0f, // Vertex 3
    //     1.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         0.0f, 0.0f, 1.0f, 1.0f, // Vertex 2
    //     1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         1.0f, 0.0f, 1.0f, 1.0f, // Vertex 6
    //                                                                                                                 
    //     1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         1.0f, 0.0f, 1.0f, 1.0f, // Vertex 6
    //    -1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,         0.5f, 0.5f, 0.5f, 1.0f,  // Vertex 7
    //    -1.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,         1.0f, 1.0f, 0.0f, 1.0f, // Vertex 3
    //    // TOP FACE END //                                                                                           
    //                                                                                                                 
    //    // BOTTOM FACE //                                                                                            
    //    -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         1.0f, 0.0f, 0.0f, 1.0f, // Vertex 0
    //     1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 0.0f,         0.0f, 1.0f, 0.0f, 1.0f, // Vertex 1
    //     1.0f, -1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 0.0f,         0.0f, 1.0f, 1.0f, 1.0f, // Vertex 5
    //                                                                                                                 
    //     1.0f, -1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 0.0f,         0.0f, 1.0f, 1.0f, 1.0f, // Vertex 5
    //    -1.0f, -1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         0.0f, 0.0f, 1.0f, 1.0f, // Vertex 4
    //    -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         1.0f, 0.0f, 0.0f, 1.0f // Vertex 0
    //    // BOTTOM FACE END //

    //};

    float vertices[] = {
        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         1.0f, 0.0f, 0.0f, 1.0f, // Vertex 0
         1.0f, -1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 0.0f,         0.0f, 1.0f, 0.0f, 1.0f, // Vertex 1
         1.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         0.0f, 0.0f, 1.0f, 1.0f, // Vertex 2
        -1.0f,  1.0f, -1.0f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,         1.0f, 1.0f, 0.0f, 1.0f, // Vertex 3
        -1.0f, -1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,         0.0f, 0.0f, 1.0f, 1.0f, // Vertex 4
         1.0f, -1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 0.0f,         0.0f, 1.0f, 1.0f, 1.0f, // Vertex 5
         1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         1.0f, 0.0f, 1.0f, 1.0f, // Vertex 6
        -1.0f,  1.0f,  1.0f,    0.0f, 0.0f,  1.0f,      1.0f, 0.0f, 0.0f,       0.0f, 1.0f,         0.5f, 0.5f, 0.5f, 1.0f,  // Vertex 7

    };

    int indices[] = {
        0, 1, 2, 2, 3, 0,  // Front face
        4, 5, 6, 6, 7, 4,  // Back face
        1, 5, 6, 6, 2, 1,  // Right face
        0, 4, 7, 7, 3, 0,  // Left face
        3, 2, 6, 6, 7, 3,  // Top face
        0, 1, 5, 5, 4, 0   // Bottom face
    };

    vertices_min = glm::vec3(-0.5f, -0.5f, -0.5f);
    vertices_max = glm::vec3(0.5f, 0.5f, 0.5f);
    

    // first, configure the cube's VAO (and VBO)
    //unsigned int VBO, cubeVAO;
    GLuint ebo;

    glGenVertexArrays(1, &vaoid);
    glGenBuffers(1, &vboid);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vaoid);

    glBindBuffer(GL_ARRAY_BUFFER, vboid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Tangent attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Texture coord
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // color coord
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 15 * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);

    //bind ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0); // unbind vao
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind ebo

    debugAABB_init();


}

void Model::cubeinit()
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    // first, configure the cube's VAO (and VBO)
    //unsigned int VBO, cubeVAO;
    
    glGenVertexArrays(1, &vaoid);
    glGenBuffers(1, &vboid);

    glBindBuffer(GL_ARRAY_BUFFER, vboid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(vaoid);

    glGenBuffers(1, &texturebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    //// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    //unsigned int lightCubeVAO;
    //glGenVertexArrays(1, &lightCubeVAO);
    //glBindVertexArray(lightCubeVAO);

    //// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    //glBindBuffer(GL_ARRAY_BUFFER, vboid);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);
}

void Model::lightSource_draw()
{
    glEnable(GL_DEPTH_TEST); // might be sus to place this here

    glm::mat4 scaling_mat(
        glm::vec4(100.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 100.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 100.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    );

    glm::mat4 rotation_mat(
        glm::vec4(cos(90.f), 0.f, -sin(90.f), 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(sin(90.f), 0.f, cos(90.f), 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    );
    //glm::mat3 translation_mat = glm::mat3(1.f);

    glm::mat4 translation_mat(
        glm::vec4(1.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(position, 1.f)
    );
    glm::mat4 SRT = translation_mat * rotation_mat * scaling_mat;


    shader.Use();
    // UNIFORM VARIABLES ----------------------------------------
    // Persp Projection
    GLint uniform_var_loc1 =
        glGetUniformLocation(this->shader.GetHandle(),
            "persp_projection");
    glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE,
        glm::value_ptr(EditorCam.GetProjMatrix()));
    GLint uniform_var_loc2 =
        glGetUniformLocation(this->shader.GetHandle(),
            "View");
    glUniformMatrix4fv(uniform_var_loc2, 1, GL_FALSE,
        glm::value_ptr(EditorCam.GetViewMatrix()));

    // Scuffed SRT

    GLint uniform_var_loc3 =
        glGetUniformLocation(this->shader.GetHandle(),
            "SRT");
    glUniformMatrix4fv(uniform_var_loc3, 1, GL_FALSE,
        glm::value_ptr(SRT));

    //// test
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturebuffer);
    glUniform1i(glGetUniformLocation(shader.GetHandle(), "myTextureSampler"), 0);


    glBindVertexArray(vaoid);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    shader.UnUse();

}

void Model::affectedByLight_draw(glm::vec3 lightPos)
{

    glEnable(GL_DEPTH_TEST); // might be sus to place this here

    glm::mat4 scaling_mat(
        glm::vec4(100.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 100.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 100.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)

    );

    glm::mat4 rotation_mat(
        glm::vec4(cos(90.f), 0.f, -sin(90.f), 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(sin(90.f), 0.f, cos(90.f), 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    );
    //glm::mat3 translation_mat = glm::mat3(1.f);

    glm::mat4 translation_mat(
        glm::vec4(1.f, 0.f, 0.f, 0.f),
        glm::vec4(0.f, 1.f, 0.f, 0.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(position, 1.f)
    );
    glm::mat4 SRT = translation_mat * rotation_mat * scaling_mat;


    shader.Use();

    // UNIFORM VARIABLES ----------------------------------------
    // Persp Projection
    GLint uniform_var_loc1 =
        glGetUniformLocation(this->shader.GetHandle(),
            "persp_projection");
    glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE,
        glm::value_ptr(EditorCam.GetProjMatrix()));
    GLint uniform_var_loc2 =
        glGetUniformLocation(this->shader.GetHandle(),
            "View");
    glUniformMatrix4fv(uniform_var_loc2, 1, GL_FALSE,
        glm::value_ptr(EditorCam.GetViewMatrix()));

    // Scuffed SRT
    GLint uniform_var_loc3 =
        glGetUniformLocation(this->shader.GetHandle(),
            "SRT");
    glUniformMatrix4fv(uniform_var_loc3, 1, GL_FALSE,
        glm::value_ptr(SRT));

    // Setting Up Light Thing here

    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 objectColor(1.0f, 0.5f, 0.31f);

    GLint uniform_var_loc4 =
        glGetUniformLocation(this->shader.GetHandle(),
            "lightColor");
    glUniform3fv(uniform_var_loc4, 1, 
        glm::value_ptr(lightColor));

    GLint uniform_var_loc5 =
        glGetUniformLocation(this->shader.GetHandle(),
            "objectColor");
    glUniform3fv(uniform_var_loc5, 1,
        glm::value_ptr(objectColor));

    GLint uniform_var_loc6 =
        glGetUniformLocation(this->shader.GetHandle(),
            "lightPos");
    glUniform3fv(uniform_var_loc6, 1,
        glm::value_ptr(lightPos));

    GLint uniform_var_loc7 =
        glGetUniformLocation(this->shader.GetHandle(),
            "camPos");
    glUniform3fv(uniform_var_loc7, 1,
        glm::value_ptr(EditorCam.GetCameraPosition()));



    //// test
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturebuffer);
    glUniform1i(glGetUniformLocation(shader.GetHandle(), "myTextureSampler"), 0);


    glBindVertexArray(vaoid);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    shader.UnUse();

}

void Model::debugAABB_init() // vao & shader
{
    //// find min max points for each axis
    //glm::vec3 minpt = _geom->_vertices[0].pos, maxpt = _geom->_vertices[0].pos;

    //for (size_t i = 0; i < _geom->_vertices.size(); i++)
    //{
    //    minpt.x = std::min(minpt.x, _geom->_vertices[i].pos.x);
    //    minpt.y = std::min(minpt.y, _geom->_vertices[i].pos.y);
    //    minpt.z = std::min(minpt.z, _geom->_vertices[i].pos.z);

    //    maxpt.x = std::max(maxpt.x, _geom->_vertices[i].pos.x);
    //    maxpt.y = std::max(maxpt.y, _geom->_vertices[i].pos.y);
    //    maxpt.z = std::max(maxpt.z, _geom->_vertices[i].pos.z);
    //}
    glm::vec3 minpt = vertices_min;
    glm::vec3 maxpt = vertices_max;

    pntAABB[0] = minpt;
    pntAABB[1] = glm::vec4(minpt.x, minpt.y, maxpt.z, 1.f);
    pntAABB[2] = glm::vec4(minpt.x, maxpt.y, maxpt.z, 1.f);
    pntAABB[3] = glm::vec4(minpt.x, maxpt.y, minpt.z, 1.f);

    pntAABB[4] = maxpt;
    pntAABB[5] = glm::vec4(maxpt.x, maxpt.y, minpt.z, 1.f);
    pntAABB[6] = glm::vec4(maxpt.x, minpt.y, minpt.z, 1.f);
    pntAABB[7] = glm::vec4(maxpt.x, minpt.y, maxpt.z, 1.f);
    
    int indice = 0;

    idxAABB.push_back(glm::ivec2(indice, indice + 1));
    idxAABB.push_back(glm::ivec2(indice + 1, indice + 2));
    idxAABB.push_back(glm::ivec2(indice + 2, indice + 3));
    idxAABB.push_back(glm::ivec2(indice + 3, indice));

    idxAABB.push_back(glm::ivec2(indice + 4, indice + 5));
    idxAABB.push_back(glm::ivec2(indice + 5, indice + 6));
    idxAABB.push_back(glm::ivec2(indice + 6, indice + 7));
    idxAABB.push_back(glm::ivec2(indice + 7, indice + 4));

    idxAABB.push_back(glm::ivec2(indice + 7, indice + 1));
    idxAABB.push_back(glm::ivec2(indice + 4, indice + 2));
    idxAABB.push_back(glm::ivec2(indice + 5, indice + 3));
    idxAABB.push_back(glm::ivec2(indice + 6, indice));


    // setup vao
    GLuint Pbuff; // point buffer
    GLuint Ibuff; // indice buffer

    glGenVertexArrays(1, &vaoidAABB);
    glGenBuffers(1, &Pbuff);
    glGenBuffers(1, &Ibuff);

    glBindVertexArray(vaoidAABB);
    glBindBuffer(GL_ARRAY_BUFFER, Pbuff);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 8, &pntAABB[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ibuff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 2 * idxAABB.size(),
        &idxAABB[0], GL_STATIC_DRAW);

    glBindVertexArray(0); // unbind vao
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind ebo


    //// setup shader
    //std::vector<std::pair<GLenum, std::string>> shdr_files;
    //// Vertex Shader
    //shdr_files.emplace_back(std::make_pair(
    //    GL_VERTEX_SHADER,
    //    "Shaders/abnb2.vert"));

    //// Fragment Shader
    //shdr_files.emplace_back(std::make_pair(
    //    GL_FRAGMENT_SHADER,
    //    "Shaders/debugAABB.frag"));

    //std::cout << "DEBUG AABB SHADER\n";
    //shaderAABB.CompileLinkValidate(shdr_files);
    //std::cout << "\n\n";

    //// if linking failed
    //if (GL_FALSE == shaderAABB.IsLinked()) {
    //    std::stringstream sstr;
    //    sstr << "Unable to compile/link/validate shader programs\n";
    //    sstr << shaderAABB.GetLog() << "\n";
    //    std::cout << sstr.str();
    //    std::exit(EXIT_FAILURE);
    //}
}

void Model::debugAABB_draw(glm::mat4 & SRT)
{
    shaderAABB.Use();
    // UNIFORM VARIABLES ----------------------------------------
    // Persp Projection
    GLint uniform_var_loc1 =
        glGetUniformLocation(this->shaderAABB.GetHandle(),
            "persp_projection");
    glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE,
        glm::value_ptr(EditorCam.GetProjMatrix()));
    GLint uniform_var_loc2 =
        glGetUniformLocation(this->shaderAABB.GetHandle(),
            "View");
    glUniformMatrix4fv(uniform_var_loc2, 1, GL_FALSE,
        glm::value_ptr(EditorCam.GetViewMatrix()));

    // Scuffed SRT
    GLint uniform_var_loc3 =
        glGetUniformLocation(this->shaderAABB.GetHandle(),
            "SRT");
    glUniformMatrix4fv(uniform_var_loc3, 1, GL_FALSE,
        glm::value_ptr(SRT));

    glBindVertexArray(vaoidAABB);
    glDrawElements(GL_LINES, (GLsizei)(2 * idxAABB.size()), GL_UNSIGNED_INT, 0);

    // unbind and free stuff
    glBindVertexArray(0);
    shaderAABB.UnUse();
}

void RaycastLine::lineinit()
{
    GLfloat vertices[] =
    {
        0.f,0.f,0.f
        ,1.f,1.f,1.f
    };

    GLushort elements[] =
    {
        0,1
    };
    unsigned int temp_vaoID;
    glGenVertexArrays(1, &temp_vaoID);
    glBindVertexArray(temp_vaoID);


    GLuint vbo_vertices;
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint ibo_elements;
    glGenBuffers(1, &ibo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), &elements[0], GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    vaoid = temp_vaoID;

    //// setup shader
    //std::vector<std::pair<GLenum, std::string>> shdr_files;
    //// Vertex Shader
    //shdr_files.emplace_back(std::make_pair(
    //    GL_VERTEX_SHADER,
    //    "Shaders/abnb2.vert"));

    //// Fragment Shader
    //shdr_files.emplace_back(std::make_pair(
    //    GL_FRAGMENT_SHADER,
    //    "Shaders/debugAABB.frag"));

    //std::cout << "DEBUG AABB SHADER\n";
    //shaderAABB.CompileLinkValidate(shdr_files);
    //std::cout << "\n\n";

    //// if linking failed
    //if (GL_FALSE == shaderAABB.IsLinked()) {
    //    std::stringstream sstr;
    //    sstr << "Unable to compile/link/validate shader programs\n";
    //    sstr << shaderAABB.GetLog() << "\n";
    //    std::cout << sstr.str();
    //    std::exit(EXIT_FAILURE);
    //}

}

void RaycastLine::debugline_draw(glm::mat4& SRT)
{
    shaderAABB.Use();
    // UNIFORM VARIABLES ----------------------------------------
    // Persp Projection
    GLint uniform_var_loc1 =
        glGetUniformLocation(this->shaderAABB.GetHandle(),
            "persp_projection");
    glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE,
        glm::value_ptr(EditorCam.GetProjMatrix()));
    GLint uniform_var_loc2 =
        glGetUniformLocation(this->shaderAABB.GetHandle(),
            "View");
    glUniformMatrix4fv(uniform_var_loc2, 1, GL_FALSE,
        glm::value_ptr(EditorCam.GetViewMatrix()));

    // Scuffed SRT
    GLint uniform_var_loc3 =
        glGetUniformLocation(this->shaderAABB.GetHandle(),
            "SRT");
    glUniformMatrix4fv(uniform_var_loc3, 1, GL_FALSE,
        glm::value_ptr(SRT));

    glBindVertexArray(vaoid);
    glDrawElements(GL_LINES,2, GL_UNSIGNED_SHORT, 0);

    // unbind and free stuff
    glBindVertexArray(0);
    shaderAABB.UnUse();
}

void SkyBox::SkyBoxinit()
{
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vaoid = skyboxVAO;
    vboid = skyboxVBO;

}

void SkyBox::SkyBoxDraw(GLuint skyboxtex, BaseCamera& _camera)
{
    GLSLShader& skyBoxShader = SHADER.GetShader(SHADERTYPE::SKYBOX);
    skyBoxShader.Use();
    GLint uniform_var_loc1 =
        glGetUniformLocation(this->shader.GetHandle(),
            "projection");

    glUniformMatrix4fv(uniform_var_loc1, 1, GL_FALSE,
        glm::value_ptr(_camera.GetProjMatrix()));
    
    
    GLint uniform_var_loc2 =
        glGetUniformLocation(this->shader.GetHandle(),
            "view");

    glUniformMatrix4fv(uniform_var_loc2, 1, GL_FALSE,
        glm::value_ptr(glm::mat4(glm::mat3(_camera.GetViewMatrix()))));

    glBindVertexArray(vaoid);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxtex);
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    skyBoxShader.UnUse();

}
