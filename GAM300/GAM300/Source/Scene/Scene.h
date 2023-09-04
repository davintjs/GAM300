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
#include "Editor/EditorHeaders.h"


struct Scene
{
	EntitiesList entities;	//Vector should be in order
	SingleComponentsArrays singleComponentsArrays;
	MultiComponentsArrays multiComponentsArrays;

	EntitiesPtrArray entitiesDeletionBuffer;
	ComponentsBufferArray componentsDeletionBuffer;

	enum class State : char 
	{
		Edit = 0,
		Play,
		Paused
	};
	Scene(const std::string& _filepath);

	Scene(Scene&) = delete;
	Scene& operator=(Scene&) = delete;


	Entity& AddEntity(Engine::UUID uuid = Engine::CreateUUID())
	{
		Entity& entity = entities.emplace_back(uuid);
		entity.pScene = this;
		entity.denseIndex = entities.GetDenseIndex(entity);
		entities.SetActive(entity.denseIndex);
		AddComponent<Transform>(entity);
		Tag& tag = AddComponent<Tag>(entity);
		tag.name = "New GameObject(";
		tag.name += std::to_string(entities.size());
		tag.name += ")";
		EditorDebugger::Instance().Debug_Sys.AddLog("[%i]{Entity}New Entity Created!\n", EditorDebugger::Instance().debugcounter++);
		return entity;
	}

	template<typename T>
	void Destroy(T& object)
	{
		if constexpr (std::is_same<T, Entity>())
		{
			entitiesDeletionBuffer.push_back(&object);
			entities.SetActive(object.denseIndex,false);
		}
		else if constexpr (SingleComponentTypes::Has<T>())
		{
			componentsDeletionBuffer.GetArray<T>().push_back(&object);
			auto& arr = singleComponentsArrays.GetArray<T>();
			arr.SetActive(arr.GetDenseIndex(object),false);
		}
		else if constexpr (MultiComponentTypes::Has<T>())
		{
			componentsDeletionBuffer.GetArray<T>().push_back(&object);
			auto& arr = multiComponentsArrays.GetArray<T>();
			arr.SetActive(object, false);
		}
		static_assert(true,"Not a valid type of object to destroy");
	}

	template <typename T, typename... Ts>
	struct ClearBufferStruct
	{
		ClearBufferStruct(TemplatePack<T, Ts...> pack) {}
		ClearBufferStruct(Scene& _scene) : scene{ _scene }
		{
			CleanComponents<T, Ts...>();
		}
		Scene& scene;
		template <typename T1, typename... T1s>
		void CleanComponents()
		{
			auto& arr = scene.componentsDeletionBuffer.GetArray<T1>();
			if constexpr (SingleComponentTypes::Has<T1>())
			{
				auto& compArray = scene.singleComponentsArrays.GetArray<T1>();
				for (T1* pComponent : arr)
				{
					compArray.erase(*pComponent);
				}
			}
			else if constexpr (MultiComponentTypes::Has<T1>())
			{
				auto& compArray = scene.multiComponentsArrays.GetArray<T1>();
				for (T1* pComponent : arr)
				{
					compArray.erase(*pComponent);
				}
			}
			arr.clear();
			if constexpr (sizeof...(T1s) != 0)
			{
				CleanComponents<T1s...>();
			}
		}
	};

	using ClearBufferHelper = decltype(ClearBufferStruct(AllComponentTypes()));

	void ClearBuffer()
	{
		for (Entity* pEntity : entitiesDeletionBuffer)
		{
			entities.erase(*pEntity);
		}
		entitiesDeletionBuffer.clear();

		ClearBufferHelper(*this);
	}

	template <typename Component>
	bool ComponentIsEnabled(uint32_t index, size_t multiIndex);


	template <typename Component>
	void ComponentSetEnabled(uint32_t index,bool value, size_t multiIndex = 0);

	template <typename Component>
	Component& AddComponent(const Entity& entity)
	{
		return AddComponent<Component>(entity.denseIndex);
	}

	template <typename Component>
	Component& AddComponent(uint32_t index)
	{
		if constexpr (SingleComponentTypes::Has<Component>())
		{
			auto& arr = singleComponentsArrays.GetArray<Component>();
			Component& component = arr.emplace(index);
			arr.SetActive(index);
			return component;
		}
		else if constexpr (MultiComponentTypes::Has<Component>())
		{
			auto& arr = multiComponentsArrays.GetArray<Component>();
			Component& component = arr.emplace(index);
			arr.SetActive(component);
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
bool Scene::ComponentIsEnabled(uint32_t index, size_t multiIndex)
{
	if constexpr (SingleComponentTypes::Has<Component>())
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
void Scene::ComponentSetEnabled(uint32_t index, bool value, size_t multiIndex)
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