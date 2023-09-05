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
#include "Utilities/ObjectsBList.h"

constexpr size_t MAX_ENTITIES{ 5 };

using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using Vector4 = glm::vec4;

#define ComponentName(Type) #Type
template <typename T>
using ComponentsPtrArray = std::vector<T*>;

template<typename T,typename... Ts>
struct GetComponentTypeGroup
{
	static constexpr const char* name = ComponentName(T);
	constexpr GetComponentTypeGroup(TemplatePack<T,Ts...> pack) {}
	constexpr GetComponentTypeGroup() = default;

	template <typename T1>
	static constexpr size_t E()
	{
		if constexpr (std::is_same<T, T1>())
		{
			return sizeof...(Ts);
		}
		else
		{
			return GetComponentTypeGroup<Ts...>::template E<T1>();
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

template<typename... Ts>
struct ComponentsBuffer
{
	constexpr ComponentsBuffer(TemplatePack<Ts...>) {}
	ComponentsBuffer() = default;

	template <typename T1>
	constexpr std::vector<T1*>& GetArray()
	{
		static_assert((std::is_same_v<T1, Ts> || ...), "Type not found in ArrayGroup");
		return std::get<std::vector<T1*>>(arrays);
	}
private:
	std::tuple<std::vector<Ts*>...> arrays;
};

template <typename Component>
using MultiComponentsArray = ObjectsBList<Component, MAX_ENTITIES, MAX_MULTI_COMPONENTS>;

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

struct Tag
{
	std::string name;
};

struct Transform
{
	Vector3 translation{};
	Vector3 rotation{};
	Vector3 scale{1};
	std::vector<Transform*>child;
	Transform* Parent = nullptr;

	bool isLeaf() {
		return (child.size() > 0) ? true : false;
	}
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

using SingleComponentTypes = TemplatePack<Tag,Transform,Rigidbody, Animator>;
using MultiComponentTypes = TemplatePack<BoxCollider, SphereCollider, CapsuleCollider, AudioSource, Script>;
using SingleComponentsArrays = decltype(SingleComponentsGroup(SingleComponentTypes()));
using MultiComponentsArrays = decltype(MultiComponentsGroup(MultiComponentTypes()));
using AllComponentTypes = decltype(SingleComponentTypes().Concatenate(MultiComponentTypes()));
using ComponentsBufferArray = decltype(ComponentsBuffer(AllComponentTypes()));
using GetComponentType = decltype(GetComponentTypeGroup(AllComponentTypes()));


#endif // !COMPONENTS_H