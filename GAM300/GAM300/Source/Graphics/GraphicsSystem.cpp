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
//#include "ParticleSystem/ParticleSystem.h"
#include "ParticleRenderer.h"
#include "Editor/EditorCamera.h"
#include "Core/SystemsGroup.h"
#include "Scene/SceneManager.h"
#include "Core/EventsManager.h"
//#include "AnimationManager.h"
#include "MeshManager.h"
#include "Texture/TextureManager.h"
#include "IOManager/InputHandler.h"
#include <glm/gtx/matrix_decompose.hpp>
#include "AppEntry/Application.h"

using GraphicsSystemsPack =
TemplatePack
<
	MeshManager,
	ShaderManager,
	FramebufferManager,
	Texture_Manager,
	MaterialSystem,
	DebugDraw,
	Lighting,
	Renderer,
	Shadows,
	ParticleRenderer,
	TextSystem
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

// Bloom
unsigned int pingpongFBO[2];
unsigned int pingpongColorbuffers[2];

extern glm::vec2 windowPos; // In ColourPicker.cpp
extern glm::vec2 windowDimension; // In ColourPicker.cpp

std::vector<temp_instance> temporary_presets;

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

void renderQuadWireMesh(unsigned int& _quadVAO, unsigned int& _quadVBO)
{
	if (_quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 
			-1.0f, -1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 
			 1.0f, -1.0f, 0.0f, 

			 1.0f, -1.0f, 0.0f, 
			 -1.0f, -1.0f, 0.0f, 
			 1.0f, 1.0f, 0.0f, 
			 -1.0f, 1.0f, 0.0f, 
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(_quadVAO);
	glDrawArrays(GL_LINES, 0, 8);
	//glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}


bool bloom(unsigned int amount, unsigned int VAO, unsigned int VBO, BaseCamera& _camera)
{
	bool horizontal = true, first_iteration = true;
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::BLUR);
	shader.Use();
	
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);

		glUniform1f(glGetUniformLocation(shader.GetHandle(), "horizontal"), horizontal);

		/*glBindTexture(
			GL_TEXTURE_2D, first_iteration ? _camera.GetFramebuffer().colorBuffer[1] : pingpongColorbuffers[!horizontal]
		);*/

		glBindTexture(GL_TEXTURE_2D, first_iteration ? FRAMEBUFFER.GetTextureID(_camera.GetFramebufferID(), _camera.GetBloomAttachment()) : pingpongColorbuffers[!horizontal]);

		renderQuad(VAO, VBO);
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	shader.UnUse();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return !horizontal;

}

void GraphicsSystem::Init()
{
	temp_instance emissionMat;
	emissionMat.name = "Emission material";
	emissionMat.albedo = glm::vec4(0.f, 255.f, 255.f, 255.f);
	emissionMat.metallic = 1.f;
	emissionMat.roughness = 1.f;
	emissionMat.ao = 1.f;

	temp_instance blackSurfaceMat;
	blackSurfaceMat.name = "Black Surface material";
	blackSurfaceMat.albedo = glm::vec4(0.f, 0.f, 0.f, 255.f);
	blackSurfaceMat.metallic = 1.f;
	blackSurfaceMat.roughness = 1.f;
	blackSurfaceMat.ao = 1.f;

	temp_instance darkBlueMat;
	darkBlueMat.name = "Dark Blue material";
	darkBlueMat.albedo = glm::vec4(73.f, 85.f, 128.f, 255.f);
	darkBlueMat.metallic = 1.f;
	darkBlueMat.roughness = 1.f;
	darkBlueMat.ao = 1.f;

	temporary_presets.emplace_back(emissionMat);
	temporary_presets.emplace_back(blackSurfaceMat);
	temporary_presets.emplace_back(darkBlueMat);

	// All subsystem initialize
	GraphicsSubSystems::Init();
	SkyboxManager::Instance().Init();
	COLOURPICKER.Init();
	AnimationManager.Init();
	glEnable(GL_EXT_texture_sRGB); // Unsure if this is required
	EditorCam.Init();
	BLOOMER.Init(1600, 900);
	FRAMEBUFFER.CreateBloom(pingpongFBO, pingpongColorbuffers);
}

void GraphicsSystem::Update(float dt)
{
	//// Game Cameras
	//EditorWindowEvent e1("Game");
	//EVENTS.Publish(&e1);

	//if (e1.isOpened)
	//{
	//	Scene& currentScene = MySceneManager.GetCurrentScene();
	//	for (Camera& camera : currentScene.GetArray<Camera>())
	//	{
	//		if (camera.state == DELETED) continue;

	//		COLOURPICKER.ColorPickingUIButton(camera);

	//	}
	//}
	// All subsystem updates
	GraphicsSubSystems::Update(dt);
	AnimationManager.Update(dt);

#if defined(_BUILD)
	Scene& currentScene = MySceneManager.GetCurrentScene();
	for (Camera& camera : currentScene.GetArray<Camera>())
	{
		if (camera.state == DELETED) continue;

		Transform* transform = &currentScene.Get<Transform>(camera.EUID());
		camera.TryResize(glm::vec2(Application::GetWidth(), Application::GetHeight()));
		windowDimension = camera.GetViewportSize();
		windowPos = glm::vec2(0.f, 0.f);

		// Update camera view 
		camera.UpdateCamera(transform->GetGlobalTranslation(), transform->GetGlobalRotation());

		//COLOURPICKER.ColorPickingUIButton(camera);

		PreDraw(camera, cameraQuadVAO, cameraQuadVBO);
	}
#else
	// Editor Camera
	EditorWindowEvent e("Scene");
	EVENTS.Publish(&e);

	if (e.isOpened)
	{
		EditorCam.Update(dt);

		PreDraw(EditorCam, quadVAO, quadVBO);
	}

	// Game Cameras
	Scene& currentScene = MySceneManager.GetCurrentScene();
	for (Camera& camera : currentScene.GetArray<Camera>())
	{
		if (camera.state == DELETED) continue;

		Transform* transform = &currentScene.Get<Transform>(camera.EUID());


		const glm::vec3 translation = transform->GetGlobalTranslation();
		const glm::vec3 rotation = transform->GetGlobalRotation();
		// Update camera view 
		camera.UpdateCamera(translation, rotation);

		COLOURPICKER.ColorPickingUIButton(camera);

		PreDraw(camera, cameraQuadVAO, cameraQuadVBO);
	}
#endif	

	PostDraw();
}

void GraphicsSystem::PreDraw(BaseCamera& _camera, unsigned int& _vao, unsigned int& _vbo)
{
	FRAMEBUFFER.Bind(_camera.GetFramebufferID(), _camera.GetHDRAttachment());
	unsigned int attachments[2] = { _camera.GetHDRAttachment(), _camera.GetBloomAttachment() };
	glDrawBuffers(2, attachments);

	Draw(_camera);
	UIRENDERER.UIDraw_3D(_camera);

	FRAMEBUFFER.Unbind();

	if (RENDERER.enableBloom())
	{
		BLOOMER.RenderBloomTexture(0.005, _camera, cameraQuadVAO, cameraQuadVBO);
		
		FRAMEBUFFER.Bind(_camera.GetFramebufferID(), _camera.GetHDRAttachment());
		GLSLShader& shader = SHADER.GetShader(SHADERTYPE::MERGE_BLOOM);
		shader.Use();

		//bool index = bloom(RENDERER.GetBloomCount(), _vao, _vbo, _camera);
		
		glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[index]);

		glBindTexture(GL_TEXTURE_2D, BLOOMER.BloomTexture());

		// Bean: This is not being used right now if the camera is using colorBuffer, will be used if using ColorAttachment when drawing in the camera
		glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, _camera.GetFramebuffer().colorBuffer[0]);

		glBindTexture(GL_TEXTURE_2D, FRAMEBUFFER.GetTextureID(_camera.GetFramebufferID(), _camera.GetHDRAttachment()));
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[index]); 


		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "bloomStrength");
		glUniform1f(uniform1, (float)(RENDERER.GetBloomCount()) / 100.f);

		renderQuad(_vao, _vbo);
		shader.UnUse();

		FRAMEBUFFER.Unbind();
	}

	// Draw debug drawing without being affected by the bloom
	FRAMEBUFFER.Bind(_camera.GetFramebufferID(), _camera.GetHDRAttachment());

	if (_camera.GetCameraType() == CAMERATYPE::GAME)
	{
		UIRENDERER.UIDraw_2D(_camera);
		//TEXTSYSTEM.Draw(_camera);
	}
	else
	{
		UIRENDERER.UIDraw_2DWorldSpace(_camera);
	}
	if (_camera.GetCameraType() == CAMERATYPE::SCENE)
	{
		DEBUGDRAW.Draw();
	}

	TEXTSYSTEM.Draw(_camera);

	PARTICLERENDER.Draw2D(_camera);

	FRAMEBUFFER.Unbind();

#if defined(_BUILD)
	//GLsizei height = Application::GetWidth() / 16.f * 9.f;
	//GLint offset = GLint((Application::GetHeight() - height) * 0.5f);
	glViewport(0, 0, Application::GetWidth(), Application::GetHeight());
#else
	FRAMEBUFFER.Bind(_camera.GetFramebufferID(), _camera.GetAttachment());
	glDrawBuffer(_camera.GetAttachment());
#endif

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.5f, 0.5f, 1.f);

	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::HDR);
	shader.Use();

	// Bean: This is not being used right now if the camera is using colorBuffer, will be used if using ColorAttachment when drawing in the camera
	glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, _camera.GetFramebuffer().colorBuffer[0]);

	glBindTexture(GL_TEXTURE_2D, FRAMEBUFFER.GetTextureID(_camera.GetFramebufferID(), _camera.GetHDRAttachment()));
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[index]); 

	GLint uniform1 =
		glGetUniformLocation(shader.GetHandle(), "hdr");

	glUniform1i(uniform1, RENDERER.IsHDR());

	GLint uniform2 =
		glGetUniformLocation(shader.GetHandle(), "exposure");

	glUniform1f(uniform2, RENDERER.GetExposure());

	GLint uniform3 =
		glGetUniformLocation(shader.GetHandle(), "gammaCorrection");

	glUniform1f(uniform3, RENDERER.getGamma());

	/*GLint uniform3 =
		glGetUniformLocation(shader.GetHandle(), "enableBloom");

	glUniform1f(uniform3, RENDERER.enableBloom());

	GLint uniform4 =
		glGetUniformLocation(shader.GetHandle(), "bloomStrength");

	glUniform1f(uniform4, (float)(RENDERER.GetBloomCount())/100.f);*/

	renderQuad(_vao, _vbo);
	shader.UnUse();

#if defined(_BUILD)

#else
	FRAMEBUFFER.Unbind();
#endif
	
}

void GraphicsSystem::Draw(BaseCamera& _camera) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.5f, 0.5f, 1.f);

	glEnable(GL_DEPTH_BUFFER);
	MYSKYBOX.Draw(_camera);

	RENDERER.Draw(_camera);
	PARTICLERENDER.Draw(_camera);
}

void GraphicsSystem::PostDraw()
{
	//@kk clear the one with shader instead
	for (int i = 0; i < static_cast<int>(SHADERTYPE::COUNT); ++i) {
		for (auto& [name, prop] : RENDERER.GetInstanceContainer()[i])
		{
			prop.iter = 0;
		}
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
