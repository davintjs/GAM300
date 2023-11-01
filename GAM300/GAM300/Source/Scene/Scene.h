/*!***************************************************************************************
\file			Scene.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\date			10/08/2023

\brief
	This file declares all the functions used by Scene which functions as a ECS

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#ifndef SCENE_H
#define SCENE_H

#include <filesystem>
#include <string>
#include "Entity.h"
#include "Components.h"
#include "Editor/EditorHeaders.h"
#include "Core/EventsManager.h"
#include "HandlesTable.h"


struct Scene
{
	std::filesystem::path filePath;
	using Layer = std::list<Engine::UUID>;
	const Engine::UUID uuid = Engine::CreateUUID();
	Layer layer;
	std::vector<tag>Tags;
	std::string sceneName;

	//Creates empty scene
	Scene(const std::string& _filepath);

	//Copy assignment
	Scene& operator=(Scene& rhs);

	//Reads deletion buffer and deletes them from their arrays
	void ClearBuffer();

	//Get an object by euid and uuid(Multi components only)
	template<typename T>
	T& Get(Engine::UUID euid, Engine::UUID uuid = 0);

	//Get an object by handle
	template<typename T>
	T& Get(Handle& handle);

	//Get an object by passing in their owner(Entity/Component)
	template<typename T, typename Owner>
	T& Get(Owner& object);

	//Find an object solely by UUID(Slow)
	template<typename T>
	T& GetByUUID(Engine::UUID uuid);

	//Gets an array of component pointers that belongs to given EUID
	template<typename T>
	std::vector<T*> GetMulti(Engine::UUID euid);

	//Gets an array of component pointers that belongs to given object
	template<typename T, typename Owner>
	std::vector<T*> GetMulti(Owner& object);

	//Destroy an object
	template<typename T>
	void Destroy(T& object);

	//Gets an array of objects(Entity/Components)
	template <typename T>
	auto& GetArray();

	//Checks if an object is active
	template <typename T>
	bool IsActive(T& object);

	//Checks if an object is active
	template <typename T>
	bool IsActive(const Handle& handle);

	//Sets active for an object
	template <typename T>
	void SetActive(T& object, bool val = true);

	//Sets active for an object
	template <typename T>
	void SetActive(const Handle& handle, bool val = true);

	//Checks if an entity has a component
	template <typename Component>
	bool Has(const Entity& entity);

	//Checks if scene has a handle
	template <typename T>
	bool HasHandle(const Handle& handle);

	//Adds an object by giving a owner to it, sharing its euid
	template <typename T, typename Owner, typename... Args>
	T* Add(const Owner& owner, Args&&... args);

	//Adds an object creating an entirely new one, EUID should be specified if 
	//they are components as they should belong to an entity
	template <typename T, typename... Args>
	T* Add
	(
		Engine::UUID euid = Engine::CreateUUID(),
		Engine::UUID uuid = Engine::CreateUUID(),
		Args&&... args
	);


#pragma region SCRIPTING/DESERIALIZATION HELPERS
	//Get component of another object as a void pointer(Mono does type casting)
	GENERIC_RECURSIVE(void*, Get, &Get<T>(*(Object*)pObject));

	GENERIC_RECURSIVE(void*, Add, Add<T>(*(Entity*)pObject));

	//Get active from scripts
	GENERIC_RECURSIVE(bool, GetActive, IsActive(*(T*)pObject));

	struct SetActiveHelper
	{
		void* object;
		bool val;
	};
	//Set active from scripts
	GENERIC_RECURSIVE(void, SetActive, SetActive(*(T*)reinterpret_cast<SetActiveHelper*>(pObject)->object, reinterpret_cast<SetActiveHelper*>(pObject)->val));

	GENERIC_RECURSIVE(void*, GetByHandle, &Get<T>(*(Handle*)pObject));
	//Check whether handle exists in scene
	GENERIC_RECURSIVE(bool, HasHandle, HasHandle<T>(*(Handle*)pObject));
#pragma endregion
private:
	using EntitiesList = ObjectsList<Entity, MAX_ENTITIES>;
	using EntitiesPtrArray = std::vector<Entity*>;
	EntitiesList entities;	//Vector should be in order
	SingleComponentsArrays singleComponentsArrays;
	MultiComponentsArrays multiComponentsArrays;
	EntitiesPtrArray entitiesDeletionBuffer;
	ComponentsBufferArray componentsDeletionBuffer;
	SingleHandles singleHandles;
	MultiHandles multiHandles;

	//Helper to iterate through all types in buffer to delete components
	template <typename T, typename... Ts>
	struct ClearBufferStruct
	{
		//Template pack initialization
		constexpr ClearBufferStruct(TemplatePack<T, Ts...> pack) {}
		//Constructor, calls CleanComponents
		ClearBufferStruct(Scene& _scene);

		//Deletes components by reading buffer and erasing from respective component arrays
		template <typename T1, typename... T1s>
		void CleanComponents();

		Scene& scene;
	};

	//Helper to iterate through all types and add to buffer when entity is deleted
	template<typename T, typename... Ts>
	struct DestroyComponentsGroup
	{
		//Template pack initialization
		constexpr DestroyComponentsGroup(TemplatePack<T, Ts...> pack) {}
		DestroyComponentsGroup(Scene& _scene, Entity& _entity);

		//Adds to components buffer for deletion
		template <typename T1, typename... T1s>
		void DestroyComponents();

		Scene& scene;
		Entity& entity;
	};

	//Helps copy paste all components over to copy construction
	template <typename T, typename... Ts>
	void CloneHelper(Scene& rhs);

	//Helps copy paste all components over to copy construction
	template <typename T, typename... Ts>
	void CloneHelper(Scene& rhs, TemplatePack<T, Ts...>);

	//Erases the handle from a table when an object is destroyed
	template <typename T>
	void EraseHandle(T& object);

	using ClearBufferHelper = decltype(ClearBufferStruct(AllComponentTypes()));
	using DestroyEntityComponents = decltype(DestroyComponentsGroup((AllComponentTypes())));
};

#include "SceneTemplates.cpp"

#endif SCENE_H