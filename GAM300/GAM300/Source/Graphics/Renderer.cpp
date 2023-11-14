/*!***************************************************************************************
\file			Renderer.cpp
\project
\author			Sean Ngo, Jake Lian, Euan Lim

\par			Course: GAM300
\date           11/10/2023

\brief
	This file contains the definitions of Graphics Renderer that includes:
	1. Render UI, Meshs, Depth Draw calls

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Scene/SceneManager.h"
#include "MESHMANAGER.h"
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


#include "GBuffer.h"
unsigned int Renderer_quadVAO = 0;
unsigned int Renderer_quadVBO = 0;

unsigned int Renderer_quadVAO_WM = 0;
unsigned int Renderer_quadVBO_WM = 0;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
const unsigned int SHADOW_WIDTH_DIRECTIONAL = 4096, SHADOW_HEIGHT_DIRECTIONAL = 4096;

void Renderer::Init()
{
	//instanceContainers.resize(static_cast<size_t>(SHADERTYPE::COUNT));
	m_gBuffer.Init(1600, 900);
	
	FRAMEBUFFER.CreateDirectionalAndSpotLight(depthMapFBO, depthMap, SHADOW_WIDTH_DIRECTIONAL, SHADOW_HEIGHT_DIRECTIONAL);

	FRAMEBUFFER.CreateDirectionalAndSpotLight(depthMapFBO_S, depthMap_S, SHADOW_WIDTH, SHADOW_HEIGHT);

	FRAMEBUFFER.CreatePointLight(depthCubemapFBO, depthCubemap, SHADOW_WIDTH, SHADOW_HEIGHT);

}

void Renderer::Update(float)
{
	// maybe no need clear every time?
	/*for (InstanceContainer& container : instanceContainers) {
		for (auto& [vao, prop] : container)
		{
			std::fill_n(prop.textureIndex, EnitityInstanceLimit, glm::vec2(0.f));
			std::fill_n(prop.M_R_A_Texture, EnitityInstanceLimit, glm::vec4(33.f));
			std::fill_n(prop.texture, 32, 0);
		}
	}*/
	instanceContainers.clear(); // clear then emplace back? coz spcific vao in specific shader?
	instanceContainers.resize(static_cast<size_t>(SHADERTYPE::COUNT));
	defaultProperties.clear(); // maybe no need clear everytime, see steve rabin code?
	finalBoneMatContainer.clear();
#ifndef _BUILD
	SetupGrid(100);
#endif
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();

	int i = 0;

	for (Animator& animator : currentScene.GetArray<Animator>())
	{
		if (animator.AnimationAttached() /*&& animator.m_FinalBoneMatIdx == -1*/)
		{
			//renderProperties.isAnimatable = true; // can remove later, should be in mesh instead
			animator.m_FinalBoneMatIdx = (int)finalBoneMatContainer.size();
			finalBoneMatContainer.push_back(&animator.m_FinalBoneMatrices);
			//if ((int)finalBoneMatContainer.size() == 0) // just incase
			//	animator.m_FinalBoneMatIdx = -1;
		}
	}

	for (MeshRenderer& renderer : currentScene.GetArray<MeshRenderer>())
	{
		// No material instance, then just go next
		if (renderer.state == DELETED) continue;

		if (!currentScene.IsActive(renderer)) continue;

		if (MESHMANAGER.vaoMap.find(renderer.meshID) == MESHMANAGER.vaoMap.end()) continue; // no vao? go next
	
		Material_instance currMatInstance = MaterialSystem::Instance().getMaterialInstance(renderer.materialGUID);

		Entity& entity = currentScene.Get<Entity>(renderer);
		if (!currentScene.IsActive(entity)) continue;

		Transform& transform = currentScene.Get<Transform>(entity);

		if (currMatInstance.shaderType == (int)SHADERTYPE::DEFAULT)
		{

			Mesh* t_Mesh = MESHMANAGER.DereferencingMesh(renderer.meshID);
			if (t_Mesh == nullptr)
			{
				continue;
			}/**/
			
			GLuint vao = MESHMANAGER.vaoMap[renderer.meshID];

			DefaultRenderProperties renderProperties;
			renderProperties.VAO = vao;

			renderProperties.metallic = currMatInstance.metallicConstant;
			renderProperties.roughness = currMatInstance.roughnessConstant;
			renderProperties.ao = currMatInstance.aoConstant;
			renderProperties.emission = currMatInstance.emissionConstant;

			renderProperties.entitySRT = transform.GetWorldMatrix();
			renderProperties.Albedo = currMatInstance.albedoColour;

			renderProperties.RoughnessID = TextureManager.GetTexture(currMatInstance.roughnessTexture);
			renderProperties.MetallicID = TextureManager.GetTexture(currMatInstance.metallicTexture);
			renderProperties.AoID = TextureManager.GetTexture(currMatInstance.aoTexture);
			renderProperties.EmissionID = TextureManager.GetTexture(currMatInstance.emissionTexture);

			renderProperties.textureID = TextureManager.GetTexture(currMatInstance.albedoTexture);
			renderProperties.NormalID = TextureManager.GetTexture(currMatInstance.normalMap);

			renderProperties.drawType = t_Mesh->prim;
			renderProperties.drawCount = t_Mesh->drawCounts;

			renderProperties.isAnimatable = false;
			renderProperties.boneidx = -1;

			if (transform.parent) /*if (meshIsanimatable)*/ // if mesh can be animated + there is an animaator in its parent
			{
				Entity& parententity = currentScene.Get<Entity>(transform.parent);
				if (currentScene.Has<Animator>(parententity))
				{
					Animator& animator = currentScene.Get<Animator>(parententity);

					if (animator.AnimationAttached() && animator.m_FinalBoneMatIdx != -1)
					{
						renderProperties.isAnimatable = true; // can remove later, should be in mesh instead
						renderProperties.boneidx = animator.m_FinalBoneMatIdx; // i need to figure out how to access the parent's renderProperties....
					}
				}
			}

			// Debug Draw, consult team / UX on thursday
			if (instanceContainers[static_cast<int>(SHADERTYPE::TDR)].find(vao) == instanceContainers[static_cast<int>(SHADERTYPE::TDR)].cend()) { // if container does not have this vao, emplace
				instanceContainers[static_cast<int>(SHADERTYPE::TDR)].emplace(std::pair(vao, instanceProperties[vao]));
			}
			instanceContainers[static_cast<int>(SHADERTYPE::TDR)][vao].entitySRT.emplace_back(transform.GetWorldMatrix());
			instanceContainers[static_cast<int>(SHADERTYPE::TDR)][vao].iter++;
			defaultProperties.emplace_back(renderProperties);
			++i;
			continue;
		} // END NON-INSTANCED RENDERING CONDITION

		//if (currMatInstance.shaderType == (int)SHADERTYPE::PBR)
		size_t s = static_cast<size_t>(SHADERTYPE::PBR);
		GLuint vao = MESHMANAGER.vaoMap[renderer.meshID];

		if (instanceContainers[s].find(vao) == instanceContainers[s].cend()) { // if container does not have this vao, emplace
			instanceContainers[s].emplace(std::pair(vao, instanceProperties[vao]));
		}

		float texidx, normidx, metalidx, roughidx, aoidx, emissionidx, metal_constant, rough_constant, ao_constant, emission_constant;
		unsigned int& iter = instanceContainers[s][vao].iter;

		metal_constant = currMatInstance.metallicConstant;
		rough_constant = currMatInstance.roughnessConstant;
		ao_constant = currMatInstance.aoConstant;
		emission_constant = currMatInstance.emissionConstant;
		instanceContainers[s][vao].Albedo.emplace_back(currMatInstance.albedoColour);

		texidx = float(ReturnTextureIdx(instanceContainers[s][vao], TextureManager.GetTexture(currMatInstance.albedoTexture)));
		normidx = float(ReturnTextureIdx(instanceContainers[s][vao], TextureManager.GetTexture(currMatInstance.normalMap)));

		metalidx = float(ReturnTextureIdx(instanceContainers[s][vao], TextureManager.GetTexture(currMatInstance.metallicTexture)));
		roughidx = float(ReturnTextureIdx(instanceContainers[s][vao], TextureManager.GetTexture(currMatInstance.roughnessTexture)));
		aoidx = float(ReturnTextureIdx(instanceContainers[s][vao], TextureManager.GetTexture(currMatInstance.aoTexture)));
		emissionidx = float(ReturnTextureIdx(instanceContainers[s][vao], TextureManager.GetTexture(currMatInstance.emissionTexture)));

		instanceContainers[s][vao].M_R_A_Constant.emplace_back(glm::vec4(metal_constant, rough_constant, ao_constant, emission_constant));
		instanceContainers[s][vao].M_R_A_Texture.emplace_back(glm::vec4(metalidx, roughidx, aoidx, emissionidx));
		instanceContainers[s][vao].textureIndex.emplace_back(glm::vec2(texidx, normidx));
		instanceContainers[s][vao].entitySRT.emplace_back(transform.GetWorldMatrix());
		++iter;
		++i;
	} // END MESHRENDERER LOOP
	
	if (RENDERER.enableShadows())
	{
		DrawDepthSpot();
		
		DrawDepthDirectional();
		
		DrawDepthPoint();

	}
}

void Renderer::SetupGrid(const int& _num)
{

	float spacing = 1.f;
	float length = _num * spacing * 0.5f;

	//instanceProperties["Line"].iter = _num * 2;
	GLuint vao = MESHMANAGER.vaoMap[DEFAULT_ASSETS["Line.geom"]];
	size_t s = static_cast<int>(SHADERTYPE::TDR);
	if (instanceContainers[s].find(vao) == instanceContainers[s].cend()) { // if container does not have this vao, emplace
		instanceContainers[s].emplace(std::pair(vao, instanceProperties[vao]));
	}
	
	for (int i = 0; i < _num; i++)
	{
		glm::mat4 scalMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(length));
		glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 transMatrixZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.0f, (i * spacing) - length));
		glm::mat4 transMatrixX = glm::translate(glm::mat4(1.0f), glm::vec3((i * spacing) - length, 0.0f, 0.f));

		instanceContainers[s][vao].entitySRT.emplace_back(transMatrixZ * scalMatrix);// [i] = transMatrixZ * scalMatrix; // z axis
		instanceContainers[s][vao].entitySRT.emplace_back(transMatrixX * rotMatrix * scalMatrix);//[i + _num] = transMatrixX * rotMatrix * scalMatrix; // x axis
	}
}

void Renderer::Draw(BaseCamera& _camera) {

	// Instanced Rendering
	for (auto& [vao, prop] : instanceContainers[static_cast<size_t>(SHADERTYPE::PBR)]) {
		size_t buffersize = prop.iter;
		glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (buffersize) * sizeof(glm::mat4), prop.entitySRT.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, prop.AlbedoBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, buffersize * sizeof(glm::vec4), prop.Albedo.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, prop.Metal_Rough_AO_Texture_Buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, buffersize * sizeof(glm::vec4), prop.M_R_A_Texture.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, prop.Metal_Rough_AO_Texture_Constant);
		glBufferSubData(GL_ARRAY_BUFFER, 0, buffersize * sizeof(glm::vec4), prop.M_R_A_Constant.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, prop.textureIndexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, buffersize * sizeof(glm::vec2), prop.textureIndex.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// if debug line draw
#ifndef _BUILD
		if (_camera.GetCameraType() == CAMERATYPE::SCENE) {
			// FOR DEBUG DRAW
			if (EditorScene::Instance().DebugDraw() && prop.debugVAO)
				DrawDebug(prop.debugVAO, (unsigned)prop.entitySRT.size());
		}
#endif
		GLSLShader& shader = SHADER.GetShader(SHADERTYPE::PBR);
		shader.Use();
		for (int i = 0; i < 10; ++i) // this should be up till 10 for now... hehe
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, prop.texture[i]);
			glUniform1i(glGetUniformLocation(shader.GetHandle(), ("myTextureSampler[" + std::to_string(i) + "]").c_str()), i);

		}
		// min is 0
		// max is 31

		
		

		DrawMeshes(vao, prop.iter, prop.drawCount, prop.drawType, LIGHTING.GetLight(), _camera, SHADERTYPE::PBR);
	}

#ifndef _BUILD
	//Draw Debug grids
	for (auto& [vao, prop] : instanceContainers[static_cast<size_t>(SHADERTYPE::TDR)]) 
	{
		size_t buffersize = prop.iter;
		glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (buffersize) * sizeof(glm::mat4), prop.entitySRT.data());
		DrawGrid(vao, (unsigned)prop.entitySRT.size());
	}
#endif

	//Non-Instanced Rendering
	for (DefaultRenderProperties& prop : defaultProperties) 
	{
		GLSLShader& shader = SHADER.GetShader(SHADERTYPE::DEFAULT);
		shader.Use();

		glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, prop.textureID);
		glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, prop.NormalID);
		glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, prop.RoughnessID);
		glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, prop.MetallicID);
		glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, prop.AoID);
		glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_2D, prop.EmissionID);
		// You have 6 - 9 Texture Slots

		//glActiveTexture(GL_TEXTURE6); glBindTexture(GL_TEXTURE_2D, LIGHTING.GetDirectionLights()[0].shadow);
		//
		//glActiveTexture(GL_TEXTURE7); glBindTexture(GL_TEXTURE_2D, LIGHTING.GetSpotLights()[10].shadow);
		//glActiveTexture(GL_TEXTURE8); glBindTexture(GL_TEXTURE_CUBE_MAP, LIGHTING.GetPointLights()[0].shadow);

		for (int i = 0; i < LIGHTING.spotLightCount; ++i)
		{
			int textureUnit = 10 + i;
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, LIGHTING.GetSpotLights()[i].shadow);
			glUniform1i(glGetUniformLocation(shader.GetHandle(), ("ShadowMap_SpotDirectional[" + std::to_string(i) + "]").c_str()), textureUnit);

		}

		for (int i = 0; i < LIGHTING.directionalLightCount; ++i)
		{
			int textureUnit = 20 + i;
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, LIGHTING.GetDirectionLights()[i].shadow);
			glUniform1i(glGetUniformLocation(shader.GetHandle(), ("ShadowMap_SpotDirectional[" + std::to_string(10+i) + "]").c_str()), textureUnit);

		}

		for (int i = 0; i < LIGHTING.pointLightCount; ++i)
		{
			int textureUnit = 22 + i;
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_CUBE_MAP, LIGHTING.GetPointLights()[i].shadow);
			glUniform1i(glGetUniformLocation(shader.GetHandle(), ("PointShadows[" + std::to_string(i) + "]").c_str()), textureUnit); // Associate samplerCube with texture unit 2
		}

		// PBR TEXTURES
		GLint hasTexture = glGetUniformLocation(shader.GetHandle(), "hasTexture");
		GLint hasNormal = glGetUniformLocation(shader.GetHandle(), "hassNormal");
		GLint hasRoughness = glGetUniformLocation(shader.GetHandle(), "hasRoughness");
		GLint hasMetallic = glGetUniformLocation(shader.GetHandle(), "hasMetallic");
		GLint hasAO = glGetUniformLocation(shader.GetHandle(), "hasAO");
		GLint hasEmission = glGetUniformLocation(shader.GetHandle(), "hasEmission");

		glUniform1i(hasTexture, prop.textureID);
		glUniform1i(hasNormal, prop.NormalID);
		glUniform1i(hasRoughness, prop.RoughnessID);
		glUniform1i(hasMetallic, prop.MetallicID);
		glUniform1i(hasAO, prop.AoID);
		glUniform1i(hasEmission, prop.EmissionID);

		// PBR CONSTANT VALUES
		glUniform1f(glGetUniformLocation(shader.GetHandle(), "MetalConstant"), prop.metallic);
		glUniform1f(glGetUniformLocation(shader.GetHandle(), "RoughnessConstant"), prop.roughness);
		glUniform1f(glGetUniformLocation(shader.GetHandle(), "AoConstant"), prop.ao);
		glUniform1f(glGetUniformLocation(shader.GetHandle(), "EmissionConstant"), prop.emission);

		// Shadow
		glUniform1i(glGetUniformLocation(shader.GetHandle(), "ShadowCubeMap"), 8);


		GLint uProj = glGetUniformLocation(shader.GetHandle(), "persp_projection");
		GLint uView = glGetUniformLocation(shader.GetHandle(), "View");
		GLint SRT = glGetUniformLocation(shader.GetHandle(), "SRT");
		GLint Albedo = glGetUniformLocation(shader.GetHandle(), "Albedo");
		GLint Specular = glGetUniformLocation(shader.GetHandle(), "Specular");
		GLint Diffuse = glGetUniformLocation(shader.GetHandle(), "Diffuse");
		GLint Ambient = glGetUniformLocation(shader.GetHandle(), "Ambient");
		GLint Shininess = glGetUniformLocation(shader.GetHandle(), "Shininess");
		//GLint lightColor = glGetUniformLocation(shader.GetHandle(), "lightColor");
		//GLint lightPos = glGetUniformLocation(shader.GetHandle(), "lightPos");
		GLint camPos = glGetUniformLocation(shader.GetHandle(), "camPos");
		GLint hasAnim = glGetUniformLocation(shader.GetHandle(), "isAnim");

		glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));
		glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));
		glUniformMatrix4fv(SRT, 1, GL_FALSE, glm::value_ptr(prop.entitySRT));
		glUniform4fv(Albedo, 1, glm::value_ptr(prop.Albedo));
		glUniform4fv(Specular, 1, glm::value_ptr(prop.Specular));
		glUniform4fv(Diffuse, 1, glm::value_ptr(prop.Diffuse));
		glUniform4fv(Ambient, 1, glm::value_ptr(prop.entitySRT));
		glUniform1f(Shininess, prop.shininess);
		glUniform3fv(camPos, 1, glm::value_ptr(_camera.GetCameraPosition()));

		BindLights(shader);
		glUniform1i(glGetUniformLocation(shader.GetHandle(), "hdr"), hdr);

		// ANIMATONS
		glUniform1i(glGetUniformLocation(shader.GetHandle(), "isAnim"), prop.isAnimatable);
		if (prop.isAnimatable)
		{
			std::vector<glm::mat4> transforms = *finalBoneMatContainer[prop.boneidx];
			GLint uniform13 = glGetUniformLocation(shader.GetHandle(), "finalBonesMatrices");
			glUniformMatrix4fv(uniform13, (GLsizei)transforms.size(), GL_FALSE, glm::value_ptr(transforms[0]));
		}

		glBindVertexArray(prop.VAO);
		glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		shader.UnUse();
	}
}


void Renderer::DrawMeshes(const GLuint& _vaoid, const unsigned int& _instanceCount,
	const unsigned int& _primCount, GLenum _primType, const LightProperties& _lightSource, BaseCamera& _camera, SHADERTYPE shaderType)
{
	//testBox.instanceDraw(EntityRenderLimit);

	glEnable(GL_DEPTH_TEST); // might be sus to place this here

	GLSLShader& shader = SHADER.GetShader(shaderType);
	shader.Use();

	for (int i = 0; i < LIGHTING.spotLightCount; ++i)
	{
		int textureUnit = 10 + i;
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, LIGHTING.GetSpotLights()[i].shadow);
		glUniform1i(glGetUniformLocation(shader.GetHandle(), ("myTextureSampler[" + std::to_string(textureUnit) + "]").c_str()), textureUnit);
	
	}

	for (int i = 0; i < LIGHTING.directionalLightCount; ++i)
	{
		int textureUnit = 20 + i;
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, LIGHTING.GetDirectionLights()[i].shadow);
		glUniform1i(glGetUniformLocation(shader.GetHandle(), ("myTextureSampler[" + std::to_string(textureUnit) + "]").c_str()), textureUnit);

	}

	for (int i = 0; i < LIGHTING.pointLightCount; ++i)
	{
		int textureUnit = 22 + i;
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, LIGHTING.GetPointLights()[i].shadow);
		glUniform1i(glGetUniformLocation(shader.GetHandle(), ("PointShadows[" + std::to_string(i) + "]").c_str()), textureUnit); // Associate samplerCube with texture unit 2
	}
	
	glUniform1f(glGetUniformLocation(shader.GetHandle(), "farplane"), 1000.f);

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

	BindLights(shader);

	glBindVertexArray(_vaoid);
	glDrawElementsInstanced(_primType, _primCount, GL_UNSIGNED_INT, 0, _instanceCount);
	glBindVertexArray(0);

	shader.UnUse();
}


void Renderer::BindLights(GLSLShader& shader) {
	// POINT LIGHT STUFFS
	auto PointLight_Sources = LIGHTING.GetPointLights();

	for (int i = 0; i < LIGHTING.pointLightCount; ++i)
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
	glUniform1i(uniform7, (int)LIGHTING.pointLightCount);

	// DIRECTIONAL LIGHT STUFFS
	auto DirectionLight_Sources = LIGHTING.GetDirectionLights();
	for (int i = 0; i < LIGHTING.directionalLightCount; ++i)
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

		std::string directional_LSM;
		directional_LSM = "directionalLights[" + std::to_string(i) + "].lightSpaceMatrix";
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), directional_LSM.c_str())
			, 1, GL_FALSE, glm::value_ptr(DirectionLight_Sources[i].lightSpaceMatrix));

	}

	GLint uniform8 =
		glGetUniformLocation(shader.GetHandle(), "DirectionalLight_Count");
	glUniform1i(uniform8, (int)LIGHTING.directionalLightCount);

	// SPOTLIGHT STUFFS
	auto SpotLight_Sources = LIGHTING.GetSpotLights();
	for (int i = 0; i < LIGHTING.spotLightCount; ++i)
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

		std::string spot_LSM;
		spot_LSM = "spotLights[" + std::to_string(i) + "].lightSpaceMatrix";
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), spot_LSM.c_str())
			, 1, GL_FALSE, glm::value_ptr(SpotLight_Sources[i].lightSpaceMatrix));

	}

	GLint uniform9 =
		glGetUniformLocation(shader.GetHandle(), "SpotLight_Count");
	glUniform1i(uniform9, (int)LIGHTING.spotLightCount);

	GLint uniform10 =
		glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix_Directional");
	glUniformMatrix4fv(uniform10, 1, GL_FALSE,
		glm::value_ptr(DirectionLight_Sources[0].lightSpaceMatrix));

	GLint uniform11 =
		glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix_Spot");
	glUniformMatrix4fv(uniform11, 1, GL_FALSE,
		glm::value_ptr(SpotLight_Sources[0].lightSpaceMatrix));

	glUniform1f(glGetUniformLocation(shader.GetHandle(), "farplane"), 1000.f);

	glUniform1f(glGetUniformLocation(shader.GetHandle(), "bloomThreshold"), bloomThreshold);

	GLint uniform12 = glGetUniformLocation(shader.GetHandle(), "renderShadow");
	glUniform1f(uniform12, RENDERER.enableShadows());
	glUniform1f(glGetUniformLocation(shader.GetHandle(), "ambience_multiplier"), RENDERER.getAmbient());
}

void Renderer::UIDraw_2D(BaseCamera& _camera)
{
	// Setups required for all UI
 	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_DEPTH_BUFFER_BIT);
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
		if (Sprite.state == DELETED) continue;

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
		if (Sprite.state == DELETED) continue;
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
		if (currCanvas.state == DELETED) continue;
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



void Renderer::DrawDepthDirectional()
{
	for (int i = 0; i < LIGHTING.directionalLightCount; ++i)
	{
		LightProperties directional_light_stuffs = LIGHTING.GetDirectionLights()[i];

		glEnable(GL_DEPTH_TEST);
		//glm::vec3 lightPos(-0.2f, -1.0f, -0.3f); // This suppouse to be the actual light direction
		glm::mat4 lightProjection, lightView;
		float near_plane = -100.f, far_plane = 100.f;


		lightProjection = glm::ortho(-50.f, 50.f, -50.f, 50.f, near_plane, far_plane);
		//lightView = glm::lookAt(-directional_light_stuffs.direction + EditorCam.GetCameraPosition(), EditorCam.GetCameraPosition(), glm::vec3(0.0, 1.0, 0.0));
		lightView = glm::lookAt(-directional_light_stuffs.direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

		LIGHTING.GetDirectionLights()[i].lightSpaceMatrix = lightProjection * lightView;
		//lightSpaceMatrix_directional = lightProjection * lightView;

		glViewport(0, 0, SHADOW_WIDTH_DIRECTIONAL, SHADOW_HEIGHT_DIRECTIONAL);
		glBindFramebuffer(GL_FRAMEBUFFER, directional_light_stuffs.shadowFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		GLSLShader& shader = SHADER.GetShader(SHADERTYPE::SHADOW);
		shader.Use();

		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix");
		glUniformMatrix4fv(uniform1, 1, GL_FALSE,
			glm::value_ptr(LIGHTING.GetDirectionLights()[i].lightSpaceMatrix));

		GLint uniform2 = glGetUniformLocation(shader.GetHandle(), "isDefault");

		// render all non-instanced shadows
		glUniform1i(uniform2, true);

		for (DefaultRenderProperties prop : defaultProperties)
		{
			GLint uniform3_t =
				glGetUniformLocation(shader.GetHandle(), "defaultSRT");
			glUniformMatrix4fv(uniform3_t, 1, GL_FALSE, glm::value_ptr(prop.entitySRT));

			glUniform1i(glGetUniformLocation(shader.GetHandle(), "isAnim"), prop.isAnimatable);
			if (prop.isAnimatable)
			{
				std::vector<glm::mat4> transforms = *finalBoneMatContainer[prop.boneidx];
				GLint uniform4 = glGetUniformLocation(shader.GetHandle(), "finalBonesMatrices");
				glUniformMatrix4fv(uniform4, (GLsizei)transforms.size(), GL_FALSE, glm::value_ptr(transforms[0]));
			}

			glBindVertexArray(prop.VAO);
			glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}


		// render all instanced shadows
		glUniform1i(uniform2, false);

		for (auto& [vao, prop] : instanceContainers[static_cast<int>(SHADERTYPE::PBR)])
		{
			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (prop.entitySRT.size()) * sizeof(glm::mat4), &(prop.entitySRT[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(prop.VAO);
			glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);
			glBindVertexArray(0);
		}

		shader.UnUse();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void Renderer::DrawDepthSpot()
{
	for (int i = 0; i < LIGHTING.spotLightCount; ++i)
	{
		LightProperties spot_light_stuffs = LIGHTING.GetSpotLights()[i];

		glEnable(GL_DEPTH_TEST);
		glm::mat4 lightProjection, lightView;

		lightProjection = glm::perspective<float>(glm::radians(90.f), 1.f, 0.1f, 100.f);
		lightView = glm::lookAt(spot_light_stuffs.lightpos, spot_light_stuffs.lightpos +
			(spot_light_stuffs.direction * 100.f), glm::vec3(0.0, 0.0, 1.0));

		LIGHTING.GetSpotLights()[i].lightSpaceMatrix = lightProjection * lightView;
		//lightSpaceMatrix_spot = lightProjection * lightView;

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, spot_light_stuffs.shadowFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		GLSLShader& shader = SHADER.GetShader(SHADERTYPE::SHADOW);
		shader.Use();

		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix");
		glUniformMatrix4fv(uniform1, 1, GL_FALSE,
			glm::value_ptr(LIGHTING.GetSpotLights()[i].lightSpaceMatrix));

		GLint uniform2 = glGetUniformLocation(shader.GetHandle(), "isDefault");

		// render all non-instanced shadows
		glUniform1i(uniform2, true);

		for (DefaultRenderProperties prop : defaultProperties)
		{
			GLint uniform3_t =
				glGetUniformLocation(shader.GetHandle(), "defaultSRT");
			glUniformMatrix4fv(uniform3_t, 1, GL_FALSE, glm::value_ptr(prop.entitySRT));

			glUniform1i(glGetUniformLocation(shader.GetHandle(), "isAnim"), prop.isAnimatable);
			if (prop.isAnimatable)
			{
				std::vector<glm::mat4> transforms = *finalBoneMatContainer[prop.boneidx];
				GLint uniform4 = glGetUniformLocation(shader.GetHandle(), "finalBonesMatrices");
				glUniformMatrix4fv(uniform4, (GLsizei)transforms.size(), GL_FALSE, glm::value_ptr(transforms[0]));
			}

			glBindVertexArray(prop.VAO);
			glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		// render all instanced shadows
		glUniform1i(uniform2, false);

		for (auto& [vao, prop] : instanceContainers[static_cast<int>(SHADERTYPE::PBR)])
		{
			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (prop.entitySRT.size()) * sizeof(glm::mat4), &(prop.entitySRT[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(prop.VAO);
			glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);
			glBindVertexArray(0);
		}

		shader.UnUse();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void Renderer::DrawDepthPoint()
{

	for (int i = 0; i < LIGHTING.pointLightCount; ++i)
	{
		LightProperties point_light_stuffs = LIGHTING.GetPointLights()[i];

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
		glBindFramebuffer(GL_FRAMEBUFFER, point_light_stuffs.shadowFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		GLSLShader& shader = SHADER.GetShader(SHADERTYPE::POINTSHADOW);
		shader.Use();

		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "shadowMatrices")
			, 6, GL_FALSE, glm::value_ptr(shadowTransforms[0]));

		GLint uniform1 =
			glGetUniformLocation(shader.GetHandle(), "lightPos");
		glUniform3fv(uniform1, 1,
			glm::value_ptr(point_light_stuffs.lightpos));

		GLint uniform2 =
			glGetUniformLocation(shader.GetHandle(), "far_plane");
		glUniform1f(uniform2, far_plane);

		GLint uniform3 = glGetUniformLocation(shader.GetHandle(), "isDefault");

		// render all non-instanced shadows
		glUniform1i(uniform3, true);

		for (DefaultRenderProperties prop : defaultProperties)
		{
			GLint uniform3_t =
				glGetUniformLocation(shader.GetHandle(), "defaultSRT");
			glUniformMatrix4fv(uniform3_t, 1, GL_FALSE, glm::value_ptr(prop.entitySRT));

			glUniform1i(glGetUniformLocation(shader.GetHandle(), "isAnim"), prop.isAnimatable);
			if (prop.isAnimatable)
			{
				std::vector<glm::mat4> transforms = *finalBoneMatContainer[prop.boneidx];
				GLint uniform4 = glGetUniformLocation(shader.GetHandle(), "finalBonesMatrices");
				glUniformMatrix4fv(uniform4, (GLsizei)transforms.size(), GL_FALSE, glm::value_ptr(transforms[0]));
			}

			glBindVertexArray(prop.VAO);
			glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}


		// render all instanced shadows
		glUniform1i(uniform3, false);

		for (auto& [vao, prop] : instanceContainers[static_cast<int>(SHADERTYPE::PBR)])
		{
			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (prop.entitySRT.size()) * sizeof(glm::mat4), &(prop.entitySRT[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(prop.VAO);
			glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);
			glBindVertexArray(0);
		}

		shader.UnUse();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
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
