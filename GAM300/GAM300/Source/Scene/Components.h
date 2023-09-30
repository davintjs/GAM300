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
#define GLM_ENABLE_EXPERIMENTAL
#include "Utilities/TemplatePack.h"
#include "Utilities/ObjectsList.h"
#include "Utilities/ObjectsBList.h"
#include "Graphics/GraphicStructsAndClass.h"
#include "Scene/Object.h"
#include <Scripting/ScriptFields.h>
#include <map>

#include <Properties.h>

constexpr size_t MAX_ENTITIES{ 5 };

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using Quaternion = glm::quat;

struct Entity;

extern std::map<std::string, size_t> ComponentTypes;

template<typename T,typename... Ts>
struct GetTypeGroup
{
	constexpr GetTypeGroup(TemplatePack<T,Ts...> pack) {}
	constexpr GetTypeGroup() = default;

	template <typename T1>
	static constexpr size_t E()
	{
		if constexpr (std::is_same<T, T1>())
		{
			return sizeof...(Ts);
		}
		else
		{
			return GetTypeGroup<Ts...>::template E<T1>();
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
			return GetTypeGroup<Ts...>::template Name<T1>();
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
	
	Engine::UUID parent=0;
	std::vector<Engine::UUID> child;
	bool isLeaf();
	bool isChild();
	glm::mat4 GetWorldMatrix() const;
	glm::mat4 GetInvertedWorldMatrix() const;
	glm::mat4 GetLocalMatrix() const;
	bool isEntityChild(Transform& ent);
	void SetParent(Transform* newParent);
	void RemoveChild(Transform* t);
	property_vtable();
};

property_begin_name(Transform, "Transform") 
{
	property_var(translation).Name("Translation"),
	property_var(rotation).Name("Rotation"),
	property_var(scale).Name("Scale"),
} property_vend_h(Transform)

struct AudioSource : Object
{
	enum Channel { MUSIC, SFX, LOOPFX, COUNT } channel = SFX;
	std::vector<const char*> ChannelName =
	{
		"Music",
		"SFX",
		"Looping SFX"
	};
	bool loop = false;
	bool play = false;
	float volume = 1.0f;
	std::string currentSound = "";
	property_vtable();
};

property_begin_name(AudioSource, "Audio Source") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
		//property_var(ChannelName).Name("channel"),
		property_var(loop).Name("Loop"),
		property_var(volume).Name("Volume"),
		property_var(play).Name("Play")
} property_vend_h(AudioSource)

struct BoxCollider : Object
{
	float x = 1.0f;
	float y = 1.0f; 
	float z = 1.0f; 
	property_vtable();
};

property_begin_name(BoxCollider, "BoxCollider") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
		property_var(x).Name("X"),
		property_var(y).Name("Y"),
		property_var(z).Name("Z"),
} property_vend_h(BoxCollider)

struct SphereCollider : Object
{
	float radius = 1.0f; 
	property_vtable();
};

property_begin_name(SphereCollider, "SphereCollider") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
		property_var(radius).Name("Radius")
} property_vend_h(SphereCollider)

struct CapsuleCollider : Object
{
	float height = 1.0f; 
	float radius = 1.0f; 
	property_vtable();
};

property_begin_name(CapsuleCollider, "CapsuleCollider") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
		property_var(height).Name("Height"),
		property_var(radius).Name("Radius")
} property_vend_h(CapsuleCollider)

struct Animator : Object
{
};

//struct Camera
//{
//};

struct Rigidbody : Object
{
	Vector3 linearVelocity{};			//velocity of object
	Vector3 angularVelocity{};
	Vector3 force{};					//forces acting on object, shud be an array
	float friction{ 0.1f };				//friction of body (0 <= x <= 1)
	float mass{ 1.f };					//mass of object
	bool isStatic{ true };				//is object static? If true will override isKinematic!
	bool isKinematic{ true };			//is object simulated?
	bool useGravity{ true };			//is object affected by gravity?
	bool is_trigger = false;

	property_vtable();
	UINT32 bid{0};

	//JPH::BodyID RigidBodyID;			//Body ID 
};

property_begin_name(Rigidbody, "Rigidbody") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
		property_var(linearVelocity).Name("LinearVelocity"),
		property_var(angularVelocity).Name("AngularVelocity"),
		property_var(force).Name("Force"),
		property_var(friction).Name("Friction"),
		property_var(mass).Name("Mass"),
		property_var(isStatic).Name("IsStatic"),
		property_var(isKinematic).Name("IsKinematic"),
		property_var(useGravity).Name("UseGravity"),
		property_var(is_trigger).Name("Is_trigger")
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
	UINT32 bid{0};
	//JPH::BodyID CharacterBodyID;
};

property_begin_name(CharacterController, "CharacterController") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
		property_var(velocity).Name("Velocity"),
		property_var(force).Name("Force"),
		property_var(friction).Name("Friction"),
		property_var(mass).Name("Mass"),
		property_var(gravityFactor).Name("GravityFactor"),
		property_var(slopeLimit).Name("SlopeLimit")
} property_vend_h(CharacterController)

struct Script : Object
{
	std::string name;
	Script() {}
	Script(const char* _name):name{_name}{}
	property_vtable();
};

property_begin_name(Script, "Script") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
		property_var(name).Name("Name"),
		//property_var(fields)
} property_vend_h(Script)

struct MeshRenderer : Object
{
	std::string MeshName = "Cube";
	std::string AlbedoTexture = "";
	std::string NormalMap = "";
	//Materials mr_Material;

	// Materials stuff below here
	Vector4 mr_Albedo;

	Vector4 mr_Specular;
	Vector4 mr_Diffuse;
	Vector4 mr_Ambient;
	float mr_Shininess;


	float mr_metallic = 0.5f;
	float mr_roughness = 0.5f;
	float ao = 0.5f;

	std::string MetallicTexture = "";
	std::string RoughnessTexture = "";
	std::string AoTexture= "";

	GLuint textureID = 0;
	GLuint normalMapID = 0;
	GLuint RoughnessID = 0;
	GLuint MetallicID = 0;
	GLuint AoID = 0;

	property_vtable();
};

property_begin_name(MeshRenderer, "MeshRenderer") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
		property_var(MeshName).Flags(property::flags::DONTSHOW),
		property_var(mr_Albedo),
		property_var(AlbedoTexture),
		property_var(NormalMap),		
		/*property_var(mr_Specular),
		property_var(mr_Diffuse),
		property_var(mr_Ambient),
		property_var(mr_Shininess),*/
		property_var(mr_metallic),
		property_var(MetallicTexture),

		property_var(mr_roughness),
		property_var(RoughnessTexture),

		property_var(ao),
		property_var(AoTexture),
} property_vend_h(MeshRenderer)

struct LightSource : Object
{
	Vector3 lightingColor{ 1.f, 1.f, 1.f };
	property_vtable()
};

property_begin_name(LightSource, "LightSource") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
		property_var(lightingColor).Name("LightingColor")
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

#define GENERIC_RECURSIVE(TYPE,FUNC_NAME,FUNC) \
	template<typename T, typename... Ts>\
	TYPE FUNC_NAME##Iter(size_t objType,void* pObject)\
	{\
		if (GetType::E<T>() == objType)\
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
			return FUNC_NAME##Iter<Ts...>(objType,pObject); \
		}\
	}\
	template<typename T, typename... Ts>\
	TYPE FUNC_NAME##Start( TemplatePack<T,Ts...>,size_t objType, void* pObject)\
	{return FUNC_NAME##Iter<T,Ts...>(objType,pObject);}\
	TYPE FUNC_NAME(size_t objType, void* pObject)\
	{return FUNC_NAME##Start(AllObjectTypes(), objType,pObject);}\

template <typename T, typename... Ts>
struct GenericRecursiveStruct
{
	constexpr GenericRecursiveStruct(TemplatePack<T,Ts...>) {}
	GenericRecursiveStruct() = default;

	template <typename RET, typename... ARGS>
	auto Invoke(std::function<RET(ARGS...)> func, ARGS... args)
	{
		return Invoke<T, Ts...>(func,args);
	}

	template<typename T1,typename T1s ,typename RET ,typename... ARGS>
	auto Invoke(std::function<RET(ARGS...)> func, ARGS... args)
	{
		if constexpr (std::is_same<RET, T1>())
		{
			return func(std::forward(func));
		}
		if constexpr (sizeof...(Ts) != 0)\
		{
			return Invoke<T1s...>(func, args);
		}
	}
};

using GenericRecursive = decltype(GenericRecursiveStruct(AllComponentTypes()));

using FieldTypes = TemplatePack<float, double, bool, char, short, int, int64_t, uint16_t, uint32_t, uint64_t, std::string, Vector2, Vector3>;

using AllObjectTypes = decltype(TemplatePack<Entity>::Concatenate(AllComponentTypes()));
using AllFieldTypes = decltype(FieldTypes::Concatenate(AllObjectTypes()));
using GetFieldType = decltype(GetTypeGroup(AllFieldTypes()));

#endif // !COMPONENTS_H