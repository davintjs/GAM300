/*!***************************************************************************************
\file			Components.h
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			10/03/2023

\brief
	This file declares all types of components

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#ifndef COMPONENTS_H
#define COMPONENTS_H

#define MAX_MULTI_COMPONENTS 1

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Utilities/TemplatePack.h"
#include "Utilities/ObjectsList.h"
#include "Entity.h"

using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using Vector4 = glm::vec4;

#define ComponentName(Type) #Type
template <typename T>
using ComponentsPtrArray = std::vector<T*>;

template<typename T, typename... Ts>
struct GetComponentTypeGroup
{
	static constexpr const char* name = ComponentName(T);
	static constexpr size_t e = sizeof...(Ts);
	constexpr GetComponentTypeGroup(TemplatePack<T,Ts...> pack) {}
	constexpr GetComponentTypeGroup() = default;

	template <typename T1>
	static constexpr size_t E()
	{
		if constexpr (std::is_same<T, T1>())
		{
			return e;
		}
		else
		{
			return GetComponentTypeGroup<Ts...>::E();
		}
	}
};

template<typename... Ts>
struct SingleComponentsGroup
{
	constexpr SingleComponentsGroup(TemplatePack<Ts...>) {}
	SingleComponentsGroup() = default;

	template <typename T1>
	constexpr ObjectsList<T1, MAX_ENTITIES>& GetArray()
	{
		static_assert((std::is_same_v<T1, Ts> || ...), "Type not found in ArrayGroup");
		return std::get<ObjectsList<T1, MAX_ENTITIES>>(arrays);
	}
private:
	std::tuple<ObjectsList<Ts, MAX_ENTITIES>...> arrays;
};


template <typename Component>
using MultiComponent = ObjectsList<Component, MAX_MULTI_COMPONENTS>;

template <typename Component>
using MultiComponentsArray = ObjectsList<MultiComponent<Component>, MAX_ENTITIES>;

template<typename... Ts>
struct MultiComponentsGroup
{
	constexpr MultiComponentsGroup(TemplatePack<Ts...>) {}
	MultiComponentsGroup() = default;

	template <typename T1>
	constexpr MultiComponentsArray<T1>& GetArray()
	{
		static_assert((std::is_same_v<T1, Ts> || ...), "Type not found in ArrayGroup");
		return std::get<MultiComponentsArray<T1>>(arrays);
	}
private:
	std::tuple<MultiComponentsArray<Ts>...> arrays;
};



struct Transform
{
	Vector3 translation{};
	Vector3 rotation{};
	Vector3 scale{1};
};

struct AudioSource
{
	bool loop = false;
	float volume = 1.0f;
};

struct BoxCollider
{
};

struct SphereCollider
{
};

struct CapsuleCollider
{
};

struct Animator
{
};

//struct Camera
//{
//};

struct Rigidbody
{
	Vector3 velocity{};					//velocity of object
	Vector3 acceleration{};				//acceleration of object
	Vector3 force{};					//forces acting on object, shud be an array
	float mass{ 1.f };					//mass of object
	bool isKinematic{ true };			//is object simulated?
	bool useGravity{ true };			//is object affected by gravity?
};

struct Script
{
	std::string name;
};

//MULTI COMPONENT ARRAYS
/*
Lets say MAX_MULTI_COMPONENTS = 4
For each GameObject, they will preallocate for 4 of the same components
If there are more than 4, it will allocate for a new set of same components,
This way, its a forward list with some cache locality and can scale infinitely
So technically this would it a sparse set of 
*/

using SingleComponentTypes = TemplatePack<Rigidbody, Animator>;
using MultiComponentTypes = TemplatePack<BoxCollider, SphereCollider, CapsuleCollider, AudioSource, Script>;
using SingleComponentsArrays = decltype(SingleComponentsGroup(SingleComponentTypes()));
using MultiComponentsArrays = decltype(MultiComponentsGroup(MultiComponentTypes()));
using GetComponentType = decltype(GetComponentTypeGroup(SingleComponentTypes().Concatenate(MultiComponentTypes())));


#endif // !COMPONENTS_H