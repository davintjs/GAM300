/*!***************************************************************************************
\file			HandlesTable.h
\project
\author         Zacharie Hong

\par			Course: GAM300
\date           25/09/2023

\brief
	This file contains the declarations of the following:
	1. Tables for single and multi components to search up for the pointer to the object

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#include "Components.h"
#include "Entity.h"
#include <unordered_map>

using SingleObjectTypes = decltype(TemplatePack<Entity>::Concatenate(SingleComponentTypes()));

//Hash table that maps an ID to a pointer
template <typename T>
using Table = std::unordered_map<Engine::UUID, T*>;

//Hash table of single component types
template<typename... Ts>
struct SingleHandlesTable
{
	//Template pack expansion for decltype
	constexpr SingleHandlesTable(TemplatePack<Ts...>) {}

	//Default constructor
	SingleHandlesTable() = default;

	//Whether the table has euid for this type
	template <typename T1>
	bool Has(Engine::UUID euid, Engine::UUID uuid = 0);

	//Whether the table has this object
	template <typename T1>
	bool Has(T1& object);

	//Get the pointer if it exists, will throw an error if it doesnt
	template <typename T1>
	T1& Get(Engine::UUID euid);

	//Get the pointer by uuid exists, will throw an error if it doesnt
	template <typename T1>
	T1& GetByUUID(Engine::UUID uuid);

	//Remove a entry by using euid
	template <typename T1>
	void Remove(Engine::UUID euid);

	//Adds an entry to the hash table
	template <typename T1, typename... Args>
	T1* emplace(T1* object, Engine::UUID euid);
private:
	std::tuple<Table<Ts>...> tables;
};

//Hash table that maps an ID to another Table, for a tree like behaviour
template <typename T>
using MultiTable = std::unordered_map<Engine::UUID, Table<T>>;

//Hash table of multi component types
template<typename... Ts>
struct MultiHandlesTable
{
	//Template pack expansion for decltype
	constexpr MultiHandlesTable(TemplatePack<Ts...>) {}

	//Default constructor
	MultiHandlesTable() = default;

	//Whether the table has euid and uuid for this type
	template <typename T1>
	bool Has(Engine::UUID euid, Engine::UUID uuid);

	//Whether the table has this object
	template <typename T1>
	bool Has(T1& object);

	//Get the pointer if it exists, will throw an error if it doesnt
	template <typename T1>
	T1& Get(Engine::UUID euid, Engine::UUID uuid);

	//Get the pointer by uuid exists, will throw an error if it doesnt
	template <typename T1>
	T1& GetByUUID(Engine::UUID uuid);

	template <typename T1>
	std::vector<T1*> Get(Engine::UUID euid);

	//Remove a entry by using euid and uuid
	template <typename T1>
	void Remove(Engine::UUID euid, Engine::UUID uuid);

	//Adds an entry to the hash table
	template <typename T1, typename... Args>
	T1* emplace(T1* object);
private:
	std::tuple<MultiTable<Ts>...> tables;
};

#include "HandlesTable.cpp"

using SingleHandles = decltype(SingleHandlesTable(SingleObjectTypes()));
using MultiHandles = decltype(MultiHandlesTable(MultiComponentTypes()));