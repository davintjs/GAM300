#include "Precompiled.h"
#include "ShaderFileParser.h"
#include "Scripting/ScriptFields.h"

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

//template <typename T, typename... Ts>
//void CreateField(const char* name, size_t fieldEnum, std::unordered_map<std::string, Field>& shaderVariableMap) {
//
//	if (GetFieldType::E<T>() == fieldEnum) {
//
//		// Allocate static sized buffer
//		if constexpr (std::is_same_v<T, char*>) {
//
//
//
//
//		}
//		else {
//			shaderVariableMap.insert({name, Field(fieldEnum, sizeof(T))});
//		}
//	}
//	// If havent hit jackpot for field type, check next field type
//	if constexpr (sizeof...(Ts) != 0) {
//		CreateField<Ts...>(name, fieldEnum, shaderVariableMap);
//	}
//}



void ParseShaderFile(const std::string& fileName, bool frag) {
	
	std::cout << "Parsing shader file...\n";

	std::ifstream ifs;
	ifs.open(fileName);
	if (!ifs.is_open()) {
		std::cout << "Error opening file!\n";
		return;
	}

	std::vector<ShaderVariable> shaderVariables;

	// Parse each line
	std::string buffer;
	while (std::getline(ifs, buffer)) {

		std::string vtBuffer;

		std::cout << "Line:" << buffer << std::endl;

		// Delimiter line to make parsing faster?
		if (buffer.find("//End") != std::string::npos) {
			std::cout << "Ending Parser...\n";
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

		if (buffer.substr(startPos, startPos - endPos).find("layout") == std::string::npos
			&& buffer.substr(startPos, startPos - endPos).find("uniform") == std::string::npos)
			continue;

		if (frag && buffer.substr(startPos, startPos - endPos).find("uniform") == std::string::npos)
			continue;

		startPos = endPos + 1;
		if (startPos >= superEnd)
			continue;

		// Stop parsing if not an input var
		startPos = buffer.find_first_of(')', startPos) + 2;
		endPos = buffer.find_first_of(' ', startPos);
		if (buffer.substr(startPos, startPos - endPos).find("in") == std::string::npos)
			continue;

		std::cout << "in!\n";

		startPos = endPos + 1;
		if (startPos >= superEnd)
			continue;


		std::string name;
		ShaderVariable::VariableType vt;

		// Parse Variable Type
		endPos = buffer.find_first_of(' ', startPos);
		if (endPos == std::string::npos) {
			continue;
		}
		else {
			vtBuffer = buffer.substr(startPos, endPos - startPos);
			vt = ParseVariableType(buffer.substr(startPos, startPos - endPos));
		}


		

		// Parse Variable Name
		name = buffer.substr(endPos + 1);
		//CreateField<FieldTypes>(name.c_str(), shaderFieldTypeMap[vtBuffer], testShaderFields);
		shaderVariables.emplace_back(ShaderVariable(name, vt));





		// variables.emplace_back(shaderFieldType[vtBuffer], shaderFieldType[vtBuffer]);
		// 
		// DisplayField(variableName, variables.back())


	}

	ifs.close();


	for (ShaderVariable& sv : shaderVariables) {
		std::cout << "Type:" << sv.variableType << " Name:" << sv.name << std::endl;

		// Make a new field in the new shader and assign proper values


	}

}

