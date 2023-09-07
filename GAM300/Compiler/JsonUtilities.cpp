#include "JsonUtilities.h"

void SetJsonString(const std::string& str, rapidjson::Value& value, rapidjson::Document& document) {
	value.SetString(str.c_str(), static_cast<rapidjson::SizeType>(str.length()), document.GetAllocator());
}
void SetJsonString(const char* str, rapidjson::Value& value, rapidjson::Document& document) {
	value.SetString(str, static_cast<rapidjson::SizeType>(strlen(str)), document.GetAllocator());
}

template<>
void SerializeBasic<std::string>(const std::string& data, rapidjson::Value& value, rapidjson::Document& document, const std::string& key) {
	rapidjson::Value k;
	SetJsonString(key, k, document);
	rapidjson::Value d;
	SetJsonString(data, d, document);
	value.AddMember(k, d, document.GetAllocator());
}
template<>
void SerializeBasic<CSTR>(const CSTR& data, rapidjson::Value& value, rapidjson::Document& document, const std::string& key) {
	rapidjson::Value k;
	SetJsonString(key, k, document);
	rapidjson::Value d;
	SetJsonString(data, d, document);
	value.AddMember(k, d, document.GetAllocator());
}

template<>
bool Deserialize<bool>(bool& _data, rapidjson::Value& _value, const std::string& _key)
{
	if (!_value.HasMember(_key.c_str()))
		return false;

	_data = _value[_key.c_str()].GetBool();
	return true;
}
template<>
bool Deserialize<int>(int& _data, rapidjson::Value& _value, const std::string& _key)
{
	if (!_value.HasMember(_key.c_str()))
		return false;

	_data = _value[_key.c_str()].GetInt();
	return true;
}
template<>
bool Deserialize<int64_t>(int64_t& _data, rapidjson::Value& _value, const std::string& _key)
{
	if (!_value.HasMember(_key.c_str()))
		return false;

	_data = _value[_key.c_str()].GetInt64();
	return true;
}
template<>
bool Deserialize<unsigned int>(unsigned int& _data, rapidjson::Value& _value, const std::string& _key)
{
	if (!_value.HasMember(_key.c_str()))
		return false;

	_data = _value[_key.c_str()].GetUint();
	return true;
}
template<>
bool Deserialize<uint64_t>(uint64_t& _data, rapidjson::Value& _value, const std::string& _key)
{
	if (!_value.HasMember(_key.c_str()))
		return false;

	_data = _value[_key.c_str()].GetUint64();
	return true;
}
template<>
bool Deserialize<float>(float& _data, rapidjson::Value& _value, const std::string& _key)
{
	if (!_value.HasMember(_key.c_str()))
		return false;

	_data = _value[_key.c_str()].GetFloat();
	return true;
}
template<>
bool Deserialize<double>(double& _data, rapidjson::Value& _value, const std::string& _key)
{
	if (!_value.HasMember(_key.c_str()))
		return false;

	_data = _value[_key.c_str()].GetDouble();
	return true;
}
template<>
bool Deserialize<std::string>(std::string& _data, rapidjson::Value& _value, const std::string& _key)
{
	if (!_value.HasMember(_key.c_str()))
		return false;

	_data = _value[_key.c_str()].GetString();
	return true;
}
