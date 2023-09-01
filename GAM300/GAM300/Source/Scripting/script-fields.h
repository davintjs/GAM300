/*!***************************************************************************************
\file			script-fields.h
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			10/03/2023

\brief
	This file holds the definition of script fields, mainly allocated memory for
	storing any type of field

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#ifndef  SCRIPT_FIELDS_H
#define SCRIPT_FIELDS_H

#include <string>

enum class FieldType :int
{
	None, Float, Double,
	Bool, Char, Short, Int, Long,
	UShort, UInt, ULong, String,
	Vector2, Vector3, GameObject, Component
};

struct Field
{
	Field() = default;
	void* data{ nullptr };
	FieldType fType{};
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
	Field(FieldType _fType, size_t _size = 0, void* _data = nullptr) :
		fType{ _fType }
	{
		size = _size;
		if (size)
			data = new char[size];
		else
			data = nullptr;
		if (_data)
			memcpy(data, _data, size);
	}
	template<typename T>
	void operator=(const T& val)
	{
		COPIUM_ASSERT(sizeof(T) > size, "FIELD DOES NOT HAVE ENOUGH SPACE TO STORE TYPE");
		memcpy(data, &val, sizeof(T));
	}
	template<typename T>
	void operator=(const T* val)
	{
		memcpy(data, val, size);
	}
	template<typename T>
	T& Get()
	{
		COPIUM_ASSERT(sizeof(T) > size, "FIELD DOES NOT HAVE ENOUGH SPACE TO STORE TYPE");
		return *static_cast<T*>(data);
	}
	void Resize(size_t _size)
	{
		if (data)
			delete[] data;
		size = _size;
		data = new char[size];
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
		size = rhs.size;
		data = new char[size];
		fType = rhs.fType;
		typeName = rhs.typeName;
		memcpy(data, rhs.data, size);
	}
	Field(Field&& rhs)
	{
		size = rhs.size;
		data = rhs.data;
		fType = rhs.fType;
		typeName = std::move(rhs.typeName);
		rhs.data = nullptr;
	}
	Field& operator=(Field&& rhs)
	{
		size = rhs.size;
		data = rhs.data;
		fType = rhs.fType;
		typeName = std::move(rhs.typeName);
		rhs.data = nullptr;
		return *this;
	}
	/***************************************************************************/
	/*!
	\brief
		Destructor that frees memory
	*/
	/**************************************************************************/
	~Field()
	{
		if (data)
			delete[] data;
	}
	size_t GetSize() const
	{
		return size;
	}
private:
	size_t size{ 0 };
};
#endif // ! SCRIPT_FIELDS_H