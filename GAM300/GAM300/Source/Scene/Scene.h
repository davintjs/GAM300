/*!***************************************************************************************
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
	EntitiesArray entities;	//Vector should be in order
	SingleComponentsArrays singleComponentsArrays;
	MultiComponentsArrays multiComponentsArrays;
	EntitiesPtrArray entitiesToDelete;
	ComponentsPtrArrays componentsToDelete;

	enum class State : char 
	{
		Edit = 0,
		Play,
		Paused
	};
	Scene(const std::string& _filepath);

	Scene(Scene&) = delete;
	Scene& operator=(Scene&) = delete;
	/*******************************************************************************
	/*!
	*
	\brief
		LOAD function for the scene

	\return
		void
	*/
	/*******************************************************************************/
	void Load();
	/*******************************************************************************
	/*!
	*
	\brief
		INIT function for the scene
	/*******************************************************************************/
	void Init();
	/*******************************************************************************
	/*!
	*
	\brief
		UPDATE function for the scene
	/*******************************************************************************/
	void Update();
	/*******************************************************************************
	/*!
	*
	\brief
		DRAW function for the scene

	\return
		void
	*/
	/*******************************************************************************/
	void Draw();
	/*******************************************************************************
	/*!
	*
	\brief
		FREE function for the scene

	\return
		void
	*/
	/*******************************************************************************/
	void Free();
	/*******************************************************************************
	/*!
	*
	\brief
		UNLOAD function for the scene

	\return
		void
	*/
	/*******************************************************************************/
	void Unload();


	Entity& AddEntity()
	{
		//singleComponentsArrays.GetArray<Component>().emplace_back();
		return entities.emplace_back();
	}

	bool EntityIsActive(EntityIndex index);

	void EntitySetActive(EntityIndex index,bool value);

	template <typename Component>
	bool ComponentIsEnabled(EntityIndex index, size_t multiIndex);


	template <typename Component>
	void ComponentSetEnabled(EntityIndex index,bool value, size_t multiIndex = 0);

	template <typename Component>
	Component& AddComponent(EntityIndex index)
	{
		if constexpr (SingleComponentTypes::Has(Component))
		{
			Component& component = 
				singleComponentsArrays.GetArray<Component>().emplace_back();
			return component;
		}
		else if constexpr (MultiComponentTypes::Has(Component))
		{
			Component& component =
				multiComponentsArrays.GetArray<Component>().emplace_back();
			return component;
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
bool Scene::ComponentIsEnabled(EntityIndex index, size_t multiIndex)
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
void Scene::ComponentSetEnabled(EntityIndex index, bool value, size_t multiIndex)
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