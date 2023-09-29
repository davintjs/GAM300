/*!***************************************************************************************
\file			Scene.h
\project
\author         

\par			Course: GAM300
\date           07/09/2023

\brief
	This file contains the declarations of the following:
	1. Scene

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#ifndef SCENE_H
#define SCENE_H

#include <filesystem>
#include <string>
#include <bitset>
#include "Core/Debug.h"
#include "Entity.h"
#include "Components.h"
#include <unordered_map>
#include "Editor/EditorHeaders.h"
#include "Core/EventsManager.h"

using SingleObjectTypes = decltype(TemplatePack<Entity>::Concatenate(SingleComponentTypes()));

template <typename T>
using Table = std::unordered_map<Engine::UUID,T*>;

template<typename... Ts>
struct SingleHandlesTable
{
	constexpr SingleHandlesTable(TemplatePack<Ts...>) {}
	SingleHandlesTable() = default;
	template <typename T1>
	bool Has(Engine::UUID euid, Engine::UUID uuid = 0)
	{
		(void)uuid;
		auto& entries = std::get<Table<T1>>(tables);
		if (entries.find(euid) == entries.end())
			return false;
		return true;
	}

	template <typename T1>
	bool Has(T1& object)
	{
		return Has<T1>(object.EUID());
	}

	template <typename T1>
	constexpr T1& Get(Engine::UUID euid)
	{
		return *std::get<Table<T1>>(tables)[euid];
	}

	template <typename T1>
	constexpr void Remove(Engine::UUID euid)
	{
		auto& entries = std::get<Table<T1>>(tables);
		entries.erase(euid);
	}

	template <typename T1,typename... Args>
	constexpr T1* emplace(T1* object, Engine::UUID euid)
	{
		auto& table = std::get<Table<T1>>(tables);
		auto pair = table.emplace(std::make_pair(euid, object));
		return pair.first->second;
	}
private:
	std::tuple<Table<Ts>...> tables;
};

template <typename T>
using MultiTable = std::unordered_map<Engine::UUID,Table<T>>;

template<typename... Ts>
struct MultiHandlesTable
{
	constexpr MultiHandlesTable(TemplatePack<Ts...>) {}
	MultiHandlesTable() = default;
	template <typename T1>
	bool Has(Engine::UUID euid, Engine::UUID uuid)
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

	template <typename T1>
	bool Has(T1& object)
	{
		return Has<T1>(object.EUID(),object.UUID());
	}

	template <typename T1>
	constexpr T1& Get(Engine::UUID euid, Engine::UUID uuid)
	{
		return *std::get<MultiTable<T1>>(tables)[euid][uuid];
	}

	template <typename T1>
	constexpr std::vector<T1*> Get(Engine::UUID euid)
	{
		auto& entries = std::get<MultiTable<T1>>(tables);
		std::vector<T1*> arr;
		for (auto& pair : entries[euid])
		{
			arr.push_back(pair.second);
		}
		return arr;
	}

	template <typename T1>
	constexpr void Remove(Engine::UUID euid, Engine::UUID uuid)
	{
		(void)uuid;
		auto& entries = std::get<MultiTable<T1>>(tables);
		entries[euid].erase(euid);
		if (entries[euid].size() == 0)
			entries.erase(euid);
	}

	template <typename T1, typename... Args>
	constexpr T1* emplace(T1* object)
	{
		auto& entries = std::get<MultiTable<T1>>(tables);
		auto& subTable = entries[object->EUID()];
		auto pair = subTable.emplace(std::make_pair(object->UUID(),object));
		return pair.first->second;
	}
private:
	std::tuple<MultiTable<Ts>...> tables;
};

using SingleHandles = decltype(SingleHandlesTable(SingleObjectTypes()));
using MultiHandles = decltype(MultiHandlesTable(MultiComponentTypes()));



using GetType = decltype(GetTypeGroup(AllObjectTypes()));

template<typename T, typename... Ts>
static void RegisterComponentsHelper()
{
	ComponentTypes.emplace(GetType::Name<T>(), GetType::E<T>());
	if constexpr (sizeof...(Ts) != 0)
	{
		RegisterComponentsHelper<Ts...>();
	}
}

template<typename... Ts>
static void RegisterComponents(TemplatePack<Ts...>)
{
	RegisterComponentsHelper<Ts...>();
}

using EntitiesList = ObjectsList<Entity, MAX_ENTITIES>;

using EntitiesPtrArray = std::vector<Entity*>;

struct Scene
{
	enum class State : char 
	{
		Edit = 0,
		Play,
		Paused
	};

	std::string sceneName;

private:
	EntitiesList entities;	//Vector should be in order
	SingleComponentsArrays singleComponentsArrays;
	MultiComponentsArrays multiComponentsArrays;
	EntitiesPtrArray entitiesDeletionBuffer;
	ComponentsBufferArray componentsDeletionBuffer;
	SingleHandles singleHandles;
	MultiHandles multiHandles;
public:

	std::filesystem::path filePath;
	using Layer = std::list<Engine::UUID>;
	const Engine::UUID uuid = Engine::CreateUUID();

	Layer layer;

	Scene(const std::string& _filepath);

	template <typename T,typename... Ts>
	void CloneHelper(Scene& rhs)
	{
		for (T& object : rhs.GetArray<T>())
		{
			if constexpr (!std::is_same_v<T, Transform> && !std::is_same_v<T, Tag>)
			{
				T* pObject = Add<T>(object.euid, object.uuid);
				*pObject = object;
				if (!rhs.IsActive(object))
				{
					SetActive(*pObject, false);
				}
			}
			else
			{
				T& obj = Get<T>(object.euid);
				obj = object;
			}
		
		}
		if constexpr (sizeof...(Ts) != 0)
		{
			CloneHelper<Ts...>(rhs);
		}
	}

	template <typename T, typename... Ts>
	void CloneHelper(Scene& rhs,TemplatePack<T,Ts...>)
	{
		CloneHelper<T,Ts...>(rhs);
		//CloneLinkHelper<Ts...>(rhs);
	}


	Scene(Scene& rhs) : sceneName{rhs.sceneName}
	{
		CloneHelper(rhs, AllObjectTypes());
	} 

	Scene& operator=(Scene&) = delete;

	template<typename T>
	T& Get(Engine::UUID euid, Engine::UUID uuid = 0)
	{
		(void)uuid;
		//E_ASSERT
		//(
		//	handles.Has<T>(euid),
		//	"Entity euid: ", euid, " of ", typeid(T).name() + strlen("struct "), " doesn't exist in this scene"
		//);

		if constexpr (SingleObjectTypes::Has<T>())
		{
			return singleHandles.Get<T>(euid);
		}
		else if constexpr (MultiComponentTypes::Has<T>())
		{
			return *GetMulti<T>(euid).front();
		}
	}

	template<typename T>
	T& Get(Handle& handle)
	{
		return Get<T>(handle.euid, handle.uuid);
	}

	template<typename T, typename Owner>
	T& Get(Owner& object)
	{
		if constexpr (MultiComponentTypes::Has<T>())
		{
			return *GetMulti<T>(object.euid).front();
		}
		else
		{
			return Get<T>(object.euid);
		}
	}

	GENERIC_RECURSIVE(void*, Get, &Get<T>(((Object*)pObject)->EUID()));

	GENERIC_RECURSIVE(void*, GetByUUID, &Get<T>(*(Handle*)pObject));

	template<typename T>
	std::vector<T*> GetMulti(Engine::UUID euid)
	{
		static_assert(MultiComponentTypes::Has<T>(), "Type is not a multi component");
		return multiHandles.Get<T>(euid);
	}

	template<typename T, typename Owner>
	std::vector<T*> GetMulti(Owner& object)
	{
		static_assert(MultiComponentTypes::Has<T>(), "Type is not a multi component");
		return GetMulti<T>(object.euid);
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
			if (scene.Has<T1>(entity))
			{
				if constexpr (SingleComponentTypes::Has<T1>())
				{
					scene.Destroy(scene.Get<T1>(entity));
				}
				else if constexpr (MultiComponentTypes::Has<T1>())
				{
					for (T1* pComponent : scene.GetMulti<T1>(entity.euid))
					{
						scene.Destroy(*pComponent);
					}
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
			entities.SetActive((ObjectIndex)object.uuid,false);
			layer.erase(std::find(layer.begin(), layer.end(), object.euid));
		}
		else if constexpr (SingleComponentTypes::Has<T>())
		{
			componentsDeletionBuffer.GetArray<T>().push_back(&object);
			auto& arr = singleComponentsArrays.GetArray<T>();
			ObjectIndex index = arr.GetDenseIndex(object);
			arr.SetActive(index,false);
			entities.DenseSubscript(index).hasComponentsBitset.set(GetType::E<T>(), false);
		}
		else if constexpr (MultiComponentTypes::Has<T>())
		{
			componentsDeletionBuffer.GetArray<T>().push_back(&object);
			auto& arr = multiComponentsArrays.GetArray<T>();
			ObjectIndex index = arr.GetDenseIndex(object);
			arr.SetActive(object, false);
			if (arr.DenseSubscript(index).size() == 1)
				entities.DenseSubscript(index).hasComponentsBitset.set(GetType::E<T>(), false);
		}
		else
		{
			static_assert(true, "Not a valid type of object to destroy");
		}
		EraseHandle(object);
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
					compArray.erase(*pComponent);
			}
			else if constexpr (MultiComponentTypes::Has<T1>())
			{
				auto& compArray = scene.multiComponentsArrays.GetArray<T1>();
				for (T1* pComponent : arr)
					compArray.erase(*pComponent);
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
			DestroyEntityComponents(*this,*pEntity);
			entities.erase(*pEntity);
		}
		entitiesDeletionBuffer.clear();
		//Destroy components
		ClearBufferHelper(*this);
	}

	template <typename T>
	auto& GetArray()
	{
		if constexpr (std::is_same_v<T, Entity>)
		{
			return entities;
		}
		else if constexpr (SingleComponentTypes::Has<T>())
		{
			return singleComponentsArrays.GetArray<T>();
		}
		else if constexpr (MultiComponentTypes::Has<T>())
		{
			return multiComponentsArrays.GetArray<T>();
		}
	}

	template <typename T>
	bool IsActive(T& object)
	{
		static_assert(AllObjectTypes::Has<T>(), "Type is not a valid scene object");
		auto& arr = GetArray<T>();
		if constexpr (std::is_same_v<T, Entity>)
		{
			return arr.IsActiveDense(arr.GetDenseIndex(object));
		}
		else if constexpr (SingleComponentTypes::Has<T>())
		{
			return arr.IsActiveDense(arr.GetDenseIndex(object));
		}
		else if constexpr (MultiComponentTypes::Has<T>())
		{
			return true;
		}
	}

	template <typename T>
	void SetActive(T& object, bool val = true)
	{
		GetArray<T>().SetActive(object, val);
	}


	template <typename Component>
	bool Has(const Entity& entity)
	{
		if constexpr (AllComponentTypes::Has<Component>())
		{
			return entity.hasComponentsBitset.test(GetType::E<Component>());
		}
		else
		{
			return false;
		}
	}

	template <typename T>
	bool HasHandle(const Handle& handle)
	{
		if constexpr (SingleObjectTypes::Has<T>())
		{
			return singleHandles.Has<T>(handle.euid);
		}
		
		if constexpr (MultiComponentTypes::Has<T>())
		{
			return multiHandles.Has<T>(handle.euid,handle.uuid);
		}

	}

	template <typename T>
	void EraseHandle(T& object)
	{
		if constexpr (SingleObjectTypes::Has<T>())
		{
			return singleHandles.Remove<T>(object.euid);
		}
		else if constexpr (MultiComponentTypes::Has<T>())
		{
			return multiHandles.Remove<T>(object.euid, object.uuid);
		}
	}

	GENERIC_RECURSIVE(bool, HasHandle, HasHandle<T>(*(Handle*)pObject));

	template <typename T, typename Owner, typename... Args>
	T* Add(const Owner& owner,Args&&... args)
	{
		return Add<T>(owner.EUID(), Engine::CreateUUID(), args...);
	}

	template <typename T, typename... Args>
	T* Add
	(
		Engine::UUID euid = Engine::CreateUUID(), 
		Engine::UUID uuid = Engine::CreateUUID(),
		Args&&... args
	)
	{
		static_assert(AllObjectTypes::Has<T>(),"Type is not a valid scene object");
		auto& arr = GetArray<T>();
		T* object{ nullptr };
		//TRY CATCH HERE IN CASE WE CANT ADD
		if constexpr (std::is_same_v<T, Entity>)
		{
			//while (singleHandles.Has<Entity>(euid) || euid == 0)
			//{
			//	euid = Engine::CreateUUID();
			//}
			object = &arr.emplace_back(args...);
			object->euid = euid;
			object->uuid = arr.GetDenseIndex(*object);
			arr.SetActive((ObjectIndex)object->uuid);
			singleHandles.emplace(object, object->euid);
			Add<Transform>(*object);
			Tag* tag = Add<Tag>(*object);
			tag->name = "New GameObject(";
			tag->name += std::to_string(arr.size());
			tag->name += ")";
			layer.push_back(euid);
			// Add the entity to the inspector
		}
		else if constexpr (AllComponentTypes::Has<T>())
		{
			Entity& entity{ Get<Entity>(euid) };
			object = &arr.emplace((ObjectIndex)entity.uuid,args...);
			object->euid = euid;
			object->uuid = uuid;
			if constexpr (SingleComponentTypes::Has<T>())
			{
				singleHandles.emplace(object, object->euid);
			}
			else
			{
				multiHandles.emplace(object);
			}
			entity.hasComponentsBitset.set(GetType::E<T>(), true);
			arr.SetActive(*object);
		}
		if (object)
		{
			ObjectCreatedEvent e = {object};
			EVENTS.Publish(&e);
		}
		return object;
	}
};
#endif SCENE_H