﻿/*!***************************************************************************************
\file			scene.h
\project
\author			Matthew Lau

\par			Course: GAM200
\par			Section:
\date			28/07/2022

\brief
	Contains declarations for the Scene class.
	The Scene contains:
		1. load, init, update, draw, free, unload function
		2. string containing the filename of the file in which the scene data is stored on
		3. Data pertaining to the game objects in the scene

	Note: load, init, free and unload functions MUST be defined by scene sub-classes

	Contains definitions for NormalScene class which is a derived class from Scene class.
	Note: this is the latest version of our scene class, use this


All content � 2022 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/


#ifndef SCENE_H
#define SCENE_H

#include <filesystem>
#include <string>
#include <bitset>
#include "Core/Debug.h"
#include "Entity.h"
#include "Components.h"


struct Scene
{
	EntitiesList entities;	//Vector should be in order
	SingleComponentsArrays singleComponentsArrays;
	MultiComponentsArrays multiComponentsArrays;

	enum class State : char 
	{
		Edit = 0,
		Play,
		Paused
	};
	Scene(const std::string& _filepath);

	Scene(Scene&) = delete;
	Scene& operator=(Scene&) = delete;


	Entity& AddEntity(UUID uuid = CreateUUID())
	{
		Entity& entity = entities.emplace_back(uuid);
		entity.pScene = this;
		entity.denseIndex = entities.GetDenseIndex(entity);
		AddComponent<Transform>(entity);
		return entity;
	}

	bool EntityIsActive(DenseIndex index);

	void EntitySetActive(DenseIndex index,bool value);

	template <typename Component>
	bool ComponentIsEnabled(DenseIndex index, size_t multiIndex);


	template <typename Component>
	void ComponentSetEnabled(DenseIndex index,bool value, size_t multiIndex = 0);

	template <typename Component>
	Component& AddComponent(const Entity& entity)
	{
		return AddComponent<Component>(entity.denseIndex);
	}

	template <typename Component>
	void RemoveComponent(Entity& entity,Component& component)
	{
		//When removing a component, disable the object first
		if constexpr (SingleComponentTypes::Has<Component>())
		{
			singleComponentsArrays.GetArray<Component>().erase(component);
		}
		else if constexpr (MultiComponentTypes::Has<Component>())
		{
			MultiComponentsArray<Component>& arr = multiComponentsArrays.GetArray<Component>();
			arr.DenseSubscript(entity.denseIndex).erase(component);
		}
		else
		{
			static_assert(true, "Type is not a valid component!");
		}
	}

	template <typename Component>
	Component& AddComponent(DenseIndex index)
	{
		if constexpr (SingleComponentTypes::Has<Component>())
		{
			return singleComponentsArrays.GetArray<Component>().emplace(index);
		}
		else if constexpr (MultiComponentTypes::Has<Component>())
		{
			return multiComponentsArrays.GetArray<Component>().emplace(index);
		}
		else
		{
			static_assert(true, "Type is not a valid component!");
		}
	}

	std::filesystem::path filePath;
	State state;
};

template <typename Component>
bool Scene::ComponentIsEnabled(DenseIndex index, size_t multiIndex)
{
	if constexpr (SingleComponentTypes::Has(Component))
	{
		ASSERT(multiIndex == 0);//, "Unable to find another component of given type as only one should exist on this gameObject");
		singleComponentsArrays.GetArray<Component>().GetActive(index);
	}
	else if constexpr (MultiComponentTypes::Has(Component))
	{
		multiComponentsArrays.GetArray<Component>().GetActive(index);
	}
	static_assert(true, "Type is not a valid component!");
}

template <typename Component>
void Scene::ComponentSetEnabled(DenseIndex index, bool value, size_t multiIndex)
{
	if constexpr (SingleComponentTypes::Has(Component))
	{
		ASSERT(multiIndex == 0, "Unable to find another component of given type as only one should exist on this gameObject");
		singleComponentsArrays.GetArray<Component>().SetActive(index, value);
	}
	else if constexpr (MultiComponentTypes::Has(Component))
	{
		multiComponentsArrays.GetArray<Component>().SetActive(index, value);
	}
	static_assert(true, "Type is not a valid component!");
}
#endif SCENE_H