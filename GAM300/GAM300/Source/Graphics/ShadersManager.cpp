/*!***************************************************************************************
\file			ShadersManager.cpp
\project
\author         

\par			Course: GAM300
\date           10/10/2023

\brief
	This file contains the definitions of Shader Manager that includes:
	1.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "GraphicsHeaders.h"

const std::string shaderPath = "GAM300/Shaders";

void ShaderManager::Init()
{
	// Loaded in sequence, changing it will break, check GraphicsHeader.h for the shadertype sequence
	// Bean: A temporary solution for now
	std::string vertexPath = shaderPath + "/HDR.vert";
	std::string fragmentPath = shaderPath + "/HDR.frag";
	ShaderCompiler(vertexPath, fragmentPath, "HDR SHADER");

	vertexPath = shaderPath + "/PBR.vert";
	fragmentPath = shaderPath + "/PBR.frag";
	ShaderCompiler(vertexPath, fragmentPath, "PBR SHADER");

	vertexPath = shaderPath + "/InstancedRender.vert";
	fragmentPath = shaderPath + "/InstancedRender.frag";
	ShaderCompiler(vertexPath, fragmentPath, "TIR SHADER");

	vertexPath = shaderPath + "/InstancedDebugRender.vert";
	fragmentPath = shaderPath + "/InstancedDebugRender.frag";
	ShaderCompiler(vertexPath, fragmentPath, "TBR SHADER");

	vertexPath = shaderPath + "/Skybox.vert";
	fragmentPath = shaderPath + "/Skybox.frag";
	ShaderCompiler(vertexPath, fragmentPath, "Skybox SHADER");

	vertexPath = shaderPath + "/BasicLighting.vert";
	fragmentPath = shaderPath + "/BasicLighting.frag";
	ShaderCompiler(vertexPath, fragmentPath, "BasicLighting SHADER");

	vertexPath = shaderPath + "/LightAffected.vert";
	fragmentPath = shaderPath + "/LightAffected.frag";
	ShaderCompiler(vertexPath, fragmentPath, "LightAffected SHADER");
}

void ShaderManager::Update(float)
{

}

void ShaderManager::Exit()
{

}

void ShaderManager::ShaderCompiler(const std::string& _vertPath, const std::string& _fragPath, const std::string& _name)
{
	GLSLShader shader;
	std::vector<std::pair<GLenum, std::string>> shdr_files;
	// Vertex Shader
	shdr_files.emplace_back(std::make_pair(
		GL_VERTEX_SHADER,
		_vertPath));

	// Fragment Shader
	shdr_files.emplace_back(std::make_pair(
		GL_FRAGMENT_SHADER,
		_fragPath));

	PRINT(_name, '\n');
	shader.CompileLinkValidate(shdr_files);
	PRINT(_name, "\n\n");


	// if linking failed
	if (GL_FALSE == shader.IsLinked())
	{
		std::stringstream sstr;
		sstr << "Unable to compile/link/validate shader programs\n";
		sstr << shader.GetLog() << "\n";
		PRINT(sstr.str());
		std::exit(EXIT_FAILURE);
	}

	shaders.push_back(shader);
}