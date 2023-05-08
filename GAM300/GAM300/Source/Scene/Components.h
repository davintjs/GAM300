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

#define MAX_COMPONENTS 100000
#define MAX_MULTI_COMPONENTS 5

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Utilities/TemplatePack.h"
#include "Utilities/ObjectsArray.h"

using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using Vector4 = glm::vec4;

//#define RegisterComponent(Type) template <> struct GetComponentType<Type>{static constexpr size_t e{ (size_t)ComponentType::Type }; static constexpr const char* name = #Type;}

template <typename T>
using SingleComponentsArray = ObjectsArray<T, MAX_COMPONENTS>;
template <typename T>
using MultiComponentsArray = SparseSet<ObjectsArray<T, MAX_MULTI_COMPONENTS>, MAX_COMPONENTS>;
template <typename T>
using ComponentsPtrArray = std::vector<T*>;

struct None {};

template<typename T, typename... Ts>
struct SingleComponentGroup : SingleComponentGroup<Ts...>
{
	constexpr SingleComponentGroup(TemplatePack<T,Ts...> pack) {}
	SingleComponentsArray<T> components;
	SingleComponentGroup() = default;

	template <typename T1>
	constexpr SingleComponentsArray<T1>& GetArray()
	{
		if constexpr (std::is_same<T, T1>())
		{
			return components;
		}
		else if (sizeof...(Ts) == 1)
		{
			static_assert(true);
		}
		else
		{
			return SingleComponentGroup<Ts...>::GetArray();
		}
	}
};

template<typename... Ts>
struct SingleComponentGroup <None, Ts...>
{};

template<typename T, typename... Ts>
struct MultiComponentGroup : MultiComponentGroup<Ts...>
{
	constexpr MultiComponentGroup(TemplatePack<T,Ts...> pack) {}
	MultiComponentsArray<T> components;
	MultiComponentGroup() = default;

	template <typename T1>
	constexpr MultiComponentsArray<T1>& GetArray()
	{
		if constexpr (std::is_same<T, T1>())
		{
			return components;
		}
		else if (sizeof...(Ts) == 1)
		{
			static_assert(true);
		}
		else
		{
			return MultiComponentGroup<Ts...>::GetArray();
		}
	}
};

template<typename... Ts>
struct MultiComponentGroup <None, Ts...>
{};

template<typename T, typename... Ts>
struct ComponentPtrGroup
{
	constexpr ComponentPtrGroup(TemplatePack<T, Ts...> pack) {}
	ComponentsPtrArray<T> components;
	ComponentPtrGroup() = default;

	template <typename T1>
	constexpr ComponentsPtrArray<T1>& GetArray()
	{
		if constexpr (std::is_same<T, T1>())
		{
			return components;
		}
		else if (sizeof...(Ts) == 1)
		{
			static_assert(true);
		}
		else
		{
			return ComponentPtrGroup<Ts...>::GetArray();
		}
	}
};

template<typename... Ts>
struct ComponentPtrGroup<None,Ts...>{};

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

struct Camera
{
};

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
template <typename T>
struct GetComponentType {};

//RegisterComponent(Transform);
//RegisterComponent(BoxCollider);
//RegisterComponent(SphereCollider);
//RegisterComponent(CapsuleCollider);
//RegisterComponent(Rigidbody);
//RegisterComponent(Camera);
//RegisterComponent(Animator);
//RegisterComponent(AudioSource);
//RegisterComponent(Script);
using SingleComponentTypes = TemplatePack<Rigidbody, Animator, Camera,None>;
using MultiComponentTypes = TemplatePack<BoxCollider, SphereCollider, CapsuleCollider, AudioSource, Script, None>;
using SingleComponentsArrays = decltype(SingleComponentGroup(SingleComponentTypes()));
using MultiComponentsArrays = decltype(MultiComponentGroup(MultiComponentTypes()));
using ComponentsPtrArrays = decltype(ComponentPtrGroup(SingleComponentTypes().Concatenate(MultiComponentTypes())));


#endif // !COMPONENTS_H