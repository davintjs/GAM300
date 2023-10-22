/*!***************************************************************************************
\file			GraphicsSystem.cpp
\project
\author         Lian Khai Kiat, Euan Lim, Theophelia Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definations of Graphics System

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "GraphicsSystem.h"
#include "GraphicsHeaders.h"

#include "Editor/EditorCamera.h"
#include "Core/SystemsGroup.h"
#include "Scene/SceneManager.h"
#include "Core/EventsManager.h"

extern std::unordered_map<std::string, Engine::GUID> PRIMITIVES
{
	{"Cube", Engine::GUID(0)},
	{"Sphere", Engine::GUID(1)},
	{"Capsule", Engine::GUID(2)},
	{"Line", Engine::GUID(3)},
	{"Plane", Engine::GUID(4)},
	{"Segment3D", Engine::GUID(4)},
};

using GraphicsSystemsPack =
TemplatePack
<
	ShaderManager,
	DebugDraw,
	Lighting,
	Renderer
>;

using GraphicsSubSystems = decltype(SystemsGroup(GraphicsSystemsPack()));

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
unsigned int cameraQuadVAO = 0;
unsigned int cameraQuadVBO;


extern unsigned int depthMap;
extern unsigned int depthCubemap;

void renderQuad(unsigned int& _quadVAO, unsigned int& _quadVBO)
{
	if (_quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		//	float quadVertices[] = {
//		// pos	           // tex
//		-1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
//		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
//		-1.0f, -1.0f,0.0f, 0.0f, 0.0f,

//		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
//		1.0f, 1.0f,  0.0f, 1.0f, 1.0f,
//		1.0f, -1.0f, 0.0f, 1.0f, 0.0f
//	};

		// setup plane VAO
		glGenVertexArrays(1, &_quadVAO);
		glGenBuffers(1, &_quadVBO);
		glBindVertexArray(_quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(_quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}

void GraphicsSystem::Init()
{
	// All subsystem initialize
	GraphicsSubSystems::Init();
	SkyboxManager::Instance().Init();
	glEnable(GL_EXT_texture_sRGB); // Unsure if this is required
	EditorCam.Init();
}

void GraphicsSystem::Update(float dt)
{
	// All subsystem updates
	GraphicsSubSystems::Update(dt);

	// Editor Camera
	EditorWindowEvent e("Scene");
	EVENTS.Publish(&e);

	if (e.isOpened)
	{
		EditorCam.Update(dt);

		PreDraw(EditorCam, quadVAO, quadVBO);
	}

	// Game Cameras
	EditorWindowEvent e1("Game");
	EVENTS.Publish(&e1);

	if (e1.isOpened)
	{
		Scene& currentScene = MySceneManager.GetCurrentScene();
		for (Camera& camera : currentScene.GetArray<Camera>())
		{
			Transform* transform = &currentScene.Get<Transform>(camera.EUID());

			// Update camera view 
			camera.UpdateCamera(transform->translation, transform->rotation);

			PreDraw(camera, cameraQuadVAO, cameraQuadVBO);
		}
	}

	PostDraw();
}

void GraphicsSystem::PreDraw(BaseCamera& _camera, unsigned int& _vao, unsigned int& _vbo)
{
	
	glViewport(0, 0, 1600, 900);
	glBindFramebuffer(GL_FRAMEBUFFER, _camera.GetFramebuffer().hdrFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);

	Draw(_camera); // call draw after update
	RENDERER.UIDraw_3D(_camera); // call draw after update

	if (_camera.GetCameraType() == CAMERATYPE::GAME)
		Draw_Screen(_camera);

	_camera.GetFramebuffer().Unbind();

	_camera.GetFramebuffer().Bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.5f, 0.5f, 1.f);

	GLSLShader& shader = SHADER.GetShader(HDR);
	shader.Use();

	// Bean: This is not being used right now if the camera is using colorBuffer, will be used if using ColorAttachment when drawing in the camera
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _camera.GetFramebuffer().colorBuffer);

	GLint uniform1 =
		glGetUniformLocation(shader.GetHandle(), "hdr");

	glUniform1i(uniform1, RENDERER.IsHDR());

	GLint uniform2 =
		glGetUniformLocation(shader.GetHandle(), "exposure");

	glUniform1f(uniform2, RENDERER.GetExposure());

	renderQuad(_vao, _vbo);
	shader.UnUse();

	_camera.GetFramebuffer().Unbind();
}

void GraphicsSystem::Draw(BaseCamera& _camera) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.5f, 0.5f, 1.f);
	glEnable(GL_DEPTH_BUFFER);

	RENDERER.Draw(_camera);

	if (_camera.GetCameraType() == CAMERATYPE::SCENE)
		DEBUGDRAW.Draw();

	MYSKYBOX.Draw(_camera);
}

void GraphicsSystem::Draw_Screen(BaseCamera& _camera)
{
	// IDK if this is gonna be the final iteration, but it will loop through all the sprites 1 by 1 to render
	RENDERER.UIDraw_2D(_camera);

}

void GraphicsSystem::PostDraw()
{
	for (auto& [name, prop] : RENDERER.GetProperties())
	{
		prop.iter = 0;
	}
}

void GraphicsSystem::Exit()
{
	//CLEANUP GRAPHICS HERE
	
	// All subsystem exit
	GraphicsSubSystems::Exit();
}

void GraphicsSystem::OldUpdate()
{
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
}