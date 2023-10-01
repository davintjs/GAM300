/*!***************************************************************************************
\file			ScriptFields.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			01/09/2023

\brief
	This file holds the definition of script fields

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#ifndef  SCRIPT_FIELDS_H
#define SCRIPT_FIELDS_H

#include <string>

struct Field
{
	Field() = default;
	void* data{ nullptr };
	size_t fType{};
	std::string typeName;
	/***************************************************************************/
	/*!
	\brief
		Stores data of a given buffer to prevent out of scope destruction.
		Aka assigns memory from the heap
	\param _size
		Size of buffer
	\param _data
		Data to store and copy from
	*/
	/**************************************************************************/
	Field(size_t _fType, void* _data) :
		fType{ _fType }, data{ _data }{}
	template<typename T>
	void operator=(const T& val)
	{
		memcpy(data, &val, sizeof(T));
	}
	template<typename T>
	T& Get()
	{
		return *static_cast<T*>(data);
	}
	/***************************************************************************/
	/*!
	\brief
		Copy constructor

	\param rhs
		Field to store and copy from
	*/
	/**************************************************************************/
	Field(const Field& rhs)
	{
		if (data)
			delete[] data;
		data = rhs.data;
		fType = rhs.fType;
		typeName = rhs.typeName;
	}
	Field(Field&& rhs) noexcept
	{
		data = rhs.data;
		fType = rhs.fType;
		typeName = std::move(rhs.typeName);
		rhs.data = nullptr;
	}
};
#endif // ! SCRIPT_FIELDS_H