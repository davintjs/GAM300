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
#include "Scene/Components.h"

const std::string shaderPath = "GAM300/Shaders";

// Map of all shader field types
static std::unordered_map<std::string, size_t> shaderFieldTypeMap =
{
	{ "float",						GetFieldType::E<float>()},
	{ "double",						GetFieldType::E<double>()},
	{ "bool",						GetFieldType::E<bool>()},
	{ "char",						GetFieldType::E<char>()},
	{ "short",						GetFieldType::E<short>()},
	{ "int",						GetFieldType::E<int>()},
	{ "int64",						GetFieldType::E<int64_t>()},
	{ "uint16_t",					GetFieldType::E<uint16_t>()},
	{ "uint32_t",					GetFieldType::E<uint32_t>()},
	{ "uint32_t",					GetFieldType::E<uint32_t>()},
	{ "char*",						GetFieldType::E<char*>()},
	{ "vec2",						GetFieldType::E<Vector2>()},
	{ "vec3",						GetFieldType::E<Vector3>()},
	{ "vec4",						GetFieldType::E<Vector4>()}
};

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
	CreateShaderProperties(fragmentPath, vertexPath);

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
	
	vertexPath = shaderPath + "/BloomBlur.vert";
	fragmentPath = shaderPath + "/BloomBlur.frag";
	ShaderCompiler("BLOOM BLUR SHADER", vertexPath, fragmentPath);

	vertexPath = shaderPath + "/DefaultGbuffer.vert";
	fragmentPath = shaderPath + "/DefaultGbuffer.frag";
	ShaderCompiler("GBUFFER SHADER", vertexPath, fragmentPath);

	vertexPath = shaderPath + "/DefaultShader.vert";
	fragmentPath = shaderPath + "/DefaultShader.frag";
	ShaderCompiler("Default SHADER", vertexPath, fragmentPath);

	vertexPath = shaderPath + "/ColourPicking.vert";
	fragmentPath = shaderPath + "/ColourPicking.frag";
	ShaderCompiler("Colour Picking SHADER", vertexPath, fragmentPath);
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


void ShaderManager::CreateShaderProperties(const std::string& _frag, const std::string& _vert) {

	// Create variables for fragment and vertex shaders from the frag and vert files respectively
	shaderProperties.emplace_back();
	ParseShaderFile(_frag, true);
	ParseShaderFile(_vert, false);

	for (ShaderProperties sp : shaderProperties) {

		std::cout << "Number of Fragment Variables:" << sp.fragmentVariables.size() << std::endl;
		for (size_t i = 0; i < sp.fragmentVariables.size(); ++i) {
			std::cout << "Name:" << sp.fragmentVariables[i].first << " TypeEnum:" << sp.fragmentVariables[i].second << std::endl;
		}

		std::cout << "Number of Vertex Variables:" << sp.vertexVariables.size() << std::endl;
		for (size_t i = 0; i < sp.vertexVariables.size(); ++i) {
			std::cout << "Name:" << sp.vertexVariables[i].first << " TypeEnum:" << sp.vertexVariables[i].second << std::endl;
		}

	}
}



void ShaderManager::ParseShaderFile(const std::string& fileName, bool frag) {

	std::cout << "Parsing shader file...\n";
	std::cout << "Test\n";
	std::ifstream ifs;
	ifs.open(fileName);
	if (!ifs.is_open()) {
		PRINT("Error opening Shader File!");
		return;
	}

	size_t idx = 0;

	// Parse each line
	std::string buffer;
	while (std::getline(ifs, buffer)) {

		std::string vtBuffer;

		//std::cout << "Line:" << buffer << std::endl;

		// Delimiter line to make parsing faster?
		if (buffer.find("//End") != std::string::npos) {
			//std::cout << "Ending Parser...\n";
			break;
		}

		if (buffer[0] == '\n')
			continue;

		// Skip if line is commented
		if (buffer[0] == '/' && buffer[1] == '/')
			continue;



		size_t superEnd = buffer.size() - 1;

		// Stop parsing if 1st word is not 'layout' or 'uniform'
		size_t startPos = 0;
		size_t endPos = buffer.find_first_of(' ');
		if (endPos == std::string::npos)
			continue;

		if (frag && buffer.substr(startPos, startPos - endPos).find("uniform") == std::string::npos)
			continue;
		//else {
		//	std::cout << "Line:" << buffer << std::endl;
		//}


		if (buffer.substr(startPos, startPos - endPos).find("layout") == std::string::npos
			&& buffer.substr(startPos, startPos - endPos).find("uniform") == std::string::npos)
			continue;



		startPos = endPos + 1;
		if (startPos >= superEnd)
			continue;

		// Stop parsing if not an input var
		startPos = buffer.find_first_of(')', startPos) + 2;
		endPos = buffer.find_first_of(' ', startPos);
		if (!frag && buffer.substr(startPos, endPos - startPos).find("in") == std::string::npos)
			continue;
		else {
			std::cout << buffer.substr(startPos, endPos - startPos) << std::endl;
		}

		startPos = endPos + 1;
		if (startPos >= superEnd)
			continue;


		std::string name;

		// Parse Variable Type
		endPos = buffer.find_first_of(' ', startPos);
		if (endPos == std::string::npos) {
			continue;
		}
		else {
			vtBuffer = buffer.substr(startPos, endPos - startPos);
			std::cout << vtBuffer << std::endl;
			if (!shaderFieldTypeMap.contains(vtBuffer)) {
				std::cout << "no such field:" << vtBuffer << std::endl;
				continue;
			}
		}


		// Parse Variable Name
		size_t semicolon = buffer.find_first_of(';', endPos);
		name = buffer.substr(endPos+1, semicolon-(endPos+1));

		// Create Shaderobject and place variable in correct container
		if (frag) {
			shaderProperties.back().fragmentVariables.insert({ idx, { name, shaderFieldTypeMap[vtBuffer] } });
		}
		else {
			shaderProperties.back().vertexVariables.insert({ idx, { name, shaderFieldTypeMap[vtBuffer] } });
		}

		++idx;

	}

	ifs.close();

}

