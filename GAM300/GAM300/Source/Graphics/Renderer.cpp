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
#include "MeshManager.h"
#include "Texture/TextureManager.h"
#include "Editor/EditorCamera.h"
#include "Utilities/Serializer.h"

// ALL THIS ARE HOPEFULLY TEMPORARY

// Shadow Mapping - Directional

int numLines = 100;
std::vector<glm::mat4> lineGrids;
std::vector<glm::mat4> lineGrids2;


//LIGHT_TYPE temporary_test4 = POINT_LIGHT;


#include "GBuffer.h"

void Renderer::Init()
{
	// Load graphics settings
	Deserialize("GAM300/Data/GraphicsSettings.txt", *this);

	m_gBuffer.Init(1600, 900);
	
	// For lines
	float spacing = 1.f;
	float length = numLines * spacing * 0.5f;
	for (int i = 0; i < numLines; i++)
	{
		glm::mat4 scalMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(length));
		glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 transMatrixZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.0f, (i * spacing) - length));
		glm::mat4 transMatrixX = glm::translate(glm::mat4(1.0f), glm::vec3((i * spacing) - length, 0.0f, 0.f));
		lineGrids.push_back(transMatrixZ * scalMatrix);
		lineGrids2.push_back(transMatrixX * rotMatrix * scalMatrix);
	}

	instanceContainers.resize(static_cast<size_t>(SHADERTYPE::COUNT));
	for (auto& [guid, vao] : MESHMANAGER.vaoMap)
	{
		for (size_t j = 0; j < (int)SHADERTYPE::COUNT; j++)
		{
			instanceContainers[j].emplace(vao, instanceProperties[vao]);
			instanceContainers[j][vao].ResizeContainers();
		}
	}
	
}

void Renderer::Update(float)
{
	/*for (auto& container : instanceContainers)
	{
		container.clear();
	}*/

	defaultProperties.clear(); // maybe no need clear everytime, see steve rabin code?
	transparentContainer.clear(); // maybe no need clear everytime, see steve rabin code?
	finalBoneMatContainer.clear();

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

	auto& meshArray = currentScene.GetArray<MeshRenderer>();
	for (auto it = meshArray.begin(); it != meshArray.end(); ++it)
	{
		if (!it.IsActive()) continue; // Bean: IsActive is tanking fps
		// No material instance, then just go next
		MeshRenderer& renderer = *it;
		Entity& entity = currentScene.Get<Entity>(renderer);
		if (!currentScene.IsActive(entity)) continue; // Bean: Is there a better way ard this???

		Mesh* t_Mesh = MESHMANAGER.DereferencingMesh(renderer.meshID);
		if (!t_Mesh) continue;

		Transform& transform = currentScene.Get<Transform>(entity);
		const glm::mat4 worldMatrix = transform.GetWorldMatrix();
		const glm::vec3 position = transform.GetGlobalTranslation();

		if (frustumCulling)
		{
			bool withinCamera = false;
			for (Camera& camera : currentScene.GetArray<Camera>())
			{
				if (camera.state == DELETED) continue;

				if (!currentScene.IsActive(camera)) continue;

				if (camera.WithinFrustum(transform, t_Mesh->vertices_min, t_Mesh->vertices_max))
				{
					withinCamera = true;
					break;
				}
			}

			if (!withinCamera) continue;
		}

		auto vaoIt = MESHMANAGER.vaoMap.find(renderer.meshID);
		if (vaoIt == MESHMANAGER.vaoMap.end()) continue;

		Material_instance& currMatInstance = MATERIALSYSTEM.getMaterialInstance(renderer.materialGUID);
		if (currMatInstance.shaderType == (int)SHADERTYPE::DEFAULT)
		{
			UpdateDefaultProperties(currentScene, transform, currMatInstance, vaoIt->second, t_Mesh->prim, t_Mesh->drawCounts);
		}
		else
		{
			UpdatePBRProperties(transform, currMatInstance, vaoIt->second);
		}
		++i;
		
	} // END MESHRENDERER LOOP
}

void Renderer::UpdateDefaultProperties(Scene& _scene, Transform& _t, Material_instance& _mat, const GLuint& _vao, const GLenum& _type, const GLuint& _count)
{
	//GLuint& vao = MESHMANAGER.vaoMap[renderer.meshID];
	DefaultRenderProperties renderProperties;
	renderProperties.VAO = _vao;

	renderProperties.metallic = _mat.metallicConstant;
	renderProperties.roughness = _mat.roughnessConstant;
	renderProperties.ao = _mat.aoConstant;
	renderProperties.emission = _mat.emissionConstant;

	renderProperties.entitySRT = _t.GetWorldMatrix();
	renderProperties.Albedo = _mat.albedoColour;

	renderProperties.textureID = _mat.textureID;
	renderProperties.NormalID = _mat.normalID;
	renderProperties.MetallicID = _mat.metallicID;
	renderProperties.RoughnessID = _mat.roughnessID;
	renderProperties.AoID = _mat.ambientID;
	renderProperties.EmissionID = _mat.emissiveID;

	renderProperties.drawType = _type;
	renderProperties.drawCount = _count;

	renderProperties.isAnimatable = false;
	renderProperties.boneidx = -1;
	renderProperties.position = _t.GetGlobalTranslation();

	if (_t.parent) /*if (meshIsanimatable)*/ // if mesh can be animated + there is an animaator in its parent
	{
		Entity& parententity = _scene.Get<Entity>(_t.parent);
		if (_scene.Has<Animator>(parententity))
		{
			Animator& animator = _scene.Get<Animator>(parententity);

			if (animator.AnimationAttached() && animator.m_FinalBoneMatIdx != -1)
			{
				renderProperties.isAnimatable = true; // can remove later, should be in mesh instead
				renderProperties.boneidx = animator.m_FinalBoneMatIdx; // i need to figure out how to access the parent's renderProperties....
			}
		}
	}

	if (renderProperties.Albedo.a != 1.f)// Is a Transparent / Translucent Object
	{
		transparentContainer.emplace_back(renderProperties);
	}
	else
	{
		defaultProperties.emplace_back(renderProperties);
	}
}

void Renderer::UpdatePBRProperties(Transform& _t, Material_instance& _mat, const GLuint& _vao)
{
	//if (currMatInstance.shaderType == (int)SHADERTYPE::PBR)
	size_t s = static_cast<size_t>(SHADERTYPE::PBR);
	//GLuint& vao = MESHMANAGER.vaoMap[renderer.meshID];
	if (instanceContainers[s].find(_vao) == instanceContainers[s].cend()) { // if container does not have this vao, emplace
		instanceContainers[s].emplace(_vao, instanceProperties[_vao]);
		instanceContainers[s][_vao].ResizeContainers();
	}

	float texidx, normidx, metalidx, roughidx, aoidx, emissionidx, metal_constant, rough_constant, ao_constant, emission_constant;
	InstanceProperties& iProp = instanceContainers[s][_vao];
	unsigned int& iter = iProp.iter;

	if (iter >= iProp.maxSize)
		iProp.ResizeContainers();

	metal_constant = _mat.metallicConstant;
	rough_constant = _mat.roughnessConstant;
	ao_constant = _mat.aoConstant;
	emission_constant = _mat.emissionConstant;
	//iProp.Albedo.emplace_back(_mat.albedoColour);
	iProp.Albedo[iter] = _mat.albedoColour;

	texidx = float(ReturnTextureIdx(iProp, _mat.textureID));
	normidx = float(ReturnTextureIdx(iProp, _mat.normalID));

	metalidx = float(ReturnTextureIdx(iProp, _mat.metallicID));
	roughidx = float(ReturnTextureIdx(iProp, _mat.roughnessID));
	aoidx = float(ReturnTextureIdx(iProp, _mat.ambientID));
	emissionidx = float(ReturnTextureIdx(iProp, _mat.emissiveID));

	/*iProp.M_R_A_Constant.emplace_back(metal_constant, rough_constant, ao_constant, emission_constant);
	iProp.M_R_A_Texture.emplace_back(metalidx, roughidx, aoidx, emissionidx);
	iProp.textureIndex.emplace_back(texidx, normidx);
	iProp.entitySRT.emplace_back(_t.GetWorldMatrix());*/

	iProp.M_R_A_Constant[iter] = { metal_constant, rough_constant, ao_constant, emission_constant };
	iProp.M_R_A_Texture[iter] = { metalidx, roughidx, aoidx, emissionidx };
	iProp.textureIndex[iter] = { texidx, normidx };
	iProp.entitySRT[iter] = _t.GetWorldMatrix();

	iProp.position = _t.GetGlobalTranslation();
	++iter;
}

void Renderer::Draw(BaseCamera& _camera) {

	/* Draw PBR **********************************************/
	DrawPBR(_camera);

	/* Draw Default *******************************************/ 
	DrawDefault(_camera);

	glDisable(GL_BLEND);

#ifndef _BUILD
	// For Debug drawing
	if (_camera.GetCameraType() == CAMERATYPE::SCENE)
	{
		for (auto& [vao, prop] : instanceContainers[static_cast<size_t>(SHADERTYPE::TDR)])
		{
			if (EditorScene::Instance().DebugDraw() && prop.debugVAO)
				DrawDebug(prop.debugVAO, (unsigned)prop.iter);
		}
	}
#endif
}

void Renderer::DrawPBR(BaseCamera& _camera)
{
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::PBR);
	shader.Use();

	BindLights(shader);

	GLint uProj = glGetUniformLocation(shader.GetHandle(), "persp_projection");
	GLint uView = glGetUniformLocation(shader.GetHandle(), "View");
	GLint camPos = glGetUniformLocation(shader.GetHandle(), "camPos");

	const glm::vec3 cameraPosition = _camera.GetCameraPosition();
	glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));
	glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));
	glUniform1i(glGetUniformLocation(shader.GetHandle(), "hdr"), hdr);

	glUniform3fv(camPos, 1, glm::value_ptr(cameraPosition));

	// Instanced Rendering	
	for (auto& [vao, prop] : instanceContainers[static_cast<size_t>(SHADERTYPE::PBR)])
	{
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

		for (int i = 0; i < 10; ++i) // this should be up till 10 for now... hehe
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, prop.texture[i]);
		}

		DrawMeshes(vao, prop.iter, prop.drawCount, prop.drawType, SHADERTYPE::PBR);
	}

	shader.UnUse();
}

void Renderer::DrawDefault(BaseCamera& _camera)
{
	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::DEFAULT);
	shader.Use();

	BindLights(shader);

	GLint hasTexture = glGetUniformLocation(shader.GetHandle(), "hasTexture");
	GLint hasNormal = glGetUniformLocation(shader.GetHandle(), "hassNormal");
	GLint hasRoughness = glGetUniformLocation(shader.GetHandle(), "hasRoughness");
	GLint hasMetallic = glGetUniformLocation(shader.GetHandle(), "hasMetallic");
	GLint hasAO = glGetUniformLocation(shader.GetHandle(), "hasAO");
	GLint hasEmission = glGetUniformLocation(shader.GetHandle(), "hasEmission");

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

	GLint Metallic = glGetUniformLocation(shader.GetHandle(), "MetalConstant");
	GLint Roughness = glGetUniformLocation(shader.GetHandle(), "RoughnessConstant");
	GLint AmbientOcculusion = glGetUniformLocation(shader.GetHandle(), "AoConstant");
	GLint Emission = glGetUniformLocation(shader.GetHandle(), "EmissionConstant");

	GLint FinalBoneMatrices = glGetUniformLocation(shader.GetHandle(), "finalBonesMatrices");

	const glm::vec3 cameraPosition = _camera.GetCameraPosition();
	glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));
	glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));
	glUniform1i(glGetUniformLocation(shader.GetHandle(), "hdr"), hdr);
	glUniform3fv(camPos, 1, glm::value_ptr(cameraPosition));

	// Shadow
	glUniform1i(glGetUniformLocation(shader.GetHandle(), "ShadowCubeMap"), 8);

	// Default Rendering
	for (DefaultRenderProperties& prop : defaultProperties)
	{
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

		// PBR TEXTURES
		glUniform1i(hasTexture, prop.textureID);
		glUniform1i(hasNormal, prop.NormalID);
		glUniform1i(hasRoughness, prop.RoughnessID);
		glUniform1i(hasMetallic, prop.MetallicID);
		glUniform1i(hasAO, prop.AoID);
		glUniform1i(hasEmission, prop.EmissionID);

		// PBR CONSTANT VALUES
		glUniform1f(Metallic, prop.metallic);
		glUniform1f(Roughness, prop.roughness);
		glUniform1f(AmbientOcculusion, prop.ao);
		glUniform1f(Emission, prop.emission);

		glUniformMatrix4fv(SRT, 1, GL_FALSE, glm::value_ptr(prop.entitySRT));
		glUniform4fv(Albedo, 1, glm::value_ptr(prop.Albedo));
		glUniform4fv(Specular, 1, glm::value_ptr(prop.Specular));
		glUniform4fv(Diffuse, 1, glm::value_ptr(prop.Diffuse));
		glUniform4fv(Ambient, 1, glm::value_ptr(prop.entitySRT));
		glUniform1f(Shininess, prop.shininess);

		// ANIMATONS
		glUniform1i(hasAnim, prop.isAnimatable);
		if (prop.isAnimatable)
		{
			std::vector<glm::mat4> transforms = *finalBoneMatContainer[prop.boneidx];
			glUniformMatrix4fv(FinalBoneMatrices, (GLsizei)transforms.size(), GL_FALSE, glm::value_ptr(transforms[0]));
		}

		glBindVertexArray(prop.VAO);
		glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}

	std::map<float, DefaultRenderProperties> transparentMap;
	for (int i = 0; i < transparentContainer.size(); ++i)
	{
		float distance = std::abs(glm::distance(glm::vec3(transparentContainer[i].entitySRT[3]), cameraPosition));

		/*auto it = transparentMap.find(distance);
		if (it != transparentMap.end())
		{
			distance += 0.01f;
		}*/
		transparentMap[distance] = transparentContainer[i];
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Transparent Rendering
	for (auto rit = transparentMap.rbegin(); rit != transparentMap.rend(); ++rit) {
		auto& prop = rit->second;

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

		// PBR TEXTURES
		glUniform1i(hasTexture, prop.textureID);
		glUniform1i(hasNormal, prop.NormalID);
		glUniform1i(hasRoughness, prop.RoughnessID);
		glUniform1i(hasMetallic, prop.MetallicID);
		glUniform1i(hasAO, prop.AoID);
		glUniform1i(hasEmission, prop.EmissionID);

		// PBR CONSTANT VALUES
		glUniform1f(Metallic, prop.metallic);
		glUniform1f(Roughness, prop.roughness);
		glUniform1f(AmbientOcculusion, prop.ao);
		glUniform1f(Emission, prop.emission);

		glUniformMatrix4fv(SRT, 1, GL_FALSE, glm::value_ptr(prop.entitySRT));
		glUniform4fv(Albedo, 1, glm::value_ptr(prop.Albedo));
		glUniform4fv(Specular, 1, glm::value_ptr(prop.Specular));
		glUniform4fv(Diffuse, 1, glm::value_ptr(prop.Diffuse));
		glUniform4fv(Ambient, 1, glm::value_ptr(prop.entitySRT));
		glUniform1f(Shininess, prop.shininess);

		// ANIMATONS
		glUniform1i(hasAnim, prop.isAnimatable);
		if (prop.isAnimatable)
		{
			std::vector<glm::mat4> transforms = *finalBoneMatContainer[prop.boneidx];
			glUniformMatrix4fv(FinalBoneMatrices, (GLsizei)transforms.size(), GL_FALSE, glm::value_ptr(transforms[0]));
		}

		glBindVertexArray(prop.VAO);
		glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}
	shader.UnUse();
}

void Renderer::DrawMeshes(const GLuint& _vaoid, const unsigned int& _instanceCount,
	const unsigned int& _primCount, GLenum _primType, SHADERTYPE shaderType)
{
	//testBox.instanceDraw(EntityRenderLimit);

	glEnable(GL_DEPTH_TEST); // might be sus to place this here

	GLSLShader& shader = SHADER.GetShader(shaderType);
	shader.Use();

	glBindVertexArray(_vaoid);
	glDrawElementsInstanced(_primType, _primCount, GL_UNSIGNED_INT, 0, _instanceCount);
	glBindVertexArray(0);

	shader.UnUse();
}


void Renderer::BindLights(GLSLShader& shader) 
{
	int offset = 0;
	for (int i = 0; i < (int)LIGHTING.spotLightCount; ++i)
	{
		if (LIGHTING.GetSpotLights()[i].enableShadow)
		{
			int textureUnit = 10 + offset++;
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, LIGHTING.GetSpotLights()[i].shadow);
		}
	}
	offset = 0;
	for (int i = 0; i < (int)LIGHTING.directionalLightCount; ++i)
	{
		if (LIGHTING.GetDirectionLights()[i].enableShadow)
		{
			int textureUnit = 20 + offset++;
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, LIGHTING.GetDirectionLights()[i].shadow);
		}
	}
	offset = 0;
	for (int i = 0; i < (int)LIGHTING.pointLightCount; ++i)
	{
		if (LIGHTING.GetPointLights()[i].enableShadow)
		{
			int textureUnit = 22 + offset++;
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_CUBE_MAP, LIGHTING.GetPointLights()[i].shadow);
		}
	}

	// POINT LIGHT STUFFS
	auto& PointLight_Sources = LIGHTING.GetPointLights();

	unsigned int allLightsCount = std::max(std::max(LIGHTING.pointLightCount, LIGHTING.directionalLightCount), LIGHTING.spotLightCount);
	auto& DirectionLight_Sources = LIGHTING.GetDirectionLights();
	auto& SpotLight_Sources = LIGHTING.GetSpotLights();
	
	std::string istring, pointLight, directionalLight, spotLight;
	for (int i = 0; i < (int)LIGHTING.pointLightCount; ++i)
	{
		istring = "pointLights[" + std::to_string(i);
		//pointLights.enableShadow
		pointLight = istring + "].enableShadow";
		glUniform1f(glGetUniformLocation(shader.GetHandle(), pointLight.c_str())
			, PointLight_Sources[i].enableShadow);

		//pointLights.colour
		pointLight = istring + "].colour";
		glUniform3fv(glGetUniformLocation(shader.GetHandle(), pointLight.c_str())
			, 1, glm::value_ptr(PointLight_Sources[i].lightColor));

		//pointLights.position
		pointLight = istring + "].position";
		glUniform3fv(glGetUniformLocation(shader.GetHandle(), pointLight.c_str())
			, 1, glm::value_ptr(PointLight_Sources[i].lightpos));

		//pointLights.intensity
		pointLight = istring + "].intensity";
		glUniform1fv(glGetUniformLocation(shader.GetHandle(), pointLight.c_str())
			, 1, &PointLight_Sources[i].intensity);
	}

	// DIRECTIONAL LIGHT STUFFS
	for (int i = 0; i < (int)LIGHTING.directionalLightCount; ++i)
	{
		istring = "directionalLights[" + std::to_string(i);

		//directionalLights.enableShadow
		directionalLight = istring + "].enableShadow";
		glUniform1f(glGetUniformLocation(shader.GetHandle(), directionalLight.c_str())
			, DirectionLight_Sources[i].enableShadow);

		//directionalLights.colour
		directionalLight = istring + "].colour";
		glUniform3fv(glGetUniformLocation(shader.GetHandle(), directionalLight.c_str())
			, 1, glm::value_ptr(DirectionLight_Sources[i].lightColor));

		directionalLight = istring + "].direction";
		glUniform3fv(glGetUniformLocation(shader.GetHandle(), directionalLight.c_str())
			, 1, glm::value_ptr(DirectionLight_Sources[i].direction));

		directionalLight = istring + "].intensity";
		glUniform1fv(glGetUniformLocation(shader.GetHandle(), directionalLight.c_str())
			, 1, &DirectionLight_Sources[i].intensity);

		directionalLight = istring + "].lightSpaceMatrix";
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), directionalLight.c_str())
			, 1, GL_FALSE, glm::value_ptr(DirectionLight_Sources[i].lightSpaceMatrix));

	}

	// SPOTLIGHT STUFFS
	for (int i = 0; i < (int)LIGHTING.spotLightCount; ++i)
	{
		istring = "spotLights[" + std::to_string(i);

		//directionalLights.enableShadow
		spotLight = istring + "].enableShadow";
		glUniform1f(glGetUniformLocation(shader.GetHandle(), spotLight.c_str())
			, SpotLight_Sources[i].enableShadow);

		//spotLights.position
		spotLight = istring + "].position";
		glUniform3fv(glGetUniformLocation(shader.GetHandle(), spotLight.c_str())
			, 1, glm::value_ptr(SpotLight_Sources[i].lightpos));

		spotLight = istring + "].colour";
		glUniform3fv(glGetUniformLocation(shader.GetHandle(), spotLight.c_str())
			, 1, glm::value_ptr(SpotLight_Sources[i].lightColor));

		spotLight = istring + "].direction";
		glUniform3fv(glGetUniformLocation(shader.GetHandle(), spotLight.c_str())
			, 1, glm::value_ptr(SpotLight_Sources[i].direction));

		spotLight = istring + "].innerCutOff";
		glUniform1fv(glGetUniformLocation(shader.GetHandle(), spotLight.c_str())
			, 1, &SpotLight_Sources[i].inner_CutOff);

		spotLight = istring + "].outerCutOff";
		glUniform1fv(glGetUniformLocation(shader.GetHandle(), spotLight.c_str())
			, 1, &SpotLight_Sources[i].outer_CutOff);

		spotLight = istring + "].intensity";
		glUniform1fv(glGetUniformLocation(shader.GetHandle(), spotLight.c_str())
			, 1, &SpotLight_Sources[i].intensity);

		spotLight = istring + "].lightSpaceMatrix";
		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), spotLight.c_str())
			, 1, GL_FALSE, glm::value_ptr(SpotLight_Sources[i].lightSpaceMatrix));

	}
	GLint uniform7 =
		glGetUniformLocation(shader.GetHandle(), "PointLight_Count");
	glUniform1i(uniform7, (int)LIGHTING.pointLightCount);

	GLint uniform8 =
		glGetUniformLocation(shader.GetHandle(), "DirectionalLight_Count");
	glUniform1i(uniform8, (int)LIGHTING.directionalLightCount);

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
	glUniform3fv(glGetUniformLocation(shader.GetHandle(), "ambient_tint"), 1, glm::value_ptr(RENDERER.getAmbientRGB()));
}

//void Renderer::UIDraw_2D(BaseCamera& _camera)
//{
//	// Setups required for all UI
// 	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glClear(GL_DEPTH_BUFFER_BIT);
//	//glm::mat4 OrthoProjection = glm::ortho(-800.f, 800.f, -450.f, 450.f, 0.001f, 10.f);
//	//glm::mat4 OrthoProjection = glm::ortho(0.f, 16.f, 0.f, 9.f, -10.f, 10.f);
//	//glm::mat4 OrthoProjection = glm::ortho(-8.f, 8.f, -4.5f, 4.5f, -10.f, 10.f);
//	glm::mat4 OrthoProjection = glm::ortho(-1.f, 1.f, -1.f, 1.f, -10.f, 10.f);
//
//	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
//	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UI_SCREEN);
//	shader.Use();
//
//	// Setting the projection here since all of them use the same projection
//
//	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
//		1, GL_FALSE, glm::value_ptr(OrthoProjection));
//
//
//	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
//	{
//		if (Sprite.state == DELETED) continue;
//
//		// This means it's 3D space
//		if (Sprite.WorldSpace)
//		{
//			continue;
//		}
//
//		// Declarations for the things we need - SRT
//		Entity& entity = currentScene.Get<Entity>(Sprite);
//		if (!currentScene.IsActive(entity)) continue;
//
//		Transform& transform = currentScene.Get<Transform>(entity);
//
//		// SRT uniform
//		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
//			1, GL_FALSE, glm::value_ptr(transform.GetLocalMatrix()));
//
//		// Setting bool to see if there is a sprite to render
//		GLint uniform1 =
//			glGetUniformLocation(shader.GetHandle(), "RenderSprite");
//		GLuint spriteTextureID = TextureManager.GetTexture(Sprite.SpriteTexture);
//		if (Sprite.SpriteTexture == 0)
//		{
//			glUniform1f(uniform1, false);
//		}
//		else
//		{
//			glUniform1f(uniform1, true);
//		}
//		
//		// Binding Texture - might be empty , above uniform will sort it
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, spriteTextureID);
//		
//
//		renderQuad(Renderer_quadVAO, Renderer_quadVBO);
//
//	}
//	shader.UnUse();
//	glDisable(GL_BLEND);
//
//}
//
//void Renderer::UIDraw_3D(BaseCamera& _camera)
//{
//	// Setups required for all UI
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
//	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UI_WORLD);
//	shader.Use();
//
//	// Setting the projection here since all of them use the same projection
//
//	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
//		1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));
//	
//	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "view"),
//		1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));
//
//	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
//	{
//		if (Sprite.state == DELETED) continue;
//		// This means it's 2D space
//		if (!Sprite.WorldSpace)
//		{
//			continue;
//		}
//
//		// Declarations for the things we need - SRT
//		Entity& entity = currentScene.Get<Entity>(Sprite);
//		if (!currentScene.IsActive(entity)) continue;
//
//		Transform& transform = currentScene.Get<Transform>(entity);
//
//		// SRT uniform
//		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
//			1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix()));
//
//		// Setting bool to see if there is a sprite to render
//		GLint uniform1 =
//			glGetUniformLocation(shader.GetHandle(), "RenderSprite");
//		GLuint spriteTextureID = TextureManager.GetTexture(Sprite.SpriteTexture);
//		if (Sprite.SpriteTexture == 0)
//		{
//			glUniform1f(uniform1, false);
//		}
//		else
//		{
//			glUniform1f(uniform1, true);
//		}
//
//		// Binding Texture - might be empty , above uniform will sort it
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, spriteTextureID);
//
//		renderQuad(Renderer_quadVAO, Renderer_quadVBO);
//	}
//	shader.UnUse();
//	glDisable(GL_BLEND);
//
//}
//
//void Renderer::UIDraw_2DWorldSpace(BaseCamera& _camera)
//{
//	// Setups required for all UI
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
//	GLSLShader& shader = SHADER.GetShader(SHADERTYPE::UI_WORLD);
//	shader.Use();
//
//	// Setting the projection here since all of them use the same projection
//	
//	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
//		1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));
//
//	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "view"),
//		1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));
//
//	
//	glUniform1f(glGetUniformLocation(shader.GetHandle(), "RenderSprite"), false);
//
//	for (Canvas& currCanvas : currentScene.GetArray<Canvas>())
//	{
//		if (currCanvas.state == DELETED) continue;
//		Entity& entity = currentScene.Get<Entity>(currCanvas);
//		Transform& transform = currentScene.Get<Transform>(entity);
//
//		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
//			1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix()));
//
//		glLineWidth(20.f);
//		renderQuadWireMesh(Renderer_quadVAO_WM, Renderer_quadVBO_WM);
//	}
//	glLineWidth(1.f);
//
//	// Setting the projection here since all of them use the same projection
//
//	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "projection"),
//		1, GL_FALSE, glm::value_ptr(_camera.GetProjMatrix()));
//
//	glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "view"),
//		1, GL_FALSE, glm::value_ptr(_camera.GetViewMatrix()));
//
//	for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
//	{
//		// This means it's 2D space
//		if (Sprite.WorldSpace)
//		{
//			continue;
//		}
//
//		// Declarations for the things we need - SRT
//		Entity& entity = currentScene.Get<Entity>(Sprite);
//		if (!currentScene.IsActive(entity)) continue;
//
//		Transform& transform = currentScene.Get<Transform>(entity);
//
//		// SRT uniform
//		glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "SRT"),
//			1, GL_FALSE, glm::value_ptr(transform.GetWorldMatrix()));
//
//		// Setting bool to see if there is a sprite to render
//		GLint uniform1 =
//			glGetUniformLocation(shader.GetHandle(), "RenderSprite");
//		GLuint spriteTextureID = TextureManager.GetTexture(Sprite.SpriteTexture);
//		if (Sprite.SpriteTexture == 0)
//		{
//			glUniform1f(uniform1, false);
//		}
//		else
//		{
//			glUniform1f(uniform1, true);
//		}
//
//		// Binding Texture - might be empty , above uniform will sort it
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, spriteTextureID);
//
//		renderQuad(Renderer_quadVAO, Renderer_quadVBO);
//	}
//	shader.UnUse();
//	glDisable(GL_BLEND);
//
//}

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
	glDrawElementsInstanced(GL_LINES, 2, GL_UNSIGNED_INT, 0, _instanceCount);
	glBindVertexArray(0);

	shader.UnUse();
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
	//if (prop.textureMap.find(_id) != prop.textureMap.end()) {
	//	return prop.textureMap[_id];
	//}

	//// Check if there are empty slots in the texture array
	//if (prop.textureCount < 32) {
	//	prop.texture[prop.textureCount] = _id;
	//	prop.textureMap[_id] = prop.textureCount++;
	//	return prop.textureCount - 1;
	//}
	for (unsigned int iter = 0; iter < prop.textureCount + 1; ++iter)
	{
		if (prop.texture[iter] == _id) // this happen more often in big scene
		{
			prop.textureCount++;
			return iter;
		}
		if (prop.texture[iter] == 0)
		{
			prop.texture[iter] = _id;
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
