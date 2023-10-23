/*!***************************************************************************************
\file			ShadersManager.cpp
\project
\author         

\par			Course: GAM300
\date           10/10/2023

\brief
	This file contains the definitions of Shader Manager that includes:
	1.

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
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
	std::string geometryPath;
	ShaderCompiler("HDR SHADER", vertexPath, fragmentPath );

	vertexPath = shaderPath + "/PBR.vert";
	fragmentPath = shaderPath + "/PBR.frag";
	ShaderCompiler("PBR SHADER", vertexPath, fragmentPath);

	vertexPath = shaderPath + "/InstancedRender.vert";
	fragmentPath = shaderPath + "/InstancedRender.frag";
	ShaderCompiler("Temp Instance SHADER", vertexPath, fragmentPath);

	vertexPath = shaderPath + "/InstancedDebugRender.vert";
	fragmentPath = shaderPath + "/InstancedDebugRender.frag";
	ShaderCompiler("Temp Debug Instance SHADER", vertexPath, fragmentPath);

	vertexPath = shaderPath + "/Skybox.vert";
	fragmentPath = shaderPath + "/Skybox.frag";
	ShaderCompiler("Skybox SHADER", vertexPath, fragmentPath);

	vertexPath = shaderPath + "/BasicLighting.vert";
	fragmentPath = shaderPath + "/BasicLighting.frag";
	ShaderCompiler("BasicLighting SHADER", vertexPath, fragmentPath);

	vertexPath = shaderPath + "/LightAffected.vert";
	fragmentPath = shaderPath + "/LightAffected.frag";
	ShaderCompiler("LightAffected SHADER", vertexPath, fragmentPath);

	vertexPath = shaderPath + "/Shadow.vert";
	fragmentPath = shaderPath + "/Shadow.frag";
	ShaderCompiler("Shadow SHADER", vertexPath, fragmentPath);

	vertexPath = shaderPath + "/PointShadow.vert";
	fragmentPath = shaderPath + "/PointShadow.frag";
	geometryPath = shaderPath + "/PointShadow.geom";
	ShaderCompiler("Point SHADER", vertexPath, fragmentPath, geometryPath);

	vertexPath = shaderPath + "/UIScreenSpace.vert";
	fragmentPath = shaderPath + "/UIScreenSpace.frag";
	ShaderCompiler("UI SCREEN SHADER", vertexPath, fragmentPath);
	
	vertexPath = shaderPath + "/UIWorldSpace.vert";
	fragmentPath = shaderPath + "/UIWorldSpace.frag";
	ShaderCompiler("UI WORLD SHADER", vertexPath, fragmentPath);

	vertexPath = shaderPath + "/DefaultGbuffer.vert";
	fragmentPath = shaderPath + "/DefaultGbuffer.frag";
	ShaderCompiler("GBUFFER SHADER", vertexPath, fragmentPath);

	vertexPath = shaderPath + "/DefaultShader.vert";
	fragmentPath = shaderPath + "/DefaultShader.frag";
	ShaderCompiler("Default SHADER", vertexPath, fragmentPath);
}

void ShaderManager::Update(float)
{

}

void ShaderManager::Exit()
{

}

void ShaderManager::ShaderCompiler(const std::string& _name, const std::string& _vertPath, 
	const std::string& _fragPath, const std::string& _geometryPath)
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

	if (_geometryPath != "")
	{
		shdr_files.emplace_back(std::make_pair(
			GL_GEOMETRY_SHADER,
			_geometryPath));
	}
	else


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