/*!***************************************************************************************
\file			Components.h
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			10/03/2023

\brief
	This file declares all types of components

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Utilities/TemplatePack.h"
#include "Utilities/ObjectsList.h"
#include "Utilities/ObjectsBList.h"
#include <vector>

// JPH
#include "Jolt/Jolt.h"
#include "Jolt/RegisterTypes.h"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Core/JobSystemSingleThreaded.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include <Jolt/Physics/Collision/Shape/ConvexShape.h>
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyActivationListener.h"

constexpr size_t MAX_ENTITIES{ 5 };

using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using Vector4 = glm::vec4;

template<typename T,typename... Ts>
struct GetComponentTypeGroup
{
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

	template <typename T1>
	static constexpr const char* Name()
	{
		if constexpr (std::is_same<T, T1>())
		{
			static const char* name = typeid(T).name() + strlen("struct ");
			return name;
		}
		else
		{
			return GetComponentTypeGroup<Ts...>::template Name<T1>();
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
using MultiComponentsArray = ObjectsBList<Component, MAX_ENTITIES>;

template<typename... Ts>
struct MultiComponentsGroup
{
	constexpr MultiComponentsGroup(TemplatePack<Ts...>) {}
	MultiComponentsGroup() = default;

	template <typename T1>
	constexpr MultiComponentsArray<T1>& GetArray()
	{
		static_assert((std::is_same_v<T1, Ts> || ...), "TESTES");
		return std::get<MultiComponentsArray<T1>>(arrays);
	}
private:
	std::tuple<MultiComponentsArray<Ts>...> arrays;
};

#pragma region COMPONENTS

struct Tag
{
	std::string name;
};

struct Transform
{
	bool is_enabled = true;
	std::string str = "Transform";
	Vector3 translation{};
	Vector3 rotation{};
	Vector3 scale{1};
	std::vector<Transform*>child;
	Transform* Parent = nullptr;

	bool isLeaf() {
		return (child.size()) ? false : true;
	}

	bool isChild() {
		if (Parent)
			return true;
		else
			return false;
	}

	bool isEntityChild(Transform& ent) {
		if (std::find(child.begin(), child.end(), &ent) != child.end()) {
			return true;
		}
		for (int i = 0; i < child.size(); i++) {
				return child[i]->isEntityChild(ent);
		}
		return false;
	}

	
};

struct AudioSource
{
	bool loop = false;
	float volume = 1.0f;
};

struct BoxCollider
{
	float x = 1.0f;
	float y = 1.0f; 
	float z = 1.0f; 
};

struct SphereCollider
{
	float radius = 1.0f; 
};

struct CapsuleCollider
{
	float height = 1.0f; 
	float radius = 1.0f; 
};

struct Animator
{
};

//struct Camera
//{
//};

struct Rigidbody
{
	bool is_enabled = true;
	std::string str = "Rigidbody";
	Vector3 linearVelocity{};					//velocity of object
	Vector3 angularVelocity{};
	Vector3 force{};					//forces acting on object, shud be an array

	float friction{ 0.1f };				//friction of body (0<=x<=1)
	float mass{ 1.f };					//mass of object
	bool isStatic{ true };				//is object static? If true will override isKinematic!
	bool isKinematic{ true };			//is object simulated?
	bool useGravity{ true };			//is object affected by gravity?
	JPH::BodyID RigidBodyID;			//Body ID 
};

struct Script
{
	std::string name;
};
#pragma endregion



//Append here if you defined a new component and each entity should only ever have one of it
using SingleComponentTypes = TemplatePack<Transform, Tag, Rigidbody, Animator>;

//Append here if entity can have multiple of this
using MultiComponentTypes = TemplatePack<BoxCollider, SphereCollider, CapsuleCollider, AudioSource, Script>;

using SingleComponentsArrays = decltype(SingleComponentsGroup(SingleComponentTypes()));
using MultiComponentsArrays = decltype(MultiComponentsGroup(MultiComponentTypes()));
using AllComponentTypes = decltype(SingleComponentTypes().Concatenate(MultiComponentTypes()));
using ComponentsBufferArray = decltype(ComponentsBuffer(AllComponentTypes()));
using GetComponentType = decltype(GetComponentTypeGroup(AllComponentTypes()));


#endif // !COMPONENTS_H