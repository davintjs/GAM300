/*!***************************************************************************************
\file			Renderer.cpp
\project
\author

\par			Course: GAM300
\date           11/10/2023

\brief
	This file contains the definitions of Graphics Renderer that includes:
	1.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "GraphicsHeaders.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Scene/SceneManager.h"
#include "MeshManager.h"
#include "Editor_Camera.h"

void Renderer::Init()
{

}

void Renderer::Update(float)
{
	for (auto& [name, prop] : instanceProperties)
	{
		std::fill_n(prop.textureIndex, EnitityInstanceLimit, glm::vec2(0.f));
		std::fill_n(prop.M_R_A_Texture, EnitityInstanceLimit, glm::vec4(33.f));
		std::fill_n(prop.texture, 32, 0);
	}
	defaultProperties.clear();
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
		//InstanceProperties* currentProp = &instanceProperties[renderer.MeshName];

		//std::string textureGUID = AssetManager::Instance().GetAssetGUID(renderer.AlbedoTexture); // problem eh
		// use bool to see if texture exist instead...

		

		// Loop through camera (@euan)

		// for each cam

		// if instance rendering put into container for instance rendering
		if (renderer.isInstance) {
			// use the properties container coz its made for instance rendering already
			float texidx = float(ReturnTextureIdx(renderer.MeshName, renderer.textureID));
			float normidx = float(ReturnTextureIdx(renderer.MeshName, renderer.normalMapID));


			float metalidx = float(ReturnTextureIdx(renderer.MeshName, renderer.MetallicID));
			float roughidx = float(ReturnTextureIdx(renderer.MeshName, renderer.RoughnessID));
			float aoidx = float(ReturnTextureIdx(renderer.MeshName, renderer.AoID));
			float emissionidx = float(ReturnTextureIdx(renderer.MeshName, renderer.EmissionID));

			float metal_constant = renderer.mr_metallic;
			float rough_constant = renderer.mr_roughness;
			float ao_constant = renderer.ao;

			instanceProperties[renderer.MeshName].M_R_A_Constant[instanceProperties[renderer.MeshName].iter] = glm::vec3(metal_constant, rough_constant, ao_constant);
			instanceProperties[renderer.MeshName].M_R_A_Texture[instanceProperties[renderer.MeshName].iter] = glm::vec4(metalidx, roughidx, aoidx, emissionidx);
			instanceProperties[renderer.MeshName].textureIndex[instanceProperties[renderer.MeshName].iter] = glm::vec2(texidx, normidx);

			instanceProperties[renderer.MeshName].Albedo[instanceProperties[renderer.MeshName].iter] = renderer.mr_Albedo;
			instanceProperties[renderer.MeshName].Ambient[instanceProperties[renderer.MeshName].iter] = renderer.mr_Ambient;
			instanceProperties[renderer.MeshName].Diffuse[instanceProperties[renderer.MeshName].iter] = renderer.mr_Diffuse;
			instanceProperties[renderer.MeshName].Specular[instanceProperties[renderer.MeshName].iter] = renderer.mr_Specular;
			instanceProperties[renderer.MeshName].Shininess[instanceProperties[renderer.MeshName].iter] = renderer.mr_Shininess;
			instanceProperties[renderer.MeshName].entitySRT[instanceProperties[renderer.MeshName].iter] = transform.GetWorldMatrix();


			++(instanceProperties[renderer.MeshName].iter);
			
			// sub meshes into instance properties
			char maxcount = 32;
			for (char namecount = 0; namecount < maxcount; ++namecount)
			{
				std::string newName = renderer.MeshName;

				newName += ('1' + namecount);

				if (instanceProperties.find(newName) == instanceProperties.end())
				{
					break;
				}
				//InstanceinstanceProperties* currentProp = &instanceProperties[renderer.MeshName];

				/*GLuint textureID = 0;
				GLuint normalMapID = 0;*/

				instanceProperties[newName].entitySRT[instanceProperties[newName].iter] = transform.GetWorldMatrix();
				instanceProperties[newName].Albedo[instanceProperties[newName].iter] = renderer.mr_Albedo;
				instanceProperties[newName].Ambient[instanceProperties[newName].iter] = renderer.mr_Ambient;
				instanceProperties[newName].Diffuse[instanceProperties[newName].iter] = renderer.mr_Diffuse;
				instanceProperties[newName].Specular[instanceProperties[newName].iter] = renderer.mr_Specular;
				instanceProperties[newName].Shininess[instanceProperties[newName].iter] = renderer.mr_Shininess;

				instanceProperties[newName].M_R_A_Texture[instanceProperties[newName].iter] = glm::vec4(metalidx, roughidx, aoidx, emissionidx);
				instanceProperties[newName].M_R_A_Constant[instanceProperties[newName].iter] = glm::vec3(metal_constant, rough_constant, ao_constant);

				++(instanceProperties[newName].iter);
			}
		}
		else /*if default rendering*/{
			// if not instance put into container for default rendering
			unsigned int iter = 0;
			for (unsigned int vao : t_Mesh->Vaoids) {
				DefaultRenderProperties renderProperties;
				renderProperties.VAO = vao;
				renderProperties.drawType = t_Mesh->prim;
				renderProperties.drawCount = t_Mesh->Drawcounts[iter++];
				renderProperties.entitySRT = transform.GetWorldMatrix();
				renderProperties.Albedo = renderer.mr_Albedo;
				renderProperties.Ambient = renderer.mr_Ambient;
				renderProperties.Diffuse = renderer.mr_Diffuse;
				renderProperties.Specular = renderer.mr_Specular;
				renderProperties.Shininess = renderer.mr_Shininess;
				renderProperties.textureID = renderer.textureID;
				renderProperties.NormalID = renderer.normalMapID;
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
	float spacing = 100.f;
	float length = _num * spacing * 0.5f;

	instanceProperties["Line"].iter = _num * 2;

	for (int i = 0; i < _num; i++)
	{
		glm::mat4 scalMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(length));
		glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 transMatrixZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.0f, (i * spacing) - length));
		glm::mat4 transMatrixX = glm::translate(glm::mat4(1.0f), glm::vec3((i * spacing) - length, 0.0f, 0.f));

		instanceProperties["Line"].entitySRT[i] = transMatrixZ * scalMatrix; // z axis
		instanceProperties["Line"].entitySRT[i + _num] = transMatrixX * rotMatrix * scalMatrix; // x axis
	}
}

void Renderer::Draw()
{
	
	// Looping Properties for instancing
	for (auto& [name, prop] : instanceProperties)
	{
		/*for (size_t i = 0; i < 32; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texIndex[i]);
		}*/
		glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
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
		DrawMeshes(prop.VAO, prop.iter, prop.drawCount, prop.drawType, LIGHTING.GetLight());

		// FOR DEBUG DRAW
		if (EditorScene::Instance().DebugDraw())
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
		}

		prop.iter = 0;
	}

	// for default render
	for (DefaultRenderProperties& prop : defaultProperties) {

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, prop.textureID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, prop.NormalID);


		GLSLShader& shader =  SHADER.GetShader(DEFAULT);
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

		shader.UnUse();
	}
}

void Renderer::DrawMeshes(const GLuint& _vaoid, const unsigned int& _instanceCount,
	const unsigned int& _primCount, GLenum _primType, const LightProperties& _lightSource)
{
	//testBox.instanceDraw(EntityRenderLimit);

	glEnable(GL_DEPTH_TEST); // might be sus to place this here

	GLSLShader& shader = SHADER.GetShader(PBR);
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

	GLSLShader& shader = SHADER.GetShader(TDR);
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

	GLSLShader& shader = SHADER.GetShader(TDR);
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

unsigned int Renderer::ReturnTextureIdx(const std::string& _meshName, const GLuint& _id)
{
	if (!_id)
	{
		return 33;
	}
	for (unsigned int iter = 0; iter < instanceProperties[_meshName].textureCount + 1; ++iter)
	{
		if (instanceProperties[_meshName].texture[iter] == 0)
		{
			instanceProperties[_meshName].texture[iter] = _id;
			instanceProperties[_meshName].textureCount++;
			return iter;
		}
		if (instanceProperties[_meshName].texture[iter] == _id)
		{
			instanceProperties[_meshName].textureCount++;
			return iter;
		}
	}
	return 33;
}

void Renderer::Exit()
{

}