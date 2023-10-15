/*!***************************************************************************************
\file			GraphicsSystem.cpp
\project
\author         Lian Khai Kiat, Euan Lim, Theophelia Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definations of Graphics System

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "GraphicsSystem.h"
#include "GraphicsHeaders.h"

#include "Editor/EditorCamera.h"
#include "Core/SystemsGroup.h"

using GraphicsSystemsPack =
TemplatePack
<
	ShaderManager,
	SkyboxManager,
	DebugDraw,
	Lighting,
	Renderer
>;

using GraphicsSubSystems = decltype(SystemsGroup(GraphicsSystemsPack()));

std::vector <glm::vec4> temp_AlbedoContainer;
std::vector <glm::vec4> temp_SpecularContainer;
std::vector <glm::vec4> temp_DiffuseContainer;
std::vector <glm::vec4> temp_AmbientContainer;
std::vector <float> temp_ShininessContainer;

//bool isThereLight = false;

//void InstanceSetup(GLuint vaoid);
//void InstancePropertySetup(InstanceProperties& prop);

//std::vector<std::string> faces
//{
//	FileSystem::getPath("resources/textures/skybox/right.jpg"),
//	FileSystem::getPath("resources/textures/skybox/left.jpg"),
//	FileSystem::getPath("resources/textures/skybox/top.jpg"),
//	FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
//	FileSystem::getPath("resources/textures/skybox/front.jpg"),
//	FileSystem::getPath("resources/textures/skybox/back.jpg")
//};

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;

void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void GraphicsSystem::Init()
{
	// All subsystem initialize
	GraphicsSubSystems::Init();

	glEnable(GL_EXT_texture_sRGB); // Unsure if this is required

	EditorCam.Init();
}

void GraphicsSystem::Update(float dt)
{
	// Temporary Material thing
	//temp_MaterialContainer[3].Albedo = glm::vec4{ 1.f,1.f,1.f,1.f };
	temp_DiffuseContainer[3] = glm::vec4{ 1.0f, 0.5f, 0.31f,1.f };
	temp_SpecularContainer[3] = glm::vec4{ 0.5f, 0.5f, 0.5f,1.f };
	temp_AmbientContainer[3] = glm::vec4{ 1.0f, 0.5f, 0.31f,1.f };
	temp_ShininessContainer[3] = 32.f;

	temp_AlbedoContainer[3].r = static_cast<float>(sin(glfwGetTime() * 2.0));
	temp_AlbedoContainer[3].g = static_cast<float>(sin(glfwGetTime() * 0.7));
	temp_AlbedoContainer[3].b = static_cast<float>(sin(glfwGetTime() * 1.3));

	// All subsystem updates
	GraphicsSubSystems::Update(dt);

	//Currently Putting in Camera Update loop here
	EditorCam.Update(dt);

	// Dont delete this -> To run on lab computers
	/*GLint maxVertexAttribs;
	glGetProgramiv(temp_instance_shader.GetHandle(), GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxVertexAttribs);
	std::cout << "max vertex attribs :" << maxVertexAttribs << "\n";*/

	// DONT DELETE THIS - EUAN need to check if like got padding or anything cause it wil break the instancing

	/*
	std::cout << "size of material struct is : " << sizeof(Materials) << "\n";

	Materials materialsArray[3]; // Create an array of 3 Materials
	// Calculate the size of the array
	size_t sizeOfArray = sizeof(materialsArray);

	std::cout << "Size of Materials array: " << sizeOfArray << " bytes" << std::endl;
	*/

	// Using Mesh Manager
	/*
	// instanced bind
	glBindBuffer(GL_ARRAY_BUFFER, MeshManager.mContainer.find("Cube")->second.SRT_Buffer_Index[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &entitySRT[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (auto mesh = MeshManager.mContainer.begin(); mesh != MeshManager.mContainer.end(); mesh++)
	{

		// Looping through submeshes
		for (int k = 0; k < mesh->second.SRT_Buffer_Index.size(); ++k)
		{
			glBindBuffer(GL_ARRAY_BUFFER, mesh->second.SRT_Buffer_Index[k]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &SRT_Buffers[mesh->second.index].transformation_mat[0]);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			Draw_Meshes(mesh->second.Vaoids[k], SRT_Buffers[mesh->second.index].index + 1, mesh->second.Drawcounts[k], mesh->second.prim,Lighting_Source);
		}
		SRT_Buffers[mesh->second.index].index = 0;
	}
	*/

	glViewport(0, 0, 1600, 900);
	glBindFramebuffer(GL_FRAMEBUFFER, EditorCam.GetFramebuffer().hdrFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);

	Draw(); // call draw after update

	EditorCam.GetFramebuffer().unbind();

	EditorCam.GetFramebuffer().bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.5f, 0.5f, 1.f);

	// Bean: For unbinding framebuffer

	GLSLShader& shader = SHADER.GetShader(HDR);
	shader.Use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, EditorCam.GetFramebuffer().colorBuffer);

	GLint uniform1 =
		glGetUniformLocation(shader.GetHandle(), "hdr");

	glUniform1i(uniform1, RENDERER.IsHDR());

	GLint uniform2 =
		glGetUniformLocation(shader.GetHandle(), "exposure");

	glUniform1f(uniform2, RENDERER.GetExposure());

	renderQuad();
	shader.UnUse();

	EditorCam.GetFramebuffer().unbind();
}

void GraphicsSystem::Draw() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.5f, 0.5f, 1.f);
	glEnable(GL_DEPTH_BUFFER);

	RENDERER.Draw();

	MYSKYBOX.Draw();
}

void GraphicsSystem::Exit()
{
	//std::cout << "-- Graphics Exit -- " << std::endl;

	//CLEANUP GRAPHICS HERE
	
	// All subsystem exit
	GraphicsSubSystems::Exit();
}
