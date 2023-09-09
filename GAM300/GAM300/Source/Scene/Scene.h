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


using EntitiesList = ObjectsList<Entity, MAX_ENTITIES>;

using EntitiesPtrArray = std::vector<Entity*>;

struct Scene
{
	std::string Scene_name;
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
		EditorDebugger::Instance().AddLog("[%i]{Entity}New Entity Created!\n", EditorDebugger::Instance().debugcounter++);
		EditorHierarchy::Instance().layer.push_back(&entity);
		return entity;
	}

	template<typename T, typename... Ts>
	struct DestroyComponentsGroup
	{
		Scene& scene;
		Entity& entity;
		DestroyComponentsGroup(TemplatePack<T, Ts...> pack) {}
		DestroyComponentsGroup(Scene& _scene, Entity& _entity) : scene{ _scene }, entity{_entity}
		{
			DestroyComponents<T, Ts...>();
		}

		template <typename T1, typename... T1s>
		void DestroyComponents()
		{
			if (scene.HasComponent<T1>(entity))
			{
				if constexpr (SingleComponentTypes::Has<T1>())
				{
					scene.singleComponentsArrays.GetArray<T1>().TryErase(entity.denseIndex);
				}
				else if constexpr (MultiComponentTypes::Has<T1>())
				{
					scene.multiComponentsArrays.GetArray<T1>().erase(entity.denseIndex);
				}
			}
			if constexpr (sizeof...(T1s) != 0)
			{
				DestroyComponents<T1s...>();
			}
		}
	};

	using DestroyEntityComponents = decltype(DestroyComponentsGroup((AllComponentTypes())));

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
			ObjectIndex index = arr.GetDenseIndex(object);
			arr.SetActive(index,false);
			entities.DenseSubscript(index).hasComponentsBitset.set(GetComponentType::E<T>(), false);
		}
		else if constexpr (MultiComponentTypes::Has<T>())
		{
			componentsDeletionBuffer.GetArray<T>().push_back(&object);
			auto& arr = multiComponentsArrays.GetArray<T>();
			ObjectIndex index = arr.GetDenseIndex(object);
			arr.SetActive(object, false);
			if (arr.DenseSubscript(index).size() == 1)
				entities.DenseSubscript(index).hasComponentsBitset.set(GetComponentType::E<T>(), false);
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
		//Destroy components
		ClearBufferHelper(*this);
		for (Entity* pEntity : entitiesDeletionBuffer)
		{
			DestroyEntityComponents(*this,*pEntity);
			entities.erase(*pEntity);
		}
		entitiesDeletionBuffer.clear();
	}

	template <typename Component>
	bool ComponentIsEnabled(uint32_t index, size_t multiIndex);

	template <typename Component>
	auto& GetComponentsArray()
	{
		if constexpr (SingleComponentTypes::Has<Component>())
		{
			return singleComponentsArrays.GetArray<Component>();
		}
		else if constexpr (MultiComponentTypes::Has<Component>())
		{
			return multiComponentsArrays.GetArray<Component>();
		}
	}

	template <typename Component>
	auto GetComponents(Entity& entity)
	{
		return multiComponentsArrays.GetArray<Component>().DenseSubscript(entity.denseIndex);
	}

	template <typename Component>
	auto& GetEntity(Component& component)
	{
		if constexpr (SingleComponentTypes::Has<Component>())
		{
			return entities.DenseSubscript(singleComponentsArrays.GetArray<Component>().GetDenseIndex(component));
		}
		else if constexpr (MultiComponentTypes::Has<Component>())
		{
			return entities.DenseSubscript(multiComponentsArrays.GetArray<Component>().GetDenseIndex(component));
		}
	}


	template <typename Component>
	void ComponentSetEnabled(uint32_t index,bool value, size_t multiIndex = 0);

	template <typename Component>
	Component& AddComponent(const Entity& entity)
	{
		return AddComponent<Component>(entity.denseIndex);
	}

	template <typename Component>
	bool HasComponent(const Entity& entity)
	{
		if constexpr (AllComponentTypes::Has<Component>())
		{
			return entity.hasComponentsBitset.test(GetComponentType::E<Component>());
		}
		return false;
	}

	template <typename Component>
	bool HasComponent(ObjectIndex& denseIndex)
	{
		if constexpr (AllComponentTypes::Has<Component>())
		{
			return entities.DenseSubscript(denseIndex).hasComponentsBitset.test(GetComponentType::E<Component>());
		}
		return false;
	}

	template <typename Component>
	Component& GetComponent(const Entity& entity)
	{
		//ASSERT(HasComponent<Component>(entity), "Entity does not have component");
		if constexpr (SingleComponentTypes::Has<Component>())
		{
			return singleComponentsArrays.GetArray<Component>().DenseSubscript(entity.denseIndex);
		}
		else if constexpr (MultiComponentTypes::Has<Component>())
		{
			return *multiComponentsArrays.GetArray<Component>().DenseSubscript(entity.denseIndex).front();
		}
	}

	template <typename Component>
	Component& AddComponent(uint32_t index)
	{
		static_assert(AllComponentTypes::Has<Component>(), "Type is not a valid component!");
		if constexpr (SingleComponentTypes::Has<Component>())
		{
			auto& arr = singleComponentsArrays.GetArray<Component>();
			Component& component = arr.emplace(index);
			entities.DenseSubscript(index).hasComponentsBitset.set(GetComponentType::E<Component>(), true);
			arr.SetActive(index);
			return component;
		}
		else if constexpr (MultiComponentTypes::Has<Component>())
		{
			auto& arr = multiComponentsArrays.GetArray<Component>();
			Component& component = arr.emplace(index);
			entities.DenseSubscript(index).hasComponentsBitset.set(GetComponentType::E<Component>(), true);
			arr.SetActive(component);
			return component;
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