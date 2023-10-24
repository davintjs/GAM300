#pragma once
#ifndef SHADERFILEPARSER_H
#define SHADERFILEPARSER_H

#include <string>

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
};


#endif 
