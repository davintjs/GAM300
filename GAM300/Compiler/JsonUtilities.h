#ifndef JSONUTILITIES_H
#define JSONUTILITIES_H
#pragma once

#include <string>

// Rapidjson
#include <rapidjson/rapidjson/document.h>
#include <rapidjson/rapidjson/prettywriter.h>

using CSTR = const char*;

void SetJsonString(const std::string& str, rapidjson::Value& value, rapidjson::Document& document);
void SetJsonString(const char* str, rapidjson::Value& value, rapidjson::Document& document);

// Serialization
// Basic Types
template<typename T>
void SerializeBasic(const T& data, rapidjson::Value& value, rapidjson::Document& document, const std::string& key) {
	value.AddMember(key.c_str(), data, document.GetAllocator());
}
template<>
void SerializeBasic<std::string>(const std::string& data, rapidjson::Value& value, rapidjson::Document& document, const std::string& key);
template<>
void SerializeBasic<CSTR>(const CSTR& data, rapidjson::Value& value, rapidjson::Document& document, const std::string& key);

// Deserialization
// Basic Types
template<typename T>
bool Deserialize(T& _data, rapidjson::Value& _value, const std::string& _key)
{
	static_assert(true);
	return false;
}
template<>
bool Deserialize<bool>(bool& _data, rapidjson::Value& _value, const std::string& _key);
template<>
bool Deserialize<int>(int& _data, rapidjson::Value& _value, const std::string& _key);
template<>
bool Deserialize<int64_t>(int64_t& _data, rapidjson::Value& _value, const std::string& _key);
template<>
bool Deserialize<unsigned int>(unsigned int& _data, rapidjson::Value& _value, const std::string& _key);
template<>
bool Deserialize<uint64_t>(uint64_t& _data, rapidjson::Value& _value, const std::string& _key);
template<>
bool Deserialize<float>(float& _data, rapidjson::Value& _value, const std::string& _key);
template<>
bool Deserialize<double>(double& _data, rapidjson::Value& _value, const std::string& _key);
template<>
bool Deserialize<std::string>(std::string& _data, rapidjson::Value& _value, const std::string& _key);


#endif // !JSONUTILITIES_H

