#include "Precompiled.h"
#include "ShaderFileParser.h"

ShaderVariable::VariableType ParseVariableType(const std::string& str) {

	if (str.find("bool"))
		return ShaderVariable::Bool;
	else if (str.find("char"))
		return ShaderVariable::Char;
	else if (str.find("int"))
		return ShaderVariable::Int;
	else if (str.find("float"))
		return ShaderVariable::Float;
	else if (str.find("vec2"))
		return ShaderVariable::Vec2;
	else if (str.find("vec3"))
		return ShaderVariable::Vec3;
	else if (str.find("vec4"))
		return ShaderVariable::Vec4;
	else if (str.find("vec4ID"))
		return ShaderVariable::ID;


	return ShaderVariable::None;
}


void ParseShaderFile(const std::string& fileName) {
	
	std::ifstream ifs;
	ifs.open(fileName);
	if (!ifs.is_open())
		return;
	
	std::vector<ShaderVariable> shaderVariables;

	std::string buffer;
	while (std::getline(ifs, buffer)) {

		// Delimiter line to make parsing faster?
		if (buffer.find("//End"))
			break;


		std::string name;
		ShaderVariable::VariableType vt;

		size_t superEnd = buffer.size() - 1;

		// Stop parsing if 1st word is not 'layout' or 'uniform'
		size_t startPos = 0;
		size_t endPos = buffer.find_first_of(' ');
		if (!buffer.substr(startPos, startPos-endPos).find("layout")
			&& !buffer.substr(startPos, startPos - endPos).find("uniform"))
			continue;

		startPos = endPos + 1;
		if (startPos >= superEnd)
			continue;

		// Stop parsing if not an input var
		startPos = buffer.find_first_of(' ', startPos) + 1;
		endPos = buffer.find_first_of(' ', startPos);
		if (!buffer.substr(startPos, startPos - endPos).find("in"))
			continue;

		startPos = endPos + 1;
		if (startPos >= superEnd)
			continue;

		// Parse Variable Type
		endPos = buffer.find_first_of(' ', startPos);
		if (endPos == std::string::npos)
			vt = ShaderVariable::None;
		else {
			vt = ParseVariableType(buffer.substr(startPos, startPos - endPos));
		}

		// Parse Variable Name
		name = buffer.substr(endPos + 1);

		shaderVariables.emplace_back(ShaderVariable(name, vt));


	}

}

