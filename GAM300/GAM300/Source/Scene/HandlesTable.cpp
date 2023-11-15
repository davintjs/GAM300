/*!***************************************************************************************
\file			HandlesTable.cpp
\project
\author         Zacharie Hong

\par			Course: GAM300
\date           25/09/2023

\brief
	This file contains the definitions of the following:
	1. Tables for single and multi components to search up for the pointer to the object

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/


#include "HandlesTable.h"

#define SINGLE_HANDLE SingleHandlesTable<Ts...>
#define MULTI_HANDLE MultiHandlesTable<Ts...>

#pragma region SINGLE
template<typename... Ts>
template <typename T1>
bool SINGLE_HANDLE::Has(Engine::UUID euid, Engine::UUID uuid)
{
	(void)uuid;
	auto& entries = std::get<Table<T1>>(tables);
	return (entries.find(euid) != entries.end());
}

template<typename... Ts>
template <typename T1>
bool SINGLE_HANDLE::Has(T1& object)
{
	return Has<T1>(object.EUID());
}

template<typename... Ts>
template <typename T1>
T1& SINGLE_HANDLE::Get(Engine::UUID euid)
{
	return *std::get<Table<T1>>(tables)[euid];
}

template<typename... Ts>
template <typename T1>
T1& SINGLE_HANDLE::GetByUUID(Engine::UUID uuid)
{
	auto& entries = std::get<Table<T1>>(tables);
	for (auto& pair : entries)
	{
		if (pair.second->UUID() == uuid)
		{
			return *pair.second;
		}
	}
	E_ASSERT(false, "Could not find handle!");
}

template<typename... Ts>
template <typename T1>
void SINGLE_HANDLE::Remove(Engine::UUID euid)
{
	auto& entries = std::get<Table<T1>>(tables);
	entries.erase(euid);
}

template<typename... Ts>
template <typename T1, typename... Args>
T1* SINGLE_HANDLE::emplace(T1* object, Engine::UUID euid)
{
	auto& table = std::get<Table<T1>>(tables);
	auto pair = table.emplace(std::make_pair(euid, object));
	return pair.first->second;
}
#pragma endregion


#pragma region MULTI
template<typename... Ts>
template <typename T1>
bool MULTI_HANDLE::Has(Engine::UUID euid, Engine::UUID uuid)
{
	auto& entries = std::get<MultiTable<T1>>(tables);
	auto entIt = entries.find(euid);
	if (entIt == entries.end())
		return false;
	Table<T1>& subEntries = entIt->second;
	if (subEntries.find(uuid) == subEntries.end())
		return false;
	return true;
}

template<typename... Ts>
template <typename T1>
bool MULTI_HANDLE::Has(T1& object)
{
	return Has<T1>(object.EUID(), object.UUID());
}

template<typename... Ts>
template <typename T1>
T1& MULTI_HANDLE::Get(Engine::UUID euid, Engine::UUID uuid)
{
	return *std::get<MultiTable<T1>>(tables)[euid][uuid];
}


template<typename... Ts>
template <typename T1>
T1& MULTI_HANDLE::GetByUUID(Engine::UUID uuid)
{
	auto& entries = std::get<MultiTable<T1>>(tables);
	for (auto& entryPair : entries)
	{
		for (auto& pair : entryPair.second)
		{
			if (pair.first == uuid)
			{
				return *pair.second;
			}
		}
	}
	E_ASSERT(false, "Could not find handle!");
}

template<typename... Ts>
template <typename T1>
std::vector<T1*> MULTI_HANDLE::Get(Engine::UUID euid)
{
	auto& entries = std::get<MultiTable<T1>>(tables);
	std::vector<T1*> arr;
	for (auto& pair : entries[euid])
	{
		if (pair.second->state != DELETED)
			arr.push_back(pair.second);
	}
	return arr;
}

template<typename... Ts>
template <typename T1>
void MULTI_HANDLE::Remove(Engine::UUID euid, Engine::UUID uuid)
{
	auto& entries = std::get<MultiTable<T1>>(tables);
	entries[euid].erase(uuid);
	if (entries[euid].size() == 0)
		entries.erase(euid);
}

template<typename... Ts>
template <typename T1, typename... Args>
T1* MULTI_HANDLE::emplace(T1* object)
{
	auto& entries = std::get<MultiTable<T1>>(tables);
	auto& subTable = entries[object->EUID()];
	auto pair = subTable.emplace(std::make_pair(object->UUID(), object));
	return pair.first->second;
}
#pragma endregion