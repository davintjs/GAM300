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
	for (auto& [name, prop] : properties)
	{
		std::fill_n(prop.textureIndex, EnitityInstanceLimit, glm::vec2(0.f));
		std::fill_n(prop.M_R_A_Texture, EnitityInstanceLimit, glm::vec3(33.f));
		std::fill_n(prop.texture, 32, 0);
	}

	Scene& currentScene = SceneManager::Instance().GetCurrentScene();

	int i = 0;

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
		//InstanceProperties* currentProp = &properties[renderer.MeshName];

		//std::string textureGUID = AssetManager::Instance().GetAssetGUID(renderer.AlbedoTexture); // problem eh
		// use bool to see if texture exist instead...

		float texidx = float(ReturnTextureIdx(renderer.MeshName, renderer.textureID));
		float normidx = float(ReturnTextureIdx(renderer.MeshName, renderer.normalMapID));


		float metalidx = float(ReturnTextureIdx(renderer.MeshName, renderer.MetallicID));
		float roughidx = float(ReturnTextureIdx(renderer.MeshName, renderer.RoughnessID));
		float aoidx = float(ReturnTextureIdx(renderer.MeshName, renderer.AoID));


		float metal_constant = renderer.mr_metallic;
		float rough_constant = renderer.mr_roughness;
		float ao_constant = renderer.ao;
		properties[renderer.MeshName].M_R_A_Constant[properties[renderer.MeshName].iter] = glm::vec3(metal_constant, rough_constant, ao_constant);


		//// button here change norm idx to 33
		//if (test_button_1)
		//{
		//	normidx = 33;
		//	
		//}
		//if (test_button_2)
		//{
		//	roughidx = 33;
		//}

		properties[renderer.MeshName].textureIndex[properties[renderer.MeshName].iter] = glm::vec2(texidx, normidx);
		properties[renderer.MeshName].M_R_A_Texture[properties[renderer.MeshName].iter] = glm::vec3(metalidx, roughidx, aoidx);

		properties[renderer.MeshName].Albedo[properties[renderer.MeshName].iter] = renderer.mr_Albedo;
		properties[renderer.MeshName].Ambient[properties[renderer.MeshName].iter] = renderer.mr_Ambient;
		properties[renderer.MeshName].Diffuse[properties[renderer.MeshName].iter] = renderer.mr_Diffuse;
		properties[renderer.MeshName].Specular[properties[renderer.MeshName].iter] = renderer.mr_Specular;
		properties[renderer.MeshName].Shininess[properties[renderer.MeshName].iter] = renderer.mr_Shininess;
		properties[renderer.MeshName].entitySRT[properties[renderer.MeshName].iter] = transform.GetWorldMatrix();


		++(properties[renderer.MeshName].iter);
		char maxcount = 32;
		// newstring
		for (char namecount = 0; namecount < maxcount; ++namecount)
		{
			std::string newName = renderer.MeshName;

			newName += ('1' + namecount);

			if (properties.find(newName) == properties.end())
			{
				break;
			}
			//InstanceProperties* currentProp = &properties[renderer.MeshName];

			/*GLuint textureID = 0;
			GLuint normalMapID = 0;*/

			properties[newName].entitySRT[properties[newName].iter] = transform.GetWorldMatrix();
			properties[newName].Albedo[properties[newName].iter] = renderer.mr_Albedo;
			properties[newName].Ambient[properties[newName].iter] = renderer.mr_Ambient;
			properties[newName].Diffuse[properties[newName].iter] = renderer.mr_Diffuse;
			properties[newName].Specular[properties[newName].iter] = renderer.mr_Specular;
			properties[newName].Shininess[properties[newName].iter] = renderer.mr_Shininess;

			properties[newName].M_R_A_Texture[properties[newName].iter] = glm::vec3(metalidx, roughidx, aoidx);
			properties[newName].M_R_A_Constant[properties[newName].iter] = glm::vec3(metal_constant, rough_constant, ao_constant);
			properties[newName].Specular[properties[newName].iter] = renderer.mr_Specular;
			properties[newName].Shininess[properties[newName].iter] = renderer.mr_Shininess;


			++(properties[newName].iter);
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
			Ray3D* temp = DEBUGDRAW.GetRay();
			if (testRayOBB(temp->origin, temp->direction, mins, maxs,
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

	properties["Line"].iter = _num * 2;

	for (int i = 0; i < _num; i++)
	{
		glm::mat4 scalMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(length));
		glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 transMatrixZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.0f, (i * spacing) - length));
		glm::mat4 transMatrixX = glm::translate(glm::mat4(1.0f), glm::vec3((i * spacing) - length, 0.0f, 0.f));

		properties["Line"].entitySRT[i] = transMatrixZ * scalMatrix; // z axis
		properties["Line"].entitySRT[i + _num] = transMatrixX * rotMatrix * scalMatrix; // x axis
	}
}

void Renderer::Draw()
{
	// Looping Properties
	for (auto& [name, prop] : properties)
	{
		/*for (size_t i = 0; i < 32; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texIndex[i]);
		}*/
		glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//glBindBuffer(GL_ARRAY_BUFFER, prop.entityMATbuffer);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(Materials), &(prop.entityMAT[0]));
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, prop.AlbedoBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.Albedo[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//glBindBuffer(GL_ARRAY_BUFFER, prop.SpecularBuffer);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.Specular[0]));
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//
		//glBindBuffer(GL_ARRAY_BUFFER, prop.DiffuseBuffer);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.Diffuse[0]));
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//
		//glBindBuffer(GL_ARRAY_BUFFER, prop.AmbientBuffer);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec4), &(prop.Ambient[0]));
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//
		//glBindBuffer(GL_ARRAY_BUFFER, prop.ShininessBuffer);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(float), &(prop.Shininess[0]));
		//glBindBuffer(GL_ARRAY_BUFFER, 0);




		glBindBuffer(GL_ARRAY_BUFFER, prop.Metal_Rough_AO_Texture_Buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec3), &(prop.M_R_A_Texture[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, prop.Metal_Rough_AO_Texture_Constant);
		glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec3), &(prop.M_R_A_Constant[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, prop.textureIndexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, EnitityInstanceLimit * sizeof(glm::vec2), &(prop.textureIndex[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//std::cout <<  " r" << prop.entityMAT[0].Albedo.r << "\n";
		//std::cout <<  " g" << prop.entityMAT[0].Albedo.g << "\n";
		//std::cout <<  " b" << prop.entityMAT[0].Albedo.b << "\n";
		//std::cout <<  " a" << prop.entityMAT[0].Albedo.a << "\n";

		//std::cout <<  " a" << temp_AlbedoContainer[3].r << "\n";
		for (int i = 0; i < 32; ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, prop.texture[i]);
		}
		DrawMeshes(prop.VAO, prop.iter, prop.drawCount, prop.drawType, LIGHTING.GetLight());
		//temp_AlbedoContainer[3], temp_SpecularContainer[3], temp_DiffuseContainer[3], temp_AmbientContainer[3], temp_ShininessContainer[3]);

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
		glm::value_ptr(EditorCam.getPerspMatrix()));
	glUniformMatrix4fv(uniform2, 1, GL_FALSE,
		glm::value_ptr(EditorCam.getViewMatrix()));
	glUniform3fv(uniform3, 1,
		glm::value_ptr(_lightSource.lightColor));
	glUniform3fv(uniform4, 1,
		glm::value_ptr(_lightSource.lightpos));
	glUniform3fv(uniform5, 1,
		glm::value_ptr(EditorCam.GetCameraPosition()));

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
		glm::value_ptr(EditorCam.getPerspMatrix()));
	glUniformMatrix4fv(uniform2, 1, GL_FALSE,
		glm::value_ptr(EditorCam.getViewMatrix()));
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
		glm::value_ptr(EditorCam.getPerspMatrix()));
	glUniformMatrix4fv(uniform2, 1, GL_FALSE,
		glm::value_ptr(EditorCam.getViewMatrix()));
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
	for (unsigned int iter = 0; iter < properties[_meshName].textureCount + 1; ++iter)
	{
		if (properties[_meshName].texture[iter] == 0)
		{
			properties[_meshName].texture[iter] = _id;
			properties[_meshName].textureCount++;
			return iter;
		}
		if (properties[_meshName].texture[iter] == _id)
		{
			properties[_meshName].textureCount++;
			return iter;
		}
	}
	return 33;
}

void Renderer::Exit()
{

}