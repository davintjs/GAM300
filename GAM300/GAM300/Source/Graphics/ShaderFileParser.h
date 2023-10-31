#pragma once
#ifndef SHADERFILEPARSER_H
#define SHADERFILEPARSER_H

#include <string>
#include <Scene/Components.h>

// Map of all the field types
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

static std::unordered_map<std::string, Field> testShaderFields;

void ParseShaderFile(const std::string& fileName, bool frag);

class ShaderVariable {
public:

	enum VariableType : char {
		None = 0,
		Char,
		Bool,
		Int,
		Float,
		Vec2,
		Vec3,
		Vec4,
		ID
	};

	ShaderVariable(const std::string& _name, ShaderVariable::VariableType _vt) : name{_name}, variableType{_vt}{}
	std::string name;
	VariableType variableType;
	size_t fieldEnum = 0;
};


#endif 
