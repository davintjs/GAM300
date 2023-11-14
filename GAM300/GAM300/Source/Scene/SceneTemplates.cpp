/*!***************************************************************************************
\file			SceneTemplates.cpp
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			10/08/2023

\brief
	This file defines all the template functions used by Scene.h

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/


#include "Scene.h"

template <typename... Ts>
void Scene::StoreComponentHierarchy(ReferencesTable& storage, Engine::UUID entityID, Engine::UUID newEntityID, TemplatePack<Ts...>)
{
	StoreComponentHierarchy<Ts...>(storage, entityID, newEntityID);
}

template <typename T,typename... Ts>
void Scene::StoreComponentHierarchy(ReferencesTable& storage, Engine::UUID entityID, Engine::UUID newEntityID)
{
	Entity& entity{ Get<Entity>(entityID) };

	if (Has<T>(entity))
	{
		if constexpr (SingleComponentTypes::Has<T>())
		{
			T& component{ Get<T>(entityID) };
			T* object;
			if constexpr (std::is_same<T, Transform>())
			{
				object = &Get<T>(newEntityID);
				object->child.clear();
			}
			else if constexpr (std::is_same<T, Tag>())
			{
				object = &Get<T>(newEntityID);
			}
			else
			{
				object = Add<T>(newEntityID);
			}
			storage[GetType::E<T>()][component] = *object;
			CopyValues(component, *object);
			if constexpr (std::is_same<T, Tag>())
			{
				object->name += " - Copy";
			}
		}
		else
		{
			for (T* pComponent : GetMulti<T>(entityID))
			{
				
				T* object;
				if constexpr (std::is_same_v<T, Script>)
				{
					object = Add<T>(Get<Entity>(newEntityID), pComponent->scriptId);
				}
				else
				{
					object = Add<T>(newEntityID);
				}
				storage[GetType::E<T>()][*pComponent] = *object;
				CopyValues(*pComponent, *object);
			}
		}
	}

	if constexpr (sizeof...(Ts) != 0)
	{
		StoreComponentHierarchy<Ts...>(storage, entityID, newEntityID);
	}
}

template <typename T>
void Scene::CopyValues(T& source, T& dest)
{
	property::SerializeEnum(source, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type Flags)
	{
		auto entry = property::entry { PropertyName, Data };

		property::set(dest, entry.first.c_str(), Data);
		// If we are dealing with a scope that is not an array someone may have change the SerializeEnum to a DisplayEnum they only show up there.
		//assert(Flags.m_isScope == false || PropertyName.back() == ']');

	});
}

template <typename... Ts>
void Scene::LinkReferences(ReferencesTable& storage, TemplatePack<Ts...>)
{
	LinkReferences<Ts...>(storage);
}

template <typename T, typename... Ts>
void Scene::LinkReferences(ReferencesTable& storage)
{
	for (auto& old_new : storage[GetType::E<T>()])
	{
		T& newObject = Get<T>(old_new.second);

		if constexpr (std::is_same<T, Transform>())
		{
			if (newObject.parent)
			{
				for (auto& pair : storage[GetType::E<T>()])
				{
					if (pair.first.euid == newObject.parent)
					{
						newObject.parent = pair.second.euid;
						Get<Transform>(newObject.parent).child.push_back(newObject.EUID());
					}
				}
			}
		}
		else if constexpr (std::is_same<T, Script>())
		{
			static char buffer[2048]{};
			T& oldObject = Get<T>(old_new.first);
			ScriptGetFieldNamesEvent getFieldNamesEvent{ oldObject };
			EVENTS.Publish(&getFieldNamesEvent);
			for (size_t i = 0; i < getFieldNamesEvent.count; ++i)
			{
				const char* fieldName = getFieldNamesEvent.pStart[i];
				Field field{ AllFieldTypes::Size(),2048, buffer };
				ScriptGetFieldEvent getFieldEvent{ oldObject,fieldName,field };
				EVENTS.Publish(&getFieldEvent);
				//Objects
				if (field.fType < AllObjectTypes::Size())
				{

					Object*& pObject = *(Object**)field.data;
					if (field.fType < MultiComponentTypes::Size())
					{
						if (storage[field.fType].contains(*pObject))
						{
							Handle& newHandle = storage[field.fType][*pObject];
							pObject = (Object*)GetByHandle(field.fType, &newHandle);
							//Set to internal linkage in game object
							//handle = storage[field.fType][handle];
						}
					}
					else
					{
						for (auto& pair : storage[field.fType])
						{
							if (pair.first.euid == pObject->euid)
							{
								Handle& newHandle = storage[field.fType][*pObject];
								pObject = (Object*)GetByHandle(field.fType, &newHandle);
								break;
							}
						}
					}
				}
				ScriptSetFieldEvent setFieldEvent{ newObject,fieldName,field };
				EVENTS.Publish(&setFieldEvent);
			}
		}
	}

	if constexpr (sizeof...(Ts) != 0)
	{
		LinkReferences<Ts...>(storage);
	}
}


template<typename T>
T& Scene::GetByUUID(Engine::UUID uuid)
{
	if constexpr (MultiComponentTypes::Has<T>())
	{
		return multiHandles.GetByUUID<T>(uuid);
	}
	else
	{
		return singleHandles.GetByUUID<T>(uuid);
	}
}

template<typename T>
T& Scene::Get(Engine::UUID euid, Engine::UUID uuid)
{
	(void)uuid;
	if constexpr (SingleObjectTypes::Has<T>())
	{
		return singleHandles.Get<T>(euid);
	}
	else if constexpr (MultiComponentTypes::Has<T>())
	{
		return multiHandles.Get<T>(euid, uuid);
	}
}


template<typename T>
T& Scene::Get(Handle& handle)
{
	return Get<T>(handle.euid, handle.uuid);
}

template<typename T, typename Owner>
T& Scene::Get(Owner& object)
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


template<typename T>
std::vector<T*> Scene::GetMulti(Engine::UUID euid)
{
	static_assert(MultiComponentTypes::Has<T>(), "Type is not a multi component");
	return multiHandles.Get<T>(euid);
}


template<typename T, typename Owner>
std::vector<T*> Scene::GetMulti(Owner& object)
{
	static_assert(MultiComponentTypes::Has<T>(), "Type is not a multi component");
	return GetMulti<T>(object.euid);
}

template<typename T>
void Scene::Destroy(T& object)
{
	if constexpr (std::is_same<T, Entity>())
	{
		entitiesDeletionBuffer.push_back(&object);
		entities.SetActive((ObjectIndex)object.uuid, false);
		
		Transform& transform = Get<Transform>(object);
		transform.SetParent(nullptr);
		auto children{ transform.child };

		for (auto& child : children)
		{
			Destroy<Entity>(Get<Entity>(child));
		}
		DestroyEntityComponents(*this, object);
	}
	else if constexpr (SingleComponentTypes::Has<T>())
	{
		componentsDeletionBuffer.GetArray<T>().push_back(&object);
		auto& arr = singleComponentsArrays.GetArray<T>();
		ObjectIndex index = arr.GetDenseIndex(object);
		arr.SetActive(index, false);
		entities.DenseSubscript(index).hasComponentsBitset.set(GetType::E<T>(), false);
	}
	else if constexpr (MultiComponentTypes::Has<T>())
	{
		componentsDeletionBuffer.GetArray<T>().push_back(&object);
		auto& arr = multiComponentsArrays.GetArray<T>();
		arr.SetActive(object, false);
		if (multiHandles.Get<T>(object.euid).size() == 1)
			Get<Entity>(object).hasComponentsBitset.set(GetType::E<T>(), false);
	}
	else
	{
		static_assert(true, "Not a valid type of object to destroy");
	}
	ObjectDestroyedEvent e(&object);
	EVENTS.Publish(&e);
	EraseHandle(object);
}


template <typename T>
auto& Scene::GetArray()
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

template <typename T, typename... Ts>
void Scene::CloneHelper(Scene& rhs)
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
			Engine::UUID id = obj.UUID();
			obj = object;
			obj.uuid = id;
		}

	}
	if constexpr (sizeof...(Ts) != 0)
	{
		CloneHelper<Ts...>(rhs);
	}
}

template <typename T, typename... Ts>
void Scene::CloneHelper(Scene& rhs, TemplatePack<T, Ts...>)
{
	CloneHelper<T, Ts...>(rhs);
}

template <typename T>
bool Scene::IsActive(T& object)
{
	static_assert(AllObjectTypes::Has<T>(), "Type is not a valid scene object");
	auto& arr = GetArray<T>();
	if constexpr (std::is_same_v<T, Entity>)
	{
		bool isActive = arr.IsActiveDense(arr.GetDenseIndex(object));
		Transform& t = Get<Transform>(object);

		if (isActive && t.parent)
		{
			Entity& parentEntity = Get<Entity>(t.parent);
			return IsActive(parentEntity);
		}

		return isActive;
	}
	else if constexpr (SingleComponentTypes::Has<T>())
	{
		return arr.IsActiveDense(arr.GetDenseIndex(object));
	}
	else if constexpr (MultiComponentTypes::Has<T>())
	{
		for (auto it = arr.begin(); it != arr.end(); ++it)
		{
			if (&(*it) == &object)
				return it.IsActive();
		}
		E_ASSERT(false, "Multicomponent cannot be found");
	}
}

template <typename T>
void Scene::SetActive(T& object, bool val)
{
	GetArray<T>().SetActive(object, val);
}


//Checks if an object is active
template <typename T>
bool Scene::IsActive(const Handle& handle)
{
	return IsActive(Get<T>(handle));
}


//Sets active for an object
template <typename T>
void Scene::SetActive(const Handle& handle, bool val)
{
	SetActive(Get<T>(handle), val);
}

template <typename Component>
bool Scene::Has(const Entity& entity)
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
bool Scene::HasHandle(const Handle& handle)
{
	if constexpr (SingleObjectTypes::Has<T>())
	{
		return singleHandles.Has<T>(handle.euid);
	}

	if constexpr (MultiComponentTypes::Has<T>())
	{
		return multiHandles.Has<T>(handle.euid, handle.uuid);
	}

}

template <typename T>
void Scene::EraseHandle(T& object)
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

template <typename T, typename Owner, typename... Args>
T* Scene::Add(const Owner& owner, Args&&... args)
{
	return Add<T>(owner.EUID(), Engine::CreateUUID(), args...);
}

template <typename T, typename... Args>
T* Scene::Add
(
	Engine::UUID euid,
	Engine::UUID uuid,
	Args&&... args
)
{
	static_assert(AllObjectTypes::Has<T>(), "Type is not a valid scene object");
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
		//Entities UUID is a dense Index
		object = &arr.emplace((ObjectIndex)entity.uuid, args...);
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
		ObjectCreatedEvent e = { object };
		EVENTS.Publish(&e);
	}
	object->state = NORMAL;
	return object;
}


template<typename T, typename... Ts>
Scene::DestroyComponentsGroup<T, Ts...>::DestroyComponentsGroup(Scene& _scene, Entity& _entity) : scene{_scene}, entity { _entity }
{
	DestroyComponents<T, Ts...>();
}

template<typename T, typename... Ts>
template <typename T1, typename... T1s>
void Scene::DestroyComponentsGroup<T, Ts...>::DestroyComponents()
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

template <typename T, typename... Ts>
Scene::ClearBufferStruct<T,Ts...>::ClearBufferStruct(Scene& _scene) : scene{ _scene }
{
	CleanComponents<T, Ts...>();
}

template <typename T, typename... Ts>
template <typename T1, typename... T1s>
void Scene::ClearBufferStruct<T, Ts...>::CleanComponents()
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
			compArray.erase(*pComponent);
	}
	arr.clear();
	if constexpr (sizeof...(T1s) != 0)
	{
		CleanComponents<T1s...>();
	}
}