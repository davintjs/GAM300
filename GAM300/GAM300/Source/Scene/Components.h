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
#include "Graphics/GraphicStructsAndClass.h"
#include "Scene/Object.h"
#include <Scripting/ScriptFields.h>
#include <map>

#include <Properties.h>

constexpr size_t MAX_ENTITIES{ 5 };

using Vector2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
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

struct Tag : Object
{
	std::string name;
};


//example of LIONant reflection

//struct test1 : property::base
//{
//	int             m_Int{ 0 };
//	float           m_Float{ 0 };
//	bool            m_Bool{ 0 };
//	std::string     m_String{};
//	oobb            m_OOBB{ 0 };
//
//	property_vtable()           // Allows the base class to get these properties  
//};
//
//property_begin(test1)
//{
//	property_var(m_Int)
//		, property_var(m_Float)
//		, property_var(m_String)
//		, property_var(m_Bool)
//		, property_var(m_OOBB)
//} property_vend_h(test1)


struct Transform : Object
{
	Vector3 translation{};
	Vector3 rotation{};
	Vector3 scale{ 1 };
	
	Transform* parent = nullptr;
	
	std::vector<Transform*> child;

	bool isLeaf() {
		return (child.size()) ? false : true;
	}

	bool isChild() {
		if (parent != nullptr)
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
		glm::mat4 rot = glm::toMat4(glm::quat(vec3(rotation)));
		
		return glm::translate(glm::mat4(1.0f), vec3(translation)) *
			rot *
			glm::scale(glm::mat4(1.0f),vec3(scale));
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

	void SetParent(Transform* newParent) 
	{
		// Calculate the global transformation matrix
		if (parent) {
			//parent->RemoveChild(this);
			//parent->RemoveChild(this);
			/*auto _parent = parent->child;
			auto it = std::find(_parent.begin(), _parent.end(), this);
			if (it == _parent.end()) {
				std::cout << "cant find\n";
			}*/
			
			/*glm::mat4 globalTransform = GetWorldMatrix();
			glm::quat rot;
			glm::vec3 skew;
			glm::vec4 perspective;

			vec3 _scale = vec3(scale);
			vec3 _translation = vec3(translation);
			glm::decompose(globalTransform, _scale, rot, _translation, skew, perspective);
			scale = _scale;
			translation = _translation;
			rotation = glm::eulerAngles(rot);*/
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

			vec3 _scale = vec3(scale);
			vec3 _translation = vec3(translation);
			glm::decompose(lTransform, _scale, rot, _translation, skew, perspective);
			scale = _scale;
			translation = _translation;
			rotation = glm::eulerAngles(rot);

			// Add the object to the new parent's child list
			parent->child.push_back(this);
		}
	}

	void RemoveChild(Transform* t)
	{
		auto it = std::find(child.begin(), child.end(),t);

		// Check if an element satisfying the condition was found
		//E_ASSERT(it != child.end(), "FAILED TO REMOVE CHILD");
		// Erase the found element
		child.erase(it);
	}

	property_vtable();

};

//property_begin_name(Transform, "Transform") {
//	property_var(scale.x), property_var(scale.y), property_var(scale.z)
//		, property_var(rotation.x), property_var(rotation.y), property_var(rotation.z)
//		, property_var(translation.x), property_var(translation.y), property_var(translation.z)
//} property_vend_h(Transform)//

property_begin_name(Transform, "Transform") {
		property_var(translation),
			property_var(rotation),
			property_var(scale),
			
	} property_vend_h(Transform)

struct AudioSource : Object
{
	bool loop = false;
	float volume = 1.0f;
	property_vtable();
};

property_begin_name(AudioSource, "Audio Source") {
		property_var(loop)
		,property_var(volume)
} property_vend_h(AudioSource)

struct BoxCollider : Object
{
	float x = 1.0f;
	float y = 1.0f; 
	float z = 1.0f; 
	property_vtable();
};

property_begin_name(BoxCollider, "BoxCollider") {
	property_var(x)
		, property_var(y)
		, property_var(z)
} property_vend_h(BoxCollider)

struct SphereCollider : Object
{
	float radius = 1.0f; 
	property_vtable();
};

property_begin_name(SphereCollider, "SphereCollider") {
	property_var(radius)
} property_vend_h(SphereCollider)

struct CapsuleCollider : Object
{
	float height = 1.0f; 
	float radius = 1.0f; 
	property_vtable();
};

property_begin_name(CapsuleCollider, "CapsuleCollider") {
	property_var(height)
		, property_var(radius)
} property_vend_h(CapsuleCollider)

struct Animator : Object
{
};

//struct Camera
//{
//};

struct Rigidbody : Object
{
	bool is_enabled = true;
	bool is_trigger = false;
	Vector3 linearVelocity{};			//velocity of object
	Vector3 angularVelocity{};
	Vector3 force{};					//forces acting on object, shud be an array

	float friction{ 0.1f };				//friction of body (0 <= x <= 1)
	float mass{ 1.f };					//mass of object
	bool isStatic{ true };				//is object static? If true will override isKinematic!
	bool isKinematic{ true };			//is object simulated?
	bool useGravity{ true };			//is object affected by gravity?
	property_vtable();
	//JPH::BodyID RigidBodyID;			//Body ID 
};

property_begin_name(Rigidbody, "Rigidbody") {

	property_var(linearVelocity)
		, property_var(angularVelocity)
		, property_var(force)
		, property_var(friction)
		, property_var(mass)
		, property_var(isStatic)
		, property_var(isKinematic)
		, property_var(useGravity)
		, property_var(is_enabled)
		, property_var(is_trigger)
} property_vend_h(Rigidbody)

struct CharacterController : Object
{

	Vector3 velocity{};					// velocity of the character
	Vector3 force{};					// forces acting on the character
	float mass{ 1.f };					// mass of object
	float friction{ 0.1f };				// friction of body (0 <= x <= 1)
	float gravityFactor{ 1.f };			// gravity modifier
	float slopeLimit{ 45.f };			// the maximum angle of slope that character can traverse in degrees!
	property_vtable();
	//JPH::BodyID CharacterBodyID;
};

property_begin_name(CharacterController, "CharacterController") {
	property_var(velocity)
		, property_var(force)
		, property_var(friction)
		, property_var(mass)
		, property_var(gravityFactor)
		, property_var(slopeLimit)
} property_vend_h(CharacterController)

struct Script : Object
{
	std::string name;
	std::map<std::string, Field> fields;
	property_vtable();
};


property_begin_name(Script, "Script") {
	property_var(name)
		//, property_var(fields)
} property_vend_h(Script)

struct MeshRenderer : Object
{
	std::string MeshName = "Cube";
	//Materials mr_Material;

	// Materials stuff below here
	Vector4 mr_Albedo;
	Vector4 mr_Specular;
	Vector4 mr_Diffuse;
	Vector4 mr_Ambient;
	float mr_Shininess;

	property_vtable();
};

property_begin_name(MeshRenderer, "MeshRenderer") {
	//property_var(MeshName),
		property_var(mr_Albedo)
		, property_var(mr_Specular)
		, property_var(mr_Diffuse)
		, property_var(mr_Ambient)
		, property_var(mr_Shininess)
} property_vend_h(MeshRenderer)

struct LightSource : Object
{
	Vector3 lightingColor{ 1.f, 1.f, 1.f };
	property_vtable()
};

property_begin_name(LightSource, "LightSource") {
	property_var(lightingColor)
} property_vend_h(LightSource)

#pragma endregion



//Append here if you defined a new component and each entity should only ever have one of it
using SingleComponentTypes = TemplatePack<Transform, Tag, Rigidbody, Animator,MeshRenderer, CharacterController, LightSource>;

//Append here if entity can have multiple of this
using MultiComponentTypes = TemplatePack<BoxCollider, SphereCollider, CapsuleCollider, AudioSource, Script>;

using SingleComponentsArrays = decltype(SingleComponentsGroup(SingleComponentTypes()));
using MultiComponentsArrays = decltype(MultiComponentsGroup(MultiComponentTypes()));
using AllComponentTypes = decltype(SingleComponentTypes().Concatenate(MultiComponentTypes()));
using DisplayableComponentTypes = decltype(AllComponentTypes().Pop().Pop());
using ComponentsBufferArray = decltype(ComponentsBuffer(AllComponentTypes()));
using GetComponentType = decltype(GetComponentTypeGroup(AllComponentTypes()));

#define GENERIC_RECURSIVE(TYPE,FUNC_NAME,FUNC) \
	template<typename T, typename... Ts>\
	static TYPE FUNC_NAME##Iter(size_t componentType,void* pComponent)\
	{\
		if (GetComponentType::E<T>() == componentType)\
		{\
			if constexpr (std::is_same<TYPE,void>())\
			{\
				FUNC;\
				return;\
			}\
			else\
			{\
				return FUNC;\
			}\
		}\
		if constexpr (sizeof...(Ts) != 0)\
		{\
			return FUNC_NAME##Iter<Ts...>(componentType,pComponent); \
		}\
		else if constexpr(!std::is_same<TYPE,void>())\
		{\
			return nullptr; \
		}\
	}\
	template<typename T, typename... Ts>\
	static TYPE FUNC_NAME##Start( TemplatePack<T,Ts...>,size_t componentType, void* pComponent)\
	{return FUNC_NAME##Iter<T,Ts...>(componentType,pComponent);}\
	static TYPE FUNC_NAME(size_t componentType, void* pComponent)\
	{return FUNC_NAME##Start(AllComponentTypes(), componentType,pComponent);}\

enum class FieldType :int
{
	Float = AllComponentTypes::Size(), Double,
	Bool, Char, Short, Int, Long,
	UShort, UInt, ULong, String,
	Vector2, Vector3, GameObject, None
};

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