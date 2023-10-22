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
#include "Editor_Camera.h"

#include "GBuffer.h"

void Renderer::Init()
{
	//instanceContainers.resize(static_cast<size_t>(SHADERTYPE::COUNT));
	m_gBuffer.Init(1600, 900);
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

	// loop through mesh renderer
	for (MeshRenderer& renderer : currentScene.GetArray<MeshRenderer>())
	{

		Mesh* t_Mesh = MeshManager.DereferencingMesh(renderer.MeshName);
		if (t_Mesh == nullptr)
		{
			continue;
		}
		//int index = t_Mesh->index;

		Entity& entity = currentScene.Get<Entity>(renderer);
		Transform& transform = currentScene.Get<Transform>(entity);

		// Loop through camera (@euan)

		// for each cam

		// if instance rendering put into container for instance rendering
		if (renderer.isInstance) {

			// dun need do dis, coz renderer contains shadertype, can access via that
			// for each shader, slot in properties into container
			/*for (int s = 0; s < static_cast<int>(SHADERTYPE::COUNT); ++s) {
				//SHADERTYPE sType = static_cast<SHADERTYPE>(s);
				instanceProperties.emplace();
				instanceContainers[s].emplace();
			}*/

			size_t s = static_cast<size_t>(renderer.shaderType);
			GLuint vao = MeshManager.vaoMap[renderer.MeshName]; // pls ask someone how to use GUID instead because deadlock
			//GLuint vao = renderer.VAO;

			//instanceProperties[vao];
			//instanceContainers[s][vao]; // holy shit u can do this?? this is map in a vec sia

			// slot in the InstanceProperties into this vector if it doesnt alr exist
			if (instanceContainers[s].find(vao) == instanceContainers[s].cend()) {
				instanceContainers[s].emplace(std::pair(vao, instanceProperties[vao]));
			}

			// use the properties container coz its made for instance rendering already
			float texidx = float(ReturnTextureIdx(instanceContainers[s][vao], renderer.textureID));
			float normidx = float(ReturnTextureIdx(instanceContainers[s][vao], renderer.normalMapID));

			float metalidx = float(ReturnTextureIdx(instanceContainers[s][vao], renderer.MetallicID));
			float roughidx = float(ReturnTextureIdx(instanceContainers[s][vao], renderer.RoughnessID));
			float aoidx = float(ReturnTextureIdx(instanceContainers[s][vao], renderer.AoID));
			float emissionidx = float(ReturnTextureIdx(instanceContainers[s][vao], renderer.EmissionID));

			float metal_constant = renderer.mr_metallic;
			float rough_constant = renderer.mr_roughness;
			float ao_constant = renderer.ao;

			unsigned int& iter = instanceContainers[s][vao].iter;
			instanceContainers[s][vao].M_R_A_Constant[iter] = glm::vec3(metal_constant, rough_constant, ao_constant);
			instanceContainers[s][vao].M_R_A_Texture[iter] = glm::vec4(metalidx, roughidx, aoidx, emissionidx);
			instanceContainers[s][vao].textureIndex[iter] = glm::vec2(texidx, normidx);

			instanceContainers[s][vao].Albedo[iter] = renderer.mr_Albedo;
			instanceContainers[s][vao].Ambient[iter] = renderer.mr_Ambient;
			instanceContainers[s][vao].Diffuse[iter] = renderer.mr_Diffuse;
			instanceContainers[s][vao].Specular[iter] = renderer.mr_Specular;
			instanceContainers[s][vao].Shininess[iter] = renderer.mr_Shininess;
			instanceContainers[s][vao].entitySRT[iter] = transform.GetWorldMatrix();
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

		}
		else /*if default rendering*/{
			// if not instance put into container for default rendering

			// batch it via shader, geom, material instanced
			// whenever things reach limit, draw
			// means need calculate SRT here
			unsigned int iter = 0;
			for (unsigned int vao : t_Mesh->Vaoids) {
				DefaultRenderProperties renderProperties;
				
				renderProperties.VAO = vao;

				renderProperties.shininess = renderer.mr_Shininess;
				renderProperties.metallic = renderer.mr_metallic;
				renderProperties.roughness = renderer.mr_roughness;
				renderProperties.ao = renderer.ao;

				renderProperties.entitySRT = transform.GetWorldMatrix();
				renderProperties.Albedo = renderer.mr_Albedo;
				renderProperties.Specular = renderer.mr_Specular;
				renderProperties.Diffuse = renderer.mr_Diffuse;
				renderProperties.Ambient = renderer.mr_Ambient;

				renderProperties.textureID = renderer.textureID;
				renderProperties.NormalID = renderer.normalMapID;

				renderProperties.drawType = t_Mesh->prim;
				renderProperties.drawCount = t_Mesh->Drawcounts[iter++];

				defaultProperties.emplace_back(renderProperties);
			}
		}
		
		++i;

		// I am putting it here temporarily, maybe this should move to some editor area :MOUSE PICKING
		if (DEBUGDRAW.HasSelection())
		{

			glm::mat4 transMatrix = transform.GetWorldMatrix();

			//glm::mat4 noscale = translation_mat * rotation_mat;

			glm::vec3 translation;
			glm::quat rot;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::vec3 scale;
			glm::decompose(transMatrix, scale, rot, translation, skew, perspective);

			glm::vec3 mins = scale * MeshManager.DereferencingMesh(renderer.MeshName)->vertices_min;
			glm::vec3 maxs = scale * MeshManager.DereferencingMesh(renderer.MeshName)->vertices_max;
			glm::mat4 rotMat = glm::toMat4(rot);

			float& intersect = DEBUGDRAW.GetIntersect();
			float& tempIntersect = DEBUGDRAW.GetTempIntersect();
			Ray3D temp = EditorCam.GetRay();
			if (testRayOBB(temp.origin, temp.direction, mins, maxs,
				glm::translate(glm::mat4(1.0f), translation) * rotMat, tempIntersect))
			{
				if (tempIntersect < intersect)
				{
					SelectedEntityEvent SelectingEntity(&entity);
					EVENTS.Publish(&SelectingEntity);
					intersect = tempIntersect;
				}
			}
		}
	}

	SetupGrid(100);
}

void Renderer::SetupGrid(const int& _num)
{
	//float spacing = 100.f;
	//float length = _num * spacing * 0.5f;

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

void Renderer::Draw()
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
			glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec3), &(prop.M_R_A_Texture[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, prop.Metal_Rough_AO_Texture_Constant);
			glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec3), &(prop.M_R_A_Constant[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, prop.textureIndexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec2), &(prop.textureIndex[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			for (int i = 0; i < 32; ++i)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, prop.texture[i]);
			}
			DrawMeshes(prop.VAO, prop.iter, prop.drawCount, prop.drawType, LIGHTING.GetLight(), static_cast<SHADERTYPE>(s));
			prop.iter = 0;
		}
		
		// FOR DEBUG DRAW
		/*if (EditorScene::Instance().DebugDraw())
		{
			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			if (prop.debugVAO)
				DrawDebug(prop.debugVAO, prop.iter);
		}
		if (name == "Line")
		{
			glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			if (prop.VAO)
				DrawGrid(prop.VAO, prop.iter);
		}*/

	}

	// non-instanced render
	for (DefaultRenderProperties& prop : defaultProperties) {

		
		//glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer.gFBO);
		//unsigned int attachments[3] =
		//{ GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 }; // position, normal, albedospec
		//glDrawBuffers(3, attachments);
		//
		
		//GLSLShader& geomPass = SHADER.GetShader(GBUFFER);
		//geomPass.Use();
		//GLint uProj =
		//	glGetUniformLocation(geomPass.GetHandle(), "persp_projection");
		//GLint uView =
		//	glGetUniformLocation(geomPass.GetHandle(), "View");
		//GLint SRT =
		//	glGetUniformLocation(geomPass.GetHandle(), "SRT");
		//
		//glUniformMatrix4fv(uProj, 1, GL_FALSE,
		//	glm::value_ptr(EditorCam.GetProjMatrix()));
		//glUniformMatrix4fv(uView, 1, GL_FALSE,
		//	glm::value_ptr(EditorCam.GetViewMatrix()));
		//glUniformMatrix4fv(SRT, 1, GL_FALSE,
		//	glm::value_ptr(prop.entitySRT));
		////m_gBuffer.BindForWriting();
		//glBindVertexArray(prop.VAO);
		//glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);
		//geomPass.UnUse();
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//// lighting pass
		//// render

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_gBuffer.gPosition);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, m_gBuffer.gNormal);
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, m_gBuffer.gAlbedoSpec);

		glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, prop.textureID);
		glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, prop.NormalID);
		glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, prop.RoughnessID);
		glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, prop.MetallicID);
		glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, prop.AoID);
		glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_2D, prop.EmissionID);

		GLSLShader& shader =  SHADER.GetShader(SHADERTYPE::DEFAULT);
		shader.Use();

		GLint hasTexture = glGetUniformLocation(shader.GetHandle(), "hasTexture");
		GLint hasNormal = glGetUniformLocation(shader.GetHandle(), "hasNormal");
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


		GLint uProj = glGetUniformLocation(shader.GetHandle(), "persp_projection");
		GLint uView = glGetUniformLocation(shader.GetHandle(), "View");
		GLint SRT = glGetUniformLocation(shader.GetHandle(), "SRT");
		GLint Albedo = glGetUniformLocation(shader.GetHandle(), "Albedo");
		GLint Specular = glGetUniformLocation(shader.GetHandle(), "Specular");
		GLint Diffuse = glGetUniformLocation(shader.GetHandle(), "Diffuse");
		GLint Ambient = glGetUniformLocation(shader.GetHandle(), "Ambient");
		GLint Shininess = glGetUniformLocation(shader.GetHandle(), "Shininess");
		GLint lightColor = glGetUniformLocation(shader.GetHandle(), "lightColor");
		GLint lightPos = glGetUniformLocation(shader.GetHandle(), "lightPos");
		GLint camPos = glGetUniformLocation(shader.GetHandle(), "camPos");

		glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(EditorCam.GetProjMatrix()));
		glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(EditorCam.GetViewMatrix()));
		glUniformMatrix4fv(SRT, 1, GL_FALSE, glm::value_ptr(prop.entitySRT));
		glUniform4fv(Albedo, 1, glm::value_ptr(prop.Albedo));
		glUniform4fv(Specular, 1, glm::value_ptr(prop.Specular));
		glUniform4fv(Diffuse, 1, glm::value_ptr(prop.Diffuse));
		glUniform4fv(Ambient, 1, glm::value_ptr(prop.entitySRT));
		glUniform1f(Shininess, prop.shininess);
		glUniform3fv(lightColor, 1, glm::value_ptr(LIGHTING.GetLight().lightColor));
		glUniform3fv(lightPos, 1, glm::value_ptr(LIGHTING.GetLight().lightpos));
		glUniform3fv(camPos, 1, glm::value_ptr(EditorCam.GetCameraPosition()));

		// POINT LIGHT STUFFS
		auto PointLight_Sources = LIGHTING.GetPointLights();
		for (int i = 0; i < PointLight_Sources.size(); ++i)
		{
			//pointLights.colour
			std::string point_color;
			point_color = "pointLights[" + std::to_string(i) + "].colour";

			glUniform3fv(glGetUniformLocation(shader.GetHandle(), point_color.c_str())
				, 1, glm::value_ptr(PointLight_Sources[i].lightColor));

			std::string point_pos;
			point_pos = "pointLights[" + std::to_string(i) + "].position";
			glUniform3fv(glGetUniformLocation(shader.GetHandle(), point_pos.c_str())
				, 1, glm::value_ptr(PointLight_Sources[i].lightpos));

			std::string point_intensity;
			point_intensity = "pointLights[" + std::to_string(i) + "].intensity";
			glUniform1fv(glGetUniformLocation(shader.GetHandle(), point_intensity.c_str())
				, 1, &PointLight_Sources[i].intensity);


		}

		GLint uniform7 = glGetUniformLocation(shader.GetHandle(), "PointLight_Count");
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

		GLint uniform8 = glGetUniformLocation(shader.GetHandle(), "DirectionalLight_Count");
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
		GLint uniform9 = glGetUniformLocation(shader.GetHandle(), "SpotLight_Count");
		glUniform1i(uniform9, (int)SpotLight_Sources.size());

		glBindVertexArray(prop.VAO);
		glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		shader.UnUse();/**/

		// forward render
		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, prop.textureID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, prop.NormalID);*/


		/*GLSLShader& shader =  SHADER.GetShader(DEFAULT);
		shader.Use();

		GLint uProj =
			glGetUniformLocation(shader.GetHandle(), "persp_projection");
		GLint uView =
			glGetUniformLocation(shader.GetHandle(), "View");
		GLint SRT =
			glGetUniformLocation(shader.GetHandle(), "SRT");

		glUniformMatrix4fv(uProj, 1, GL_FALSE,
			glm::value_ptr(EditorCam.GetProjMatrix()));
		glUniformMatrix4fv(uView, 1, GL_FALSE,
			glm::value_ptr(EditorCam.GetViewMatrix()));
		glUniformMatrix4fv(SRT, 1, GL_FALSE,
			glm::value_ptr(prop.entitySRT));

		glBindVertexArray(prop.VAO);
		glDrawElements(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		shader.UnUse();*/
		
	}
	
	
}

void Renderer::DrawMeshes(const GLuint& _vaoid, const unsigned int& _instanceCount,
	const unsigned int& _primCount, GLenum _primType, const LightProperties& _lightSource, SHADERTYPE shaderType)
{
	//testBox.instanceDraw(EntityRenderLimit);

	glEnable(GL_DEPTH_TEST); // might be sus to place this here

	GLSLShader& shader = SHADER.GetShader(shaderType);
	shader.Use();

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
	GLint uniform3 =
		glGetUniformLocation(shader.GetHandle(), "lightColor");
	GLint uniform4 =
		glGetUniformLocation(shader.GetHandle(), "lightPos");
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
		glm::value_ptr(EditorCam.GetProjMatrix()));
	glUniformMatrix4fv(uniform2, 1, GL_FALSE,
		glm::value_ptr(EditorCam.GetViewMatrix()));
	glUniform3fv(uniform3, 1,
		glm::value_ptr(_lightSource.lightColor));
	glUniform3fv(uniform4, 1,
		glm::value_ptr(_lightSource.lightpos));
	glUniform3fv(uniform5, 1,
		glm::value_ptr(EditorCam.GetCameraPosition()));

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
	glUniform1i(uniform7, (int) PointLight_Sources.size());

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
	glUniform1i(uniform8, (int) DirectionLight_Sources.size());

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
	glUniform1i(uniform9, (int) SpotLight_Sources.size());

	glBindVertexArray(_vaoid);
	glDrawElementsInstanced(_primType, _primCount, GL_UNSIGNED_INT, 0, _instanceCount);
	glBindVertexArray(0);

	shader.UnUse();
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