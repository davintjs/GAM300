/*!***************************************************************************************
\file			Renderer.cpp
\project
\author

\par			Course: GAM300
\date           11/10/2023

\brief
	This file contains the definitions of Graphics Renderer that includes:
	1.

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Scene/SceneManager.h"
#include "MeshManager.h"
#include "Editor/EditorCamera.h"

// ALL THIS ARE HOPEFULLY TEMPORARY

// Shadow Mapping - Directional
unsigned int depthMapFBO; 
unsigned int depthMap; // Shadow Texture

glm::mat4 lightSpaceMatrix_directional;

// Shadow Mapping - Spot
unsigned int depthMapFBO_S;
unsigned int depthMap_S; // Shadow Texture

glm::mat4 lightSpaceMatrix_spot;


// Shadow Cube Mapping
unsigned int depthCubemapFBO;
unsigned int depthCubemap;


//LIGHT_TYPE temporary_test4 = POINT_LIGHT;

LightProperties spot_light_stuffs;
LightProperties directional_light_stuffs;
LightProperties point_light_stuffs;

#include "GBuffer.h"
unsigned int Renderer_quadVAO = 0;
unsigned int Renderer_quadVBO = 0;

unsigned int Renderer_quadVAO_WM = 0;
unsigned int Renderer_quadVBO_WM = 0;

const unsigned int SHADOW_WIDTH = 512, SHADOW_HEIGHT = 512;
const unsigned int SHADOW_WIDTH_DIRECTIONAL = 4096, SHADOW_HEIGHT_DIRECTIONAL = 4096;


void Renderer::Init()
{
	//instanceContainers.resize(static_cast<size_t>(SHADERTYPE::COUNT));
	m_gBuffer.Init(1600, 900);
	
	FRAMEBUFFER.CreateDirectionalAndSpotLight(depthMapFBO, depthMap, SHADOW_WIDTH_DIRECTIONAL, SHADOW_HEIGHT_DIRECTIONAL);

	FRAMEBUFFER.CreateDirectionalAndSpotLight(depthMapFBO_S, depthMap_S, SHADOW_WIDTH, SHADOW_HEIGHT);

	FRAMEBUFFER.CreatePointLight(depthCubemapFBO, depthCubemap, SHADOW_WIDTH, SHADOW_HEIGHT);

	SetupGrid(100);


}

void Renderer::Update(float)
{
	// maybe no need clear every time?
	for (InstanceContainer& container : instanceContainers) {
		for (auto& [vao, prop] : container)
		{
			std::fill_n(prop.textureIndex, EnitityInstanceLimit, glm::vec2(0.f));
			std::fill_n(prop.M_R_A_Texture, EnitityInstanceLimit, glm::vec4(33.f));
			std::fill_n(prop.texture, 32, 0);
		}
	}
	instanceContainers.clear(); // clear then emplace back? coz spcific vao in specific shader?
	instanceContainers.resize(static_cast<size_t>(SHADERTYPE::COUNT)); // need this meh? alr reserve tho
	defaultProperties.clear(); // maybe no need clear everytime, see steve rabin code?

	Scene& currentScene = SceneManager::Instance().GetCurrentScene();

	int i = 0;

	for (MeshRenderer& renderer : currentScene.GetArray<MeshRenderer>())
	{
		//int index = t_Mesh->index;

		Entity& entity = currentScene.Get<Entity>(renderer);
		Transform& transform = currentScene.Get<Transform>(entity);

		// Loop through camera (@euan)

		// for each cam

		// if instance rendering put into container for instance rendering
		// 
		//if (renderer.isInstance) {

			// dun need do dis, coz renderer contains shadertype, can access via that
			// for each shader, slot in properties into container
			/*for (int s = 0; s < static_cast<int>(SHADERTYPE::COUNT); ++s) {
				//SHADERTYPE sType = static_cast<SHADERTYPE>(s);
				instanceProperties.emplace();
				instanceContainers[s].emplace();
			}*/

			size_t s = static_cast<size_t>(renderer.shaderType);
			if (MeshManager.vaoMap.find(renderer.meshID) == MeshManager.vaoMap.end())
				continue;
			GLuint vao = MeshManager.vaoMap[renderer.meshID]; // pls ask someone how to use GUID instead because deadlock
			//Mesh& mesh = MeshManager.mContainer[renderer.meshID]; // pls ask someone how to use GUID instead because deadlock

			//instanceProperties[vao];
			//instanceContainers[s][vao]; // holy shit u can do this?? this is map in a vec sia

			// slot in the InstanceProperties into this vector if it doesnt alr exist
			if (instanceContainers[s].find(vao) == instanceContainers[s].cend()) {
				instanceContainers[s].emplace(std::pair(vao, instanceProperties[vao]));
			}

			// use the properties container coz its made for instance rendering already

			float texidx = float(ReturnTextureIdx(instanceContainers[s][vao],TextureManager.GetTexture(renderer.AlbedoTexture)));
			float normidx = float(ReturnTextureIdx(instanceContainers[s][vao], TextureManager.GetTexture(renderer.NormalMap)));

			float metalidx = float(ReturnTextureIdx(instanceContainers[s][vao], TextureManager.GetTexture(renderer.MetallicTexture)));
			float roughidx = float(ReturnTextureIdx(instanceContainers[s][vao], TextureManager.GetTexture(renderer.RoughnessTexture)));
			float aoidx = float(ReturnTextureIdx(instanceContainers[s][vao], TextureManager.GetTexture(renderer.AoTexture)));
			float emissionidx = float(ReturnTextureIdx(instanceContainers[s][vao], TextureManager.GetTexture(renderer.EmissionTexture)));

			float metal_constant = renderer.mr_metallic;
			float rough_constant = renderer.mr_roughness;
			float ao_constant = renderer.ao;
			float emission_constant = renderer.emission;

			unsigned int& iter = instanceContainers[s][vao].iter;
			instanceContainers[s][vao].M_R_A_Constant[iter] = glm::vec4(metal_constant, rough_constant, ao_constant, emission_constant);
			instanceContainers[s][vao].M_R_A_Texture[iter] = glm::vec4(metalidx, roughidx, aoidx, emissionidx);
			instanceContainers[s][vao].textureIndex[iter] = glm::vec2(texidx, normidx);

			instanceContainers[s][vao].Albedo[iter] = renderer.mr_Albedo;
			instanceContainers[s][vao].Ambient[iter] = renderer.mr_Ambient;
			instanceContainers[s][vao].Diffuse[iter] = renderer.mr_Diffuse;
			instanceContainers[s][vao].Specular[iter] = renderer.mr_Specular;
			instanceContainers[s][vao].Shininess[iter] = renderer.mr_Shininess;
			instanceContainers[s][vao].entitySRT[iter] = transform.GetWorldMatrix();
			//instanceContainers[s][renderer.meshID].VAO = vao;
			++iter;
			
			// @jake rmb to do submesh...

			//++(instanceProperties[renderer.MeshName].iter);
			//
			//// sub meshes into instance properties
			//char maxcount = 32;
			//for (char namecount = 0; namecount < maxcount; ++namecount)
			//{
			//	std::string newName = renderer.MeshName;

			//	newName += ('1' + namecount);

			//	if (instanceProperties.find(newName) == instanceProperties.end())
			//	{
			//		break;
			//	}
			//	//InstanceinstanceProperties* currentProp = &instanceProperties[renderer.MeshName];	

			//	/*GLuint textureID = 0;
			//	GLuint normalMapID = 0;*/

			//	instanceProperties[newName].entitySRT[instanceProperties[newName].iter] = transform.GetWorldMatrix();
			//	instanceProperties[newName].Albedo[instanceProperties[newName].iter] = renderer.mr_Albedo;
			//	instanceProperties[newName].Ambient[instanceProperties[newName].iter] = renderer.mr_Ambient;
			//	instanceProperties[newName].Diffuse[instanceProperties[newName].iter] = renderer.mr_Diffuse;
			//	instanceProperties[newName].Specular[instanceProperties[newName].iter] = renderer.mr_Specular;
			//	instanceProperties[newName].Shininess[instanceProperties[newName].iter] = renderer.mr_Shininess;

			//	instanceProperties[newName].M_R_A_Texture[instanceProperties[newName].iter] = glm::vec4(metalidx, roughidx, aoidx, emissionidx);
			//	instanceProperties[newName].M_R_A_Constant[instanceProperties[newName].iter] = glm::vec3(metal_constant, rough_constant, ao_constant);

			//	++(instanceProperties[newName].iter);
			//}

		//}
		//else /*if default rendering*/{
			// if not instance put into container for default rendering

			// batch it via shader, geom, material instanced
			// whenever things reach limit, draw
			// means need calculate SRT here
			Mesh* t_Mesh = MeshManager.DereferencingMesh(renderer.meshID);

			if (t_Mesh == nullptr)
			{
				continue;
			}
			unsigned int iters = 0;
			for (unsigned int vao : t_Mesh->Vaoids) {
				DefaultRenderProperties renderProperties;
				
				renderProperties.VAO = vao;

				renderProperties.shininess = renderer.mr_Shininess;
				renderProperties.metallic = renderer.mr_metallic;
				renderProperties.roughness = renderer.mr_roughness;
				renderProperties.ao = renderer.ao;
				renderProperties.emission = renderer.emission;

				renderProperties.entitySRT = transform.GetWorldMatrix();
				renderProperties.Albedo = renderer.mr_Albedo;
				renderProperties.Specular = renderer.mr_Specular;
				renderProperties.Diffuse = renderer.mr_Diffuse;
				renderProperties.Ambient = renderer.mr_Ambient;

				// renderProperties.textureID = renderer.textureID;
				// renderProperties.NormalID = renderer.normalMapID;
				renderProperties.RoughnessID = TextureManager.GetTexture(renderer.RoughnessTexture);
				renderProperties.MetallicID = TextureManager.GetTexture(renderer.MetallicTexture);
				renderProperties.AoID = TextureManager.GetTexture(renderer.AoTexture);
				renderProperties.EmissionID = TextureManager.GetTexture(renderer.EmissionTexture);

				renderProperties.textureID = TextureManager.GetTexture(renderer.AlbedoTexture);
				renderProperties.NormalID = TextureManager.GetTexture(renderer.NormalMap);

				renderProperties.drawType = t_Mesh->prim;
				renderProperties.drawCount = t_Mesh->Drawcounts[iters++];

				defaultProperties.emplace_back(renderProperties);
			}
		//}
		
		++i;
	}

	//properties[MeshManager.vaoMap["Line"]].iter = 200;
	
	if (RENDERER.enableShadows())
	{
		if (spot_light_stuffs.inUse)
		{
			DrawDepthSpot();
		}
		if (directional_light_stuffs.inUse)
		{
			DrawDepthDirectional();
		}
		if (point_light_stuffs.inUse)
		{
			DrawDepthPoint();
		}
	}
}

void Renderer::SetupGrid(const int& _num)
{
	//float spacing = 100.f;
	//float length = _num * spacing * 0.5f;
	// float spacing = 1.f;
	// float length = _num * spacing * 0.5f;

	//instanceProperties["Line"].iter = _num * 2;
	//

	//for (int i = 0; i < _num; i++)
	//{
	//	glm::mat4 scalMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(length));
	//	glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//	glm::mat4 transMatrixZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.0f, (i * spacing) - length));
	//	glm::mat4 transMatrixX = glm::translate(glm::mat4(1.0f), glm::vec3((i * spacing) - length, 0.0f, 0.f));

	//	instanceProperties["Line"].entitySRT[i] = transMatrixZ * scalMatrix; // z axis
	//	instanceProperties["Line"].entitySRT[i + _num] = transMatrixX * rotMatrix * scalMatrix; // x axis
	//}
}

void Renderer::Draw(BaseCamera& _camera)
{
	
	// Looping Properties for instancing
	for (int s = 0; s < static_cast<int>(SHADERTYPE::COUNT); ++s)
	//for (auto& [shader, container] : instanceContainers)
	{
		for (auto& [vao, prop] : instanceContainers[s]) {
			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (EnitityInstanceLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, prop.AlbedoBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.Albedo[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, prop.Metal_Rough_AO_Texture_Buffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.M_R_A_Texture[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, prop.Metal_Rough_AO_Texture_Constant);
			glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.M_R_A_Constant[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, prop.textureIndexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec2), &(prop.textureIndex[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			for (int i = 0; i < 30; ++i)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, prop.texture[i]);
			}
			glActiveTexture(GL_TEXTURE0 + 29);
			glBindTexture(GL_TEXTURE_2D, depthMap_S);
			glActiveTexture(GL_TEXTURE0 + 30);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			DrawMeshes(prop.VAO, prop.iter, prop.drawCount, prop.drawType, LIGHTING.GetLight(), _camera, static_cast<SHADERTYPE>(s));

			// FOR DEBUG DRAW
			if (EditorScene::Instance().DebugDraw() && _camera.GetCameraType() == CAMERATYPE::SCENE)
			{
				glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
				glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				if (prop.debugVAO)
					DrawDebug(prop.debugVAO, prop.iter);
			}

			// @kk this one need uncomment, check vao instead of name
			/*if (name == "Line" && _camera.GetCameraType() == CAMERATYPE::SCENE)
			{
				glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
				glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				if (prop.VAO)
					DrawGrid(prop.VAO, prop.iter);
			}*/
		}
	}

	//// non-instanced render
	//for (DefaultRenderProperties& prop : defaultProperties) {

	//	
	//	//glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer.gFBO);
	//	//unsigned int attachments[3] =
	//	//{ GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 }; // position, normal, albedospec
	//	//glDrawBuffers(3, attachments);
	//	//
	//	
	//	//GLSLShader& geomPass = SHADER.GetShader(GBUFFER);
	//	//geomPass.Use();
	//	//GLint uProj =
	//	//	glGetUniformLocation(geomPass.GetHandle(), "persp_projection");
	//	//GLint uView =
	//	//	glGetUniformLocation(geomPass.GetHandle(), "View");
	//	//GLint SRT =
	//	//	glGetUniformLocation(geomPass.GetHandle(), "SRT");
	//	//
	//	//glUniformMatrix4fv(uProj, 1, GL_FALSE,
	//	//	glm::value_ptr(EditorCam.GetProjMatrix()));
	//	//glUniformMatrix4fv(uView, 1, GL_FALSE,
	//	//	glm::value_ptr(EditorCam.GetViewMatrix()));
	//	//glUniformMatrix4fv(SRT, 1, GL_FALSE,
	//	//	glm::value_ptr(prop.entitySRT));
	//	////m_gBuffer.BindForWriting();
	//	//glBindVertexArray(prop.VAO);
	//	//glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
	//	//glBindVertexArray(0);
	//	//geomPass.UnUse();
	//	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	//// lighting pass
	//	//// render

	//	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	//glActiveTexture(GL_TEXTURE0);
	//	//glBindTexture(GL_TEXTURE_2D, m_gBuffer.gPosition);
	//	//glActiveTexture(GL_TEXTURE1);
	//	//glBindTexture(GL_TEXTURE_2D, m_gBuffer.gNormal);
	//	//glActiveTexture(GL_TEXTURE2);
	//	//glBindTexture(GL_TEXTURE_2D, m_gBuffer.gAlbedoSpec);

	//	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, prop.textureID);
	//	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, prop.NormalID);
	//	glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, prop.RoughnessID);
	//	glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, prop.MetallicID);
	//	glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, prop.AoID);
	//	glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_2D, prop.EmissionID);
	//	glActiveTexture(GL_TEXTURE6); glBindTexture(GL_TEXTURE_2D, depthMap);
	//	glActiveTexture(GL_TEXTURE7); glBindTexture(GL_TEXTURE_2D, depthMap_S);
	//	glActiveTexture(GL_TEXTURE8); glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

	//	GLSLShader& shader =  SHADER.GetShader(SHADERTYPE::DEFAULT);
	//	shader.Use();

	//	// PBR TEXTURES
	//	GLint hasTexture = glGetUniformLocation(shader.GetHandle(), "hasTexture");
	//	GLint hasNormal = glGetUniformLocation(shader.GetHandle(), "hasNormal");
	//	GLint hasRoughness = glGetUniformLocation(shader.GetHandle(), "hasRoughness");
	//	GLint hasMetallic = glGetUniformLocation(shader.GetHandle(), "hasMetallic");
	//	GLint hasAO = glGetUniformLocation(shader.GetHandle(), "hasAO");
	//	GLint hasEmission = glGetUniformLocation(shader.GetHandle(), "hasEmission");

	//	glUniform1i(hasTexture, prop.textureID);
	//	glUniform1i(hasNormal, prop.NormalID);
	//	glUniform1i(hasRoughness, prop.RoughnessID);
	//	glUniform1i(hasMetallic, prop.MetallicID);
	//	glUniform1i(hasAO, prop.AoID);
	//	glUniform1i(hasEmission, prop.EmissionID);

	//	// PBR CONSTANT VALUES
	//	glUniform1f(glGetUniformLocation(shader.GetHandle(), "MetalConstant"), prop.metallic);
	//	glUniform1f(glGetUniformLocation(shader.GetHandle(), "RoughnessConstant"), prop.roughness);
	//	glUniform1f(glGetUniformLocation(shader.GetHandle(), "AoConstant"), prop.ao);
	//	glUniform1f(glGetUniformLocation(shader.GetHandle(), "EmissionConstant"), prop.emission);




	//	GLint uProj = glGetUniformLocation(shader.GetHandle(), "persp_projection");
	//	GLint uView = glGetUniformLocation(shader.GetHandle(), "View");
	//	GLint SRT = glGetUniformLocation(shader.GetHandle(), "SRT");
	//	GLint Albedo = glGetUniformLocation(shader.GetHandle(), "Albedo");
	//	GLint Specular = glGetUniformLocation(shader.GetHandle(), "Specular");
	//	GLint Diffuse = glGetUniformLocation(shader.GetHandle(), "Diffuse");
	//	GLint Ambient = glGetUniformLocation(shader.GetHandle(), "Ambient");
	//	GLint Shininess = glGetUniformLocation(shader.GetHandle(), "Shininess");
	//	GLint lightColor = glGetUniformLocation(shader.GetHandle(), "lightColor");
	//	GLint lightPos = glGetUniformLocation(shader.GetHandle(), "lightPos");
	//	GLint camPos = glGetUniformLocation(shader.GetHandle(), "camPos");

	//	glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));
	//	glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));
	//	glUniformMatrix4fv(SRT, 1, GL_FALSE, glm::value_ptr(prop.entitySRT));
	//	glUniform4fv(Albedo, 1, glm::value_ptr(prop.Albedo));
	//	glUniform4fv(Specular, 1, glm::value_ptr(prop.Specular));
	//	glUniform4fv(Diffuse, 1, glm::value_ptr(prop.Diffuse));
	//	glUniform4fv(Ambient, 1, glm::value_ptr(prop.entitySRT));
	//	glUniform1f(Shininess, prop.shininess);
	//	glUniform3fv(lightColor, 1, glm::value_ptr(LIGHTING.GetLight().lightColor));
	//	glUniform3fv(lightPos, 1, glm::value_ptr(LIGHTING.GetLight().lightpos));
	//	glUniform3fv(camPos, 1, glm::value_ptr(_camera.GetCameraPosition()));



	//	// POINT LIGHT STUFFS
	//	auto PointLight_Sources = LIGHTING.GetPointLights();
	//	for (int i = 0; i < PointLight_Sources.size(); ++i)
	//	{
	//		//pointLights.colour
	//		std::string point_color;
	//		point_color = "pointLights[" + std::to_string(i) + "].colour";

	//		glUniform3fv(glGetUniformLocation(shader.GetHandle(), point_color.c_str())
	//			, 1, glm::value_ptr(PointLight_Sources[i].lightColor));

	//		std::string point_pos;
	//		point_pos = "pointLights[" + std::to_string(i) + "].position";
	//		glUniform3fv(glGetUniformLocation(shader.GetHandle(), point_pos.c_str())
	//			, 1, glm::value_ptr(PointLight_Sources[i].lightpos));

	//		std::string point_intensity;
	//		point_intensity = "pointLights[" + std::to_string(i) + "].intensity";
	//		glUniform1fv(glGetUniformLocation(shader.GetHandle(), point_intensity.c_str())
	//			, 1, &PointLight_Sources[i].intensity);

	//	}
	//	// accepted all the way to line 635, pls take a look and merge it @kk
	//	GLint uniform7 = glGetUniformLocation(shader.GetHandle(), "PointLight_Count");
	//	glUniform1i(uniform7, (int)PointLight_Sources.size());

	//	// DIRECTIONAL LIGHT STUFFS
	//	auto DirectionLight_Sources = LIGHTING.GetDirectionLights();
	//	for (int i = 0; i < DirectionLight_Sources.size(); ++i)
	//	{
	//		//directionalLights.colour
	//		std::string directional_color;
	//		directional_color = "directionalLights[" + std::to_string(i) + "].colour";

	//		glUniform3fv(glGetUniformLocation(shader.GetHandle(), directional_color.c_str())
	//			, 1, glm::value_ptr(DirectionLight_Sources[i].lightColor));

	//		std::string directional_direction;
	//		directional_direction = "directionalLights[" + std::to_string(i) + "].direction";
	//		glUniform3fv(glGetUniformLocation(shader.GetHandle(), directional_direction.c_str())
	//			, 1, glm::value_ptr(DirectionLight_Sources[i].direction));

	//		std::string directional_intensity;
	//		directional_intensity = "directionalLights[" + std::to_string(i) + "].intensity";
	//		glUniform1fv(glGetUniformLocation(shader.GetHandle(), directional_intensity.c_str())
	//			, 1, &DirectionLight_Sources[i].intensity);
	//	}

	//	GLint uniform8 = glGetUniformLocation(shader.GetHandle(), "DirectionalLight_Count");
	//	glUniform1i(uniform8, (int)DirectionLight_Sources.size());

	//	// SPOTLIGHT STUFFS
	//	auto SpotLight_Sources = LIGHTING.GetSpotLights();
	//	for (int i = 0; i < SpotLight_Sources.size(); ++i)
	//	{

	//		//pointLights.position
	//		std::string spot_pos;
	//		spot_pos = "spotLights[" + std::to_string(i) + "].position";
	//		glUniform3fv(glGetUniformLocation(shader.GetHandle(), spot_pos.c_str())
	//			, 1, glm::value_ptr(SpotLight_Sources[i].lightpos));

	//		std::string spot_color;
	//		spot_color = "spotLights[" + std::to_string(i) + "].colour";

	//		glUniform3fv(glGetUniformLocation(shader.GetHandle(), spot_color.c_str())
	//			, 1, glm::value_ptr(SpotLight_Sources[i].lightColor));

	//		std::string spot_direction;
	//		spot_direction = "spotLights[" + std::to_string(i) + "].direction";
	//		glUniform3fv(glGetUniformLocation(shader.GetHandle(), spot_direction.c_str())
	//			, 1, glm::value_ptr(SpotLight_Sources[i].direction));

	//		std::string spot_cutoff_inner;
	//		spot_cutoff_inner = "spotLights[" + std::to_string(i) + "].innerCutOff";
	//		glUniform1fv(glGetUniformLocation(shader.GetHandle(), spot_cutoff_inner.c_str())
	//			, 1, &SpotLight_Sources[i].inner_CutOff);

	//		std::string spot_cutoff_outer;
	//		spot_cutoff_outer = "spotLights[" + std::to_string(i) + "].outerCutOff";
	//		glUniform1fv(glGetUniformLocation(shader.GetHandle(), spot_cutoff_outer.c_str())
	//			, 1, &SpotLight_Sources[i].outer_CutOff);

	//		std::string spot_intensity;
	//		spot_intensity = "spotLights[" + std::to_string(i) + "].intensity";
	//		glUniform1fv(glGetUniformLocation(shader.GetHandle(), spot_intensity.c_str())
	//			, 1, &SpotLight_Sources[i].intensity);
	//	}
	//	GLint uniform9 = glGetUniformLocation(shader.GetHandle(), "SpotLight_Count");
	//	glUniform1i(uniform9, (int)SpotLight_Sources.size());


	//	// SHADOW 
	//	GLint uniform10 =
	//		glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix_Directional");
	//	glUniformMatrix4fv(uniform10, 1, GL_FALSE,
	//		glm::value_ptr(lightSpaceMatrix_directional));
	//	GLint uniform11 =
	//		glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix_Spot");
	//	glUniformMatrix4fv(uniform11, 1, GL_FALSE,
	//		glm::value_ptr(lightSpaceMatrix_spot));



	//	glUniform1f(glGetUniformLocation(shader.GetHandle(), "farplane"), 1000.f);

	//	// SETTINGS
	//	glUniform1i(glGetUniformLocation(shader.GetHandle(), "hdr"), hdr);

	//	GLint uniform12 =
	//		glGetUniformLocation(shader.GetHandle(), "renderShadow");

	//	glUniform1f(uniform12, RENDERER.enableShadows());

	//	glUniform1f(glGetUniformLocation(shader.GetHandle(), "bloomThreshold"), bloomThreshold);

	// glUniform1f(glGetUniformLocation(shader.GetHandle(), "ambience_multiplier"), RENDERER.getAmbient());


	//	glBindVertexArray(prop.VAO);
	//	glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
	//	glBindVertexArray(0);

	//	shader.UnUse();/**/

	//	// forward render
	//	/*glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, prop.textureID);
	//	glActiveTexture(GL_TEXTURE1);
	//	glBindTexture(GL_TEXTURE_2D, prop.NormalID);*/


	//	/*GLSLShader& shader =  SHADER.GetShader(DEFAULT);
	//	shader.Use();

	//	GLint uProj =
	//		glGetUniformLocation(shader.GetHandle(), "persp_projection");
	//	GLint uView =
	//		glGetUniformLocation(shader.GetHandle(), "View");
	//	GLint SRT =
	//		glGetUniformLocation(shader.GetHandle(), "SRT");

	//	glUniformMatrix4fv(uProj, 1, GL_FALSE,
	//		glm::value_ptr(EditorCam.GetProjMatrix()));
	//	glUniformMatrix4fv(uView, 1, GL_FALSE,
	//		glm::value_ptr(EditorCam.GetViewMatrix()));
	//	glUniformMatrix4fv(SRT, 1, GL_FALSE,
	//		glm::value_ptr(prop.entitySRT));

	//	glBindVertexArray(prop.VAO);
	//	glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
	//	glBindVertexArray(0);

	//	shader.UnUse();*/
	//	
	//}
	
	
}

void Renderer::DrawMeshes(const GLuint& _vaoid, const unsigned int& _instanceCount,
	// const unsigned int& _primCount, GLenum _primType, const LightProperties& _lightSource, SHADERTYPE shaderType)
	const unsigned int& _primCount, GLenum _primType, const LightProperties& _lightSource, BaseCamera& _camera, SHADERTYPE shaderType)
{
	//testBox.instanceDraw(EntityRenderLimit);

	glEnable(GL_DEPTH_TEST); // might be sus to place this here

	GLSLShader& shader = SHADER.GetShader(shaderType);
	shader.Use();

	glActiveTexture(GL_TEXTURE0 + 31);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	glUniform1i(glGetUniformLocation(shader.GetHandle(), "PointShadowBox"), 31); // Associate samplerCube with texture unit 2
	
	
	glUniform1f(glGetUniformLocation(shader.GetHandle(), "farplane"), 1000.f);

	// UNIFORM VARIABLES ----------------------------------------
	// Persp Projection
	//GLint uniform1 =
	//	glGetUniformLocation(temp_instance_shader.GetHandle(), "persp_projection");
	//GLint uniform2 =
	//	glGetUniformLocation(temp_instance_shader.GetHandle(), "View");
	//GLint uniform3 =
	//	glGetUniformLocation(temp_instance_shader.GetHandle(), "lightColor");
	//GLint uniform4 =
	//	glGetUniformLocation(temp_instance_shader.GetHandle(), "lightPos");
	//GLint uniform5 =
	//	glGetUniformLocation(temp_instance_shader.GetHandle(), "camPos");
	GLint uniform1 =
		glGetUniformLocation(shader.GetHandle(), "persp_projection");
	GLint uniform2 =
		glGetUniformLocation(shader.GetHandle(), "View");
	//GLint uniform3 =
	//	glGetUniformLocation(shader.GetHandle(), "lightColor");
	//GLint uniform4 =
	//	glGetUniformLocation(shader.GetHandle(), "lightPos");
	GLint uniform5 =
		glGetUniformLocation(shader.GetHandle(), "camPos");
	GLint uniform6 =
		glGetUniformLocation(shader.GetHandle(), "hdr");

	glUniform1i(uniform6, hdr);

	// Scuffed SRT
	// srt not uniform
	/*GLint uniform3 =
		glGetUniformLocation(this->shader.GetHandle(), "SRT");*/

	glUniformMatrix4fv(uniform1, 1, GL_FALSE,
		glm::value_ptr(_camera.GetProjMatrix()));
	glUniformMatrix4fv(uniform2, 1, GL_FALSE,
		glm::value_ptr(_camera.GetViewMatrix()));
	//glUniform3fv(uniform3, 1,
	//	glm::value_ptr(_lightSource.lightColor));
	//glUniform3fv(uniform4, 1,
	//	glm::value_ptr(_lightSource.lightpos));
	glUniform3fv(uniform5, 1,
		glm::value_ptr(_camera.GetCameraPosition()));

	// POINT LIGHT STUFFS
	auto PointLight_Sources = LIGHTING.GetPointLights();
	for (int i = 0; i < PointLight_Sources.size(); ++i)
	{
		//pointLights.colour
		std::string point_color;
		point_color = "pointLights[" + std::to_string(i) + "].colour";

		glUniform3fv(glGetUniformLocation(shader.GetHandle(), point_color.c_str())
			, 1, glm::value_ptr(PointLight_Sources[i].lightColor));

		//pointLights.position
		std::string point_pos;
		point_pos = "pointLights[" + std::to_string(i) + "].position";
		glUniform3fv(glGetUniformLocation(shader.GetHandle(), point_pos.c_str())
			, 1, glm::value_ptr(PointLight_Sources[i].lightpos));

		//pointLights.intensity
		std::string point_intensity;
		point_intensity = "pointLights[" + std::to_string(i) + "].intensity";
		glUniform1fv(glGetUniformLocation(shader.GetHandle(), point_intensity.c_str())
			, 1, &PointLight_Sources[i].intensity);
	}

	GLint uniform7 =
		glGetUniformLocation(shader.GetHandle(), "PointLight_Count");
	glUniform1i(uniform7, (int)PointLight_Sources.size());

	// DIRECTIONAL LIGHT STUFFS
	auto DirectionLight_Sources = LIGHTING.GetDirectionLights();
	for (int i = 0; i < DirectionLight_Sources.size(); ++i)
	{
		//directionalLights.colour
		std::string directional_color;
		directional_color = "directionalLights[" + std::to_string(i) + "].colour";

		glUniform3fv(glGetUniformLocation(shader.GetHandle(), directional_color.c_str())
			, 1, glm::value_ptr(DirectionLight_Sources[i].lightColor));

		std::string directional_direction;
		directional_direction = "directionalLights[" + std::to_string(i) + "].direction";
		glUniform3fv(glGetUniformLocation(shader.GetHandle(), directional_direction.c_str())
			, 1, glm::value_ptr(DirectionLight_Sources[i].direction));

		std::string directional_intensity;
		directional_intensity = "directionalLights[" + std::to_string(i) + "].intensity";
		glUniform1fv(glGetUniformLocation(shader.GetHandle(), directional_intensity.c_str())
			, 1, &DirectionLight_Sources[i].intensity);
	}

	GLint uniform8 =
		glGetUniformLocation(shader.GetHandle(), "DirectionalLight_Count");
	glUniform1i(uniform8, (int)DirectionLight_Sources.size());

	// SPOTLIGHT STUFFS
	auto SpotLight_Sources = LIGHTING.GetSpotLights();
	for (int i = 0; i < SpotLight_Sources.size(); ++i)
	{

		//pointLights.position
		std::string spot_pos;
		spot_pos = "spotLights[" + std::to_string(i) + "].position";
		glUniform3fv(glGetUniformLocation(shader.GetHandle(), spot_pos.c_str())
			, 1, glm::value_ptr(SpotLight_Sources[i].lightpos));


		std::string spot_color;
		spot_color = "spotLights[" + std::to_string(i) + "].colour";

		glUniform3fv(glGetUniformLocation(shader.GetHandle(), spot_color.c_str())
			, 1, glm::value_ptr(SpotLight_Sources[i].lightColor));

		std::string spot_direction;
		spot_direction = "spotLights[" + std::to_string(i) + "].direction";
		glUniform3fv(glGetUniformLocation(shader.GetHandle(), spot_direction.c_str())
			, 1, glm::value_ptr(SpotLight_Sources[i].direction));

		std::string spot_cutoff_inner;
		spot_cutoff_inner = "spotLights[" + std::to_string(i) + "].innerCutOff";
		glUniform1fv(glGetUniformLocation(shader.GetHandle(), spot_cutoff_inner.c_str())
			, 1, &SpotLight_Sources[i].inner_CutOff);

		std::string spot_cutoff_outer;
		spot_cutoff_outer = "spotLights[" + std::to_string(i) + "].outerCutOff";
		glUniform1fv(glGetUniformLocation(shader.GetHandle(), spot_cutoff_outer.c_str())
			, 1, &SpotLight_Sources[i].outer_CutOff);


		std::string spot_intensity;
		spot_intensity = "spotLights[" + std::to_string(i) + "].intensity";
		glUniform1fv(glGetUniformLocation(shader.GetHandle(), spot_intensity.c_str())
			, 1, &SpotLight_Sources[i].intensity);

	}

	GLint uniform9 =
		glGetUniformLocation(shader.GetHandle(), "SpotLight_Count");
	glUniform1i(uniform9, (int)SpotLight_Sources.size());





	GLint uniform10 =
		glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix_Directional");

	glUniformMatrix4fv(uniform10, 1, GL_FALSE,
		glm::value_ptr(lightSpaceMatrix_directional));

	GLint uniform11 =
		glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix_Spot");

	glUniformMatrix4fv(uniform11, 1, GL_FALSE,
		glm::value_ptr(lightSpaceMatrix_spot));

	glUniform1f(glGetUniformLocation(shader.GetHandle(), "farplane"), 1000.f);

		glUniform1f(glGetUniformLocation(shader.GetHandle(), "bloomThreshold"), bloomThreshold);

	// Settings

	GLint uniform12 =
		glGetUniformLocation(shader.GetHandle(), "renderShadow");

	glUniform1f(uniform12, RENDERER.enableShadows());

	glUniform1f(glGetUniformLocation(shader.GetHandle(), "ambience_multiplier"), RENDERER.getAmbient());



	glBindVertexArray(_vaoid);
	glDrawElementsInstanced(_primType, _primCount, GL_UNSIGNED_INT, 0, _instanceCount);
	glBindVertexArray(0);

	shader.UnUse();
}


void Renderer::UIDraw_2D(BaseCamera& _camera)
{
	// Setups required for all UI
 	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glm::mat4 OrthoProjection = glm::ortho(-800.f, 800.f, -450.f, 450.f, 0.001f, 10.f);
	//glm::mat4 OrthoProjection = glm::ortho(0.f, 16.f, 0.f, 9.f, -10.f, 10.f);
	//glm::mat4 OrthoProjection = glm::ortho(-8.f, 8.f, -4.5f, 4.5f, -10.f, 10.f);
	glm::mat4 OrthoProjection = glm::ortho(-1.f, 1.f, -1.f, 1.f, -10.f, 10.f);

	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UI_SCREEN);
	shader.Use();

	// Setting the projection here since all of them use the same projection

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(OrthoProjection));


	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
	{
		// This means it's 3D space
		if (Sprite.WorldSpace)
		{
			continue;
		}

		// Declarations for the things we need - SRT
		Entity& entity = currentScene.Get<Entity>(Sprite);
		Transform& transform = currentScene.Get<Transform>(entity);

		// SRT uniform
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(transform.GetLocalMatrix()));

		// Setting bool to see if there is a sprite to render
		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "RenderSprite");
		GLuint spriteTextureID = TextureManager.GetTexture(Sprite.SpriteTexture);
		if (Sprite.SpriteTexture == DEFAULT_ASSETS["None.dds"])
		{
			glUniform1f(uniform1, false);
		}
		else
		{
			glUniform1f(uniform1, true);
		}
		
		// Binding Texture - might be empty , above uniform will sort it
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spriteTextureID);
		

		renderQuad(Renderer_quadVAO, Renderer_quadVBO);

	}
	shader.UnUse();
	glDisable(GL_BLEND);

}

void Renderer::UIDraw_3D(BaseCamera& _camera)
{
	// Setups required for all UI
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UI_WORLD);
	shader.Use();

	// Setting the projection here since all of them use the same projection

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));
	
	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "view"),
		1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));

	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
	{
		// This means it's 2D space
		if (!Sprite.WorldSpace)
		{
			continue;
		}

		// Declarations for the things we need - SRT
		Entity& entity = currentScene.Get<Entity>(Sprite);
		Transform& transform = currentScene.Get<Transform>(entity);

		// SRT uniform
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix()));

		// Setting bool to see if there is a sprite to render
		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "RenderSprite");
		GLuint spriteTextureID = TextureManager.GetTexture(Sprite.SpriteTexture);
		if (Sprite.SpriteTexture == DEFAULT_ASSETS["None.dds"])
		{
			glUniform1f(uniform1, false);
		}
		else
		{
			glUniform1f(uniform1, true);
		}

		// Binding Texture - might be empty , above uniform will sort it
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spriteTextureID);

		renderQuad(Renderer_quadVAO, Renderer_quadVBO);
	}
	shader.UnUse();
	glDisable(GL_BLEND);

}

void Renderer::UIDraw_2DWorldSpace(BaseCamera& _camera)
{
	// Setups required for all UI
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UI_WORLD);
	shader.Use();

	// Setting the projection here since all of them use the same projection
	
	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "view"),
		1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));

	
	glUniform1f(glGetUniformLocation(shader.GetHandle(), "RenderSprite"), false);

	for (Canvas& currCanvas : currentScene.GetArray<Canvas>())
	{
		Entity& entity = currentScene.Get<Entity>(currCanvas);
		Transform& transform = currentScene.Get<Transform>(entity);

		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix()));

		glLineWidth(20.f);
		renderQuadWireMesh(Renderer_quadVAO_WM, Renderer_quadVBO_WM);
	}
	glLineWidth(1.f);



	// Setting the projection here since all of them use the same projection

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
		1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));

	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "view"),
		1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));

	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
	{
		// This means it's 2D space
		if (Sprite.WorldSpace)
		{
			continue;
		}

		// Declarations for the things we need - SRT
		Entity& entity = currentScene.Get<Entity>(Sprite);
		Transform& transform = currentScene.Get<Transform>(entity);

		// SRT uniform
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
			1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix()));

		// Setting bool to see if there is a sprite to render
		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "RenderSprite");
		GLuint spriteTextureID = TextureManager.GetTexture(Sprite.SpriteTexture);
		if (Sprite.SpriteTexture == DEFAULT_ASSETS["None.dds"])
		{
			glUniform1f(uniform1, false);
		}
		else
		{
			glUniform1f(uniform1, true);
		}

		// Binding Texture - might be empty , above uniform will sort it
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spriteTextureID);

		renderQuad(Renderer_quadVAO, Renderer_quadVBO);
	}
	shader.UnUse();
	glDisable(GL_BLEND);

}



void Renderer::DrawGrid(const GLuint& _vaoid, const unsigned int& _instanceCount)
{
	glm::vec3 color{ 1.f, 1.f, 1.f };

	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::TDR);
	shader.Use();
	// UNIFORM VARIABLES ----------------------------------------
	// Persp Projection
	GLint uniform1 =
		glGetUniformLocation(shader.GetHandle(), "persp_projection");
	GLint uniform2 =
		glGetUniformLocation(shader.GetHandle(), "View");
	GLint uniform3 =
		glGetUniformLocation(shader.GetHandle(), "uColor");
	glUniformMatrix4fv(uniform1, 1, GL_FALSE,
		glm::value_ptr(EditorCam.GetProjMatrix()));
	glUniformMatrix4fv(uniform2, 1, GL_FALSE,
		glm::value_ptr(EditorCam.GetViewMatrix()));
	glUniform3fv(uniform3, 1, glm::value_ptr(color));

	glBindVertexArray(_vaoid);
	//glDrawElements(GL_LINES, 2 * 12, GL_UNSIGNED_INT, 0);
	glDrawElementsInstanced(GL_LINES, 2, GL_UNSIGNED_INT, 0, _instanceCount);

	// unbind and free stuff
	glBindVertexArray(0);
	shader.UnUse();
}

void Renderer::DrawDebug(const GLuint& _vaoid, const unsigned int& _instanceCount)
{
	glm::vec3 color{ 1.f, 0.f, 1.f };

	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::TDR);
	shader.Use();
	// UNIFORM VARIABLES ----------------------------------------
	// Persp Projection
	GLint uniform1 =
		glGetUniformLocation(shader.GetHandle(), "persp_projection");
	GLint uniform2 =
		glGetUniformLocation(shader.GetHandle(), "View");
	GLint uniform3 =
		glGetUniformLocation(shader.GetHandle(), "uColor");
	glUniformMatrix4fv(uniform1, 1, GL_FALSE,
		glm::value_ptr(EditorCam.GetProjMatrix()));
	glUniformMatrix4fv(uniform2, 1, GL_FALSE,
		glm::value_ptr(EditorCam.GetViewMatrix()));
	glUniform3fv(uniform3, 1, glm::value_ptr(color));

	glBindVertexArray(_vaoid);
	//glDrawElements(GL_LINES, 2 * 12, GL_UNSIGNED_INT, 0);
	glDrawElementsInstanced(GL_LINES, 2 * 12, GL_UNSIGNED_INT, 0, _instanceCount);

	// unbind and free stuff
	glBindVertexArray(0);
	shader.UnUse();
}

//void Renderer::DrawDepth(LIGHT_TYPE temporary_test)
//{
//	glEnable(GL_DEPTH_TEST);
//	//glm::vec3 lightPos(-2.0f, 4.0f, -1.0f); // This suppouse to be the actual light direction
//	glm::vec3 lightPos(-0.2f, -1.0f, -0.3f); // This suppouse to be the actual light direction
//	lightPos = -lightPos;
//	glm::mat4 lightProjection, lightView;
//	float near_plane = -100.f, far_plane = 100.f;
//
//	// Above is directional light and spot light related stuffs
//	// this vector is for point light
//	std::vector<glm::mat4> shadowTransforms;
//	
//	//if (temporary_test == DIRECTIONAL_LIGHT)
//	//{
//		// Good Light pos {-0.2f, -1.0f, -0.3f}
//		lightProjection = glm::ortho(-50.f, 50.f, -50.f, 50.f, near_plane, far_plane);
//		//lightView = glm::lookAt(-directional_light_stuffs.direction + EditorCam.GetCameraPosition(), EditorCam.GetCameraPosition(), glm::vec3(0.0, 1.0, 0.0));
//		lightView = glm::lookAt(-directional_light_stuffs.direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
//
//	//}
//
//
//	//else if (temporary_test == SPOT_LIGHT)	
//	//{
//		lightProjection = glm::perspective<float>(glm::radians(90.f), 1.f, 0.1f, 100.f);
//		lightView = glm::lookAt(spot_light_stuffs.lightpos, spot_light_stuffs.lightpos + 
//			(spot_light_stuffs.direction * 100.f), glm::vec3(0.0, 0.0, 1.0));
//	//}
//
//	//else if (temporary_test == POINT_LIGHT)
//	//{
//		near_plane = 0.001f;
//		far_plane = 1000.f;
//		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
//		shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//	//}
//
//	lightSpaceMatrix = lightProjection * lightView;
//	
//
//	//glEnable(GL_CULL_FACE);
//	//glCullFace(GL_FRONT);
//
//
//	if (temporary_test == POINT_LIGHT)
//	{
//		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//		glBindFramebuffer(GL_FRAMEBUFFER, depthCubemapFBO);
//		glClear(GL_DEPTH_BUFFER_BIT);
//	}
//	else
//	{
//		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//		glClear(GL_DEPTH_BUFFER_BIT);
//	}
//
//	/*for (auto& [str, vao] : MeshManager.vaoMap) {
//		std::cout << "NAME: " << str << " vao: " << vao << "\n";
//	}*/
//	//std::cout << std::endl;
//	for (int s = 0; s < static_cast<int>(SHADERTYPE::COUNT); ++s)
//	{
//		for (auto& [vao, prop] : instanceContainers[s]) {
//			//for (auto& [name, prop] : instanceProperties) // @kk check if need use the container with shader anot
//			//{
//			
//			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
//			glBufferSubData(GL_ARRAY_BUFFER, 0, (EnitityInstanceLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
//			glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//			if (temporary_test == POINT_LIGHT)
//			{
//				//glClear(GL_DEPTH_BUFFER_BIT);
//
//				GLSLShader& shader = SHADER.GetShader(SHADERTYPE::POINTSHADOW);
//				shader.Use();
//				/*for (int i = 0; i < 6; ++i)
//				{
//					std::string spot_pos;
//					spot_pos = "shadowMatrices[" + std::to_string(i) + "]";
//					glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), spot_pos.c_str())
//						, 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
//				}*/
//
//				glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "shadowMatrices")
//					, 6, GL_FALSE, glm::value_ptr(shadowTransforms[0]));
//
//				GLint uniform1 =
//					glGetUniformLocation(shader.GetHandle(), "lightPos");
//				glUniform3fv(uniform1, 1,
//					glm::value_ptr(point_light_stuffs.lightpos));
//
//				GLint uniform2 =
//					glGetUniformLocation(shader.GetHandle(), "far_plane");
//				glUniform1f(uniform2, far_plane);
//
//
//				glBindVertexArray(prop.VAO);
//				glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);
//				glBindVertexArray(0);
//
//				shader.UnUse();
//
//			}
//			else
//			{
//				GLSLShader& shader = SHADER.GetShader(SHADERTYPE::SHADOW);
//				shader.Use();
//
//				GLint uniform1 =
//					glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix");
//
//				glUniformMatrix4fv(uniform1, 1, GL_FALSE,
//					glm::value_ptr(lightSpaceMatrix));
//
//				glBindVertexArray(prop.VAO);
//				glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);
//				glBindVertexArray(0);
//
//				shader.UnUse();
//
//			}
//		}
//	}
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	//glDisable(GL_CULL_FACE);
//
//	// reset viewport
//	//glViewport(0, 0, 1600, 900);// screen width and screen height
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//}


void Renderer::DrawDepthDirectional()
{
	glEnable(GL_DEPTH_TEST);
	//glm::vec3 lightPos(-0.2f, -1.0f, -0.3f); // This suppouse to be the actual light direction
	glm::mat4 lightProjection, lightView;
	float near_plane = -100.f, far_plane = 100.f;


	lightProjection = glm::ortho(-50.f, 50.f, -50.f, 50.f, near_plane, far_plane);
	//lightView = glm::lookAt(-directional_light_stuffs.direction + EditorCam.GetCameraPosition(), EditorCam.GetCameraPosition(), glm::vec3(0.0, 1.0, 0.0));
	lightView = glm::lookAt(-directional_light_stuffs.direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

	lightSpaceMatrix_directional = lightProjection * lightView;


	glViewport(0, 0, SHADOW_WIDTH_DIRECTIONAL, SHADOW_HEIGHT_DIRECTIONAL);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::SHADOW);
	shader.Use();
	for (int s = 0; s < static_cast<int>(SHADERTYPE::COUNT); ++s)
	{
		for (auto& [vao, prop] : instanceContainers[s]) {
			//for (auto& [name, prop] : instanceProperties) // @kk check if need use the container with shader anot
			//{

			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (EnitityInstanceLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			

			GLint uniform1 =
				glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix");

			glUniformMatrix4fv(uniform1, 1, GL_FALSE,
				glm::value_ptr(lightSpaceMatrix_directional));

			glBindVertexArray(prop.VAO);
			glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);
			glBindVertexArray(0);


			
		}
	}

	shader.UnUse();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void Renderer::DrawDepthSpot()
{
	glEnable(GL_DEPTH_TEST);
	glm::mat4 lightProjection, lightView;

	lightProjection = glm::perspective<float>(glm::radians(90.f), 1.f, 0.1f, 100.f);
	lightView = glm::lookAt(spot_light_stuffs.lightpos, spot_light_stuffs.lightpos +
		(spot_light_stuffs.direction * 100.f), glm::vec3(0.0, 0.0, 1.0));

	lightSpaceMatrix_spot = lightProjection * lightView;

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_S);
	glClear(GL_DEPTH_BUFFER_BIT);

	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::SHADOW);
	shader.Use();
	for (int s = 0; s < static_cast<int>(SHADERTYPE::COUNT); ++s)
	{
		for (auto& [vao, prop] : instanceContainers[s]) {
			//for (auto& [name, prop] : instanceProperties) // @kk check if need use the container with shader anot
			//{

			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (EnitityInstanceLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);



			GLint uniform1 =
				glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix");

			glUniformMatrix4fv(uniform1, 1, GL_FALSE,
				glm::value_ptr(lightSpaceMatrix_spot));

			glBindVertexArray(prop.VAO);
			glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);
			glBindVertexArray(0);



		}
	}

	shader.UnUse();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




}

void Renderer::DrawDepthPoint()
{
	float near_plane = 0.001f, far_plane = 1000.f;

	std::vector<glm::mat4> shadowTransforms;

	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
	shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_stuffs.lightpos, point_light_stuffs.lightpos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthCubemapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::POINTSHADOW);
	shader.Use();



	for (int s = 0; s < static_cast<int>(SHADERTYPE::COUNT); ++s)
	{
		for (auto& [vao, prop] : instanceContainers[s]) {
			//for (auto& [name, prop] : instanceProperties) // @kk check if need use the container with shader anot
			//{

			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (EnitityInstanceLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);


			/*for (int i = 0; i < 6; ++i)
			{
				std::string spot_pos;
				spot_pos = "shadowMatrices[" + std::to_string(i) + "]";
				glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), spot_pos.c_str())
					, 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
			}*/

			glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "shadowMatrices")
				, 6, GL_FALSE, glm::value_ptr(shadowTransforms[0]));

			GLint uniform1 =
				glGetUniformLocation(shader.GetHandle(), "lightPos");
			glUniform3fv(uniform1, 1,
				glm::value_ptr(point_light_stuffs.lightpos));

			GLint uniform2 =
				glGetUniformLocation(shader.GetHandle(), "far_plane");
			glUniform1f(uniform2, far_plane);


			glBindVertexArray(prop.VAO);
			glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);
			glBindVertexArray(0);


			
		}
	}

	shader.UnUse();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


}


bool Renderer::Culling()
{
	return false;
}

void Renderer::Forward()
{

}

void Renderer::Deferred()
{

}

unsigned int Renderer::ReturnTextureIdx(InstanceProperties& prop, const GLuint& _id)
{
	if (!_id)
	{
		return 33;
	}
	for (unsigned int iter = 0; iter < prop.textureCount + 1; ++iter)
	{
		if (prop.texture[iter] == 0)
		{
			prop.texture[iter] = _id;
			prop.textureCount++;
			return iter;
		}
		if (prop.texture[iter] == _id)
		{
			prop.textureCount++;
			return iter;
		}
	}
	return 33;
}

void Renderer::Exit()
{

}