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

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "Utilities/TemplatePack.h"
#include "Utilities/ObjectsList.h"
#include "Utilities/ObjectsBList.h"
#include <vector>

constexpr size_t MAX_ENTITIES{ 5 };

using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using Vector4 = glm::vec4;
using Quaternion = glm::quat;

static std::map<std::string, size_t> ComponentTypes{};



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
	Vector3 scale{ 1 };
	Vector3 rotation{};
	Vector3 translation{};
	Transform* parent = nullptr;

	std::vector<Transform*> child;

	bool isLeaf() {
		return (child.size()) ? false : true;
	}

	bool isChild() {
		if (parent)
			return true;
		else
			return false;
	}

	glm::mat4 GetWorldMatrix() const 
	{
		if (parent)
			return parent->GetWorldMatrix() * GetLocalMatrix();
		return GetLocalMatrix();
	}

	glm::mat4 GetInvertedWorldMatrix() const
	{
		if (parent)
			return glm::inverse(parent->GetInvertedWorldMatrix()) * GetLocalMatrix();
		return GetLocalMatrix();
	}

	glm::mat4 GetLocalMatrix() const {
		glm::mat4 rot = glm::toMat4(glm::quat(rotation));
		return glm::translate(glm::mat4(1.0f), translation) *
			rot *
			glm::scale(glm::mat4(1.0f), scale);
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

	void SetParent(Transform* newParent) {
		// Calculate the global transformation matrix
		if (parent) {
			parent->RemoveChild(this);
			glm::mat4 globalTransform = GetWorldMatrix();
			glm::quat rot;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(globalTransform, scale, rot, translation, skew, perspective);
			rotation = glm::eulerAngles(rot);
		}

		// Set the new parent
		glm::mat4 localTransform = GetWorldMatrix();
		parent = newParent;

		if (parent) {
			glm::mat4 parentTransform = parent->GetWorldMatrix();
			glm::mat4 lTransform = glm::inverse(parentTransform) * localTransform;
			glm::quat rot;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(lTransform, scale, rot, translation, skew, perspective);
			rotation = glm::eulerAngles(rot);

			// Add the object to the new parent's child list
			parent->child.push_back(this);
		}
	}

	void RemoveChild(Transform* t)
	{
		auto it = std::find(child.begin(), child.end(),t);

		// Check if an element satisfying the condition was found
		E_ASSERT(it != child.end(), "FAILED TO REMOVE CHILD");
		// Erase the found element
		child.erase(it);
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
	bool is_enabled = true;
	bool isKinematic{ true };			//is object simulated?
	bool useGravity{ true };			//is object affected by gravity?
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
using DisplayableComponentTypes = decltype(AllComponentTypes().Pop().Pop());
using ComponentsBufferArray = decltype(ComponentsBuffer(AllComponentTypes()));
using GetComponentType = decltype(GetComponentTypeGroup(AllComponentTypes()));

template<typename T, typename... Ts>
static void RegisterComponents()
{
	ComponentTypes.emplace(GetComponentType::Name<T>(), GetComponentType::E<T>());
	RegisterComponents<Ts...>();
}

template<typename... Ts>
static void RegisterComponents()
{}

#endif // !COMPONENTS_H