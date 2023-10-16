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

unsigned int depthMapFBO; 
unsigned int depthMap; // Shadow Texture

//
unsigned int depthCubemapFBO;
unsigned int depthCubemap;

glm::mat4 lightSpaceMatrix;

LIGHT_TYPE temporary_test = POINT_LIGHT;

LightProperties spot_light_stuffs;
LightProperties directional_light_stuffs;
LightProperties point_light_stuffs;
	
const unsigned int SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;

void Renderer::Init()
{
	// This Framebuffer is used for both directional and spotlight
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "depth framebuffer exploded\n";
	else
		std::cout << "depth framebuffer created successfully\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// This Framebuffer is used for pointlight
	glGenFramebuffers(1, &depthCubemapFBO);
	// create depth cubemap texture
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthCubemapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::Update(float)
{
	for (auto& [name, prop] : properties)
	{
		std::fill_n(prop.textureIndex, EnitityInstanceLimit, glm::vec2(0.f));
		std::fill_n(prop.M_R_A_Texture, EnitityInstanceLimit, glm::vec4(33.f));
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
		float emissionidx = float(ReturnTextureIdx(renderer.MeshName, renderer.EmissionID));

		//std::cout << "metal index is : " << metalidx << "\n";
		//std::cout << "rough index is : " << roughidx << "\n";
		//std::cout << "ao index is : " << aoidx << "\n";
		//std::cout << "emission index is : " << emissionidx << "\n";
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
		properties[renderer.MeshName].M_R_A_Texture[properties[renderer.MeshName].iter] = glm::vec4(metalidx, roughidx, aoidx, emissionidx);

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

			properties[newName].M_R_A_Texture[properties[newName].iter] = glm::vec4(metalidx, roughidx, aoidx, emissionidx);
			properties[newName].M_R_A_Constant[properties[newName].iter] = glm::vec3(metal_constant, rough_constant, ao_constant);
			properties[newName].Specular[properties[newName].iter] = renderer.mr_Specular;
			properties[newName].Shininess[properties[newName].iter] = renderer.mr_Shininess;


			++(properties[newName].iter);
		}
		++i;
	}

	SetupGrid(100);

	DrawDepth();
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
		glActiveTexture(GL_TEXTURE0 + 31);
		glBindTexture(GL_TEXTURE_2D,depthMap);
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





	GLint uniform10 =
		glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix");

	glUniformMatrix4fv(uniform10, 1, GL_FALSE,
		glm::value_ptr(lightSpaceMatrix));








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

void Renderer::DrawDepth()
{
	glEnable(GL_DEPTH_TEST);
	//glm::vec3 lightPos(-2.0f, 4.0f, -1.0f); // This suppouse to be the actual light direction
	glm::vec3 lightPos(-0.2f, -1.0f, -0.3f); // This suppouse to be the actual light direction
	lightPos = -lightPos;
	glm::mat4 lightProjection, lightView;
	float near_plane = -10000.f, far_plane = 10000.f;

	// Above is directional light and spot light related stuffs
	// this vector is for point light
	std::vector<glm::mat4> shadowTransforms;


	if (temporary_test == DIRECTIONAL_LIGHT)
	{
		// Good Light pos {-0.2f, -1.0f, -0.3f}
		//lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightProjection = glm::ortho(-5000.f, 5000.f, -5000.f, 5000.f, near_plane, far_plane);
		lightView = glm::lookAt(-directional_light_stuffs.direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	}
	else if (temporary_test == SPOT_LIGHT)	
	{
		lightProjection = glm::perspective<float>(glm::radians(60.f), 1.f, 50.f, 1000.f);
		//lightView = glm::lookAt(spot_light_stuffs.lightpos, spot_light_stuffs.lightpos - spot_light_stuffs.direction, glm::vec3(0.0, 1.0, 0.0));
		lightView = glm::lookAt(spot_light_stuffs.lightpos, spot_light_stuffs.lightpos + (spot_light_stuffs.direction * 1000.f),
			glm::vec3(0.0, 0.0, 1.0));
	}
	else if (temporary_test == POINT_LIGHT)
	{
		near_plane = 1.0f;
		far_plane = 1000.f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	}

	lightSpaceMatrix = lightProjection * lightView;
	

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	for (auto& [name, prop] : properties)
	{
		glBindBuffer(GL_ARRAY_BUFFER, prop.entitySRTbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (EntityRenderLimit) * sizeof(glm::mat4), &(prop.entitySRT[0]));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (temporary_test == POINT_LIGHT)
		{
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthCubemapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);


			GLSLShader& shader = SHADER.GetShader(POINTSHADOW);
			shader.Use();
			for (int i = 0; i < 6; ++i)
			{
				std::string spot_pos;
				spot_pos = "shadowMatrices[" + std::to_string(i) + "]";
				glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), spot_pos.c_str())
					, 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));

			}

			GLint uniform1 =
				glGetUniformLocation(shader.GetHandle(), "lightPos");
			glUniform3fv(uniform1, 1,
				glm::value_ptr(point_light_stuffs.lightpos));

			GLint uniform2 =
				glGetUniformLocation(shader.GetHandle(), "far_plane");
			std::cout << "far plane is " << far_plane << "\n";
			glUniform1f(uniform2, far_plane);


			glBindVertexArray(prop.VAO);
			glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);
			glBindVertexArray(0);

			shader.UnUse();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}
		else
		{
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);

			GLSLShader& shader = SHADER.GetShader(SHADOW);
			shader.Use();


			GLint uniform1 =
				glGetUniformLocation(shader.GetHandle(), "lightSpaceMatrix");
		
			glUniformMatrix4fv(uniform1, 1, GL_FALSE,
				glm::value_ptr(lightSpaceMatrix));

			glBindVertexArray(prop.VAO);
			glDrawElementsInstanced(prop.drawType, prop.drawCount, GL_UNSIGNED_INT, 0, prop.iter);
			glBindVertexArray(0);

			shader.UnUse();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}

	}
	//glDisable(GL_CULL_FACE);

	// reset viewport
	glViewport(0, 0, 1600, 900);// screen width and screen height
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