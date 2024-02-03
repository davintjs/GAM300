/*!***************************************************************************************
\file			Components.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			10/03/2023

\brief
	This file declares all types of components and template packs to be used by ECS

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#ifndef COMPONENTS_H
#define COMPONENTS_H

#define GLM_ENABLE_EXPERIMENTAL
#include "Utilities/TemplatePack.h"
#include "Utilities/ObjectsList.h"
#include "Utilities/ObjectsBList.h"
#include "Graphics/GraphicStructsAndClass.h"
#include "Graphics/Animation/BaseAnimator.h"
#include "Graphics/BaseCamera.h"
#include "Scene/Object.h"
#include <Scripting/ScriptFields.h>
#include <map>
#include <Utilities/GUID.h>
#include <AssetManager/AssetTypes.h>
#include <Core/EventsManager.h>
#include <Core/Events.h>
#include <Properties.h>
#include "Debugging/Debugger.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>


constexpr size_t MAX_ENTITIES{ 5 };

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using Quaternion = glm::quat;

struct Entity;
enum class SHADERTYPE;

extern std::map<std::string, size_t> ComponentTypes;

#pragma region COMPONENTS

//To store id information like name
struct Tag : Object
{
	std::string name;
	Engine::UUID tagName;
	size_t physicsLayerIndex = 0;
	property_vtable();
};

property_begin_name(Tag, "Tag")
{
	property_var(name),
		property_var(tagName).Name("Tag Name"),
		property_var(physicsLayerIndex).Name("Layer Index"),
} property_vend_h(Tag)

//To store transform of entity
struct Transform : Object
{
	//Parent's euid
	Engine::UUID parent = 0;

	enum class Flag : char
	{
		WorldEnabled = 0x01,
		Modified = 0x02,
	};

	char flags = (char)Flag::WorldEnabled;
	void EnableFlag(Flag flag);
	void DisableFlag(Flag flag);
	bool GetFlag(Flag flag);

	//Childrens' euids
	std::vector<Engine::UUID> child;

	bool operator == (const Transform& other) const {
		if ((translation == other.translation) &&
			(rotation == other.rotation) &&
			(scale == other.scale)) {
			return true;
		}
		return false;
	}

	//Check whether this is a leaf node
	bool isLeaf();

	//Check whether this is a child
	bool isChild();

	bool isSelectedChild();

	Transform* GetParent();

	static glm::mat4 CreateTransformationMtx(vec3 translation, vec3 rotation, vec3 scale)
	{
		glm::mat4 rot = glm::toMat4(glm::quat(vec3(rotation)));

		return glm::translate(glm::mat4(1.0f), vec3(translation)) *
			rot *
			glm::scale(glm::mat4(1.0f), vec3(scale));
	}

	static void Decompose(glm::mat4& mat, Vector3& pos, glm::quat& rot, Vector3& scale)
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(mat, (glm::fvec3&)scale, rot, (glm::fvec3&)pos, skew, perspective);
	}

	// Get the translation in world space
	glm::vec3 GetGlobalTranslation() const { return globalPos; }

	// Get the rotation in world space
	glm::vec3 GetGlobalRotation() const { return globalRot; }

	// Get the scale in world space
	glm::vec3 GetGlobalScale() const { return globalScale; }

	glm::vec3 GetLocalTranslation() const { return translation; }

	glm::vec3 GetLocalRotation() const { return rotation; }

	glm::vec3 GetLocalScale() const { return scale; }

	//Get the SRT matrix in world space, with account to parents transform
	glm::mat4 GetWorldMatrix() const;

	void SetGlobalPosition(Vector3 newPos);

	void SetGlobalRotation(Vector3 newRot);

	void SetGlobalScale(Vector3 newScale);

	void SetLocalPosition(Vector3 newPos);

	void SetLocalRotation(Vector3 newRot);

	void SetLocalScale(Vector3 newScale);

	void SetWorldMatrix(vec3 translation, vec3 rotation, vec3 scale);

	void SetLocalMatrix(vec3 _translation, vec3 _rotation, vec3 _scale);

	void UpdateChildrenMatrices();

	void UpdateWorldMatrix(glm::mat4& _world);

	void TempSetLocal(Vector3 pos, Vector3 rot, Vector3 scale);

	void RecalculateLocalMatrices();


	//Get the SRT matrix in local space
	glm::mat4 GetLocalMatrix() const;

	//Checks if this transform is a child of another
	bool isEntityChild(Transform& ent);

	//Sets the parent of the transform
	void SetParent(Transform* newParent);

	//Removes a child from child vector
	void RemoveChild(Transform* t);

private:
	glm::mat4x4 worldMatrix;
	Vector3 translation{};
	Vector3 rotation{};
	Vector3 scale{ 1 };
	Vector3 globalPos{};
	Vector3 globalRot{};
	Vector3 globalScale{};

public:

	property_vtable();

};

property_begin_name(Transform, "Transform")
{
	property_parent(Object).Flags(property::flags::DONTSHOW),
	property_var(translation).Name("Translation"),
	property_var(rotation).Name("Rotation"),
	property_var(scale).Name("Scale"),
	property_var(parent).Name("Father").Flags(property::flags::DONTSHOW| property::flags::REFERENCE)
} property_vend_h(Transform)

struct AudioSource : Object
{
	enum Channel { MUSIC, SFX, LOOPFX, COUNT };
	int current_channel = (int)SFX;

	std::vector<const char*> ChannelName =
	{
		"Music",
		"SFX",
		"Looping SFX"
	};
	bool loop = false;
	bool play = false;
	float volume{1.f};
	float fadetime{1.f};
	Engine::GUID<AudioAsset> currentSound;
	property_vtable();
};

property_begin_name(AudioSource, "Audio Source") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
		property_var(current_channel).Name("AudioChannel"),
		property_var(loop).Name("Loop"),
		property_var(volume).Name("Volume"),
		property_var(fadetime).Name("Fade Time (s)"),
		property_var(currentSound).Name("Sound File"),
		property_var(play).Name("Play")
} property_vend_h(AudioSource)

struct BoxCollider : Object
{
	Vector3 dimensions{1,1,1};
	Vector3 offset;
	property_vtable();
};

property_begin_name(BoxCollider, "BoxCollider") {
property_parent(Object).Flags(property::flags::DONTSHOW),
	property_var(dimensions).Name("Dimensions"),
	property_var(offset).Name("Offset")
} property_vend_h(BoxCollider)

struct SphereCollider : Object
{
	float radius = 1.0f;
	Vector3 offset;
	property_vtable();
};

property_begin_name(SphereCollider, "SphereCollider") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
	property_var(radius).Name("Radius"),
	property_var(offset).Name("Offset")
} property_vend_h(SphereCollider)

struct CapsuleCollider : Object
{
	float height = 1.0f;
	float radius = 1.0f;
	Vector3 offset;
	property_vtable();
};

property_begin_name(CapsuleCollider, "CapsuleCollider") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
	property_var(height).Name("Height"),
	property_var(radius).Name("Radius")
} property_vend_h(CapsuleCollider)

struct Animator : Object, BaseAnimator
{
	Animator() : BaseAnimator() {}
	property_vtable();
};

property_begin_name(Animator, "Animator") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
	property_parent(BaseAnimator)
} property_vend_h(Animator)

struct TextRenderer : Object
{
	std::string text = "Default Text";
	float x = 0.f, y = 0.f, scale = 1.f; // need to find out  how to link with existing obj srt
	float r = 0.5, g = 0.8f, b = 0.2f;
	property_vtable();
};

property_begin_name(TextRenderer, "TextRenderer") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
		property_var(text).Name("Text"),
		property_var(x).Name("X-axis Position"),
		property_var(y).Name("Y-axis Position"),
		property_var(scale).Name("Scale"),
		property_var(r).Name("Color R"),
		property_var(g).Name("Color G"),
		property_var(b).Name("Color B")
} property_vend_h(TextRenderer)

struct Camera : Object, BaseCamera
{
	Camera();
	Vector4 backgroundColor{};
	property_vtable();
};

property_begin_name(Camera, "Camera") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
	property_var(backgroundColor).Name("BackgroundColor"),
	property_parent(BaseCamera)
} property_vend_h(Camera)


struct PhysicsComponent : Object 
{
	enum Type : UINT32 {
		rb = 0, cc
	};
	UINT32 bid{ 0 };					// Jolt Body ID
	Type componentType{ rb };
};

struct Rigidbody : PhysicsComponent
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

struct CharacterController : PhysicsComponent
{
	Vector3 velocity{};					// velocity of the character
	Vector3 force{};					// forces acting on the character
	Vector3 direction{};

	float mass{ 1.f };					// mass of object
	float friction{ 0.1f };				// friction of body (0 <= x <= 1)
	float gravityFactor{ 1.f };			// gravity modifier
	float slopeLimit{ 45.f };			// the maximum angle of slope that character can traverse in degrees!
	bool isGrounded = false;

	float radius = 0.5f;				// radius of collider = radius * max(scale.x, scale.z)
	float height = 2.f;					// determines how much to offset the sphere caps and the cylinder height

	property_vtable();
};

property_begin_name(CharacterController, "CharacterController") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
	property_var(velocity).Name("Velocity"),
	property_var(force).Name("Force"),
	property_var(friction).Name("Friction"),
	property_var(mass).Name("Mass"),
	property_var(gravityFactor).Name("GravityFactor"),
	property_var(slopeLimit).Name("SlopeLimit"),
	property_var(isGrounded).Name("IsGrounded"),
	property_var(radius).Name("Radius"),
	property_var(height).Name("Height")
} property_vend_h(CharacterController)

struct Script : Object
{
	Script(){}
	Script(const char* yes) 
	{
		E_ASSERT(false,"INVALID CONSTRUCTOR");
	}
	Script(Engine::GUID<ScriptAsset> _scriptId) : scriptId{ _scriptId } {}
	Engine::GUID<ScriptAsset> scriptId{0};
	property_vtable();
};

property_begin_name(Script, "Script") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
	property_var(scriptId).Name("Script").Flags(property::flags::DONTSHOW),
	//property_var(fields)
} property_vend_h(Script)

struct MeshRenderer : Object
{

	// Material Instance
	//Material_instance* materialInstance;
	
	//Materials mr_Material;

	// Materials stuff below here

	GLuint VAO;
	GLuint debugVAO;

	// This 2 dont delete -> Future use
	Engine::GUID<MeshAsset> meshID{ 0 };

	//bool isInstance = true;
	int shaderType = (int)SHADERTYPE::PBR;
	
	//temporary index for current material
	Engine::GUID<MaterialAsset> materialGUID{0};

	bool operator == (const MeshRenderer& other) const {
		if ((meshID == other.meshID) &&
			(materialGUID == other.materialGUID)) {
			return true;
		}
		return false;
	}

	property_vtable();
};

property_begin_name(MeshRenderer, "MeshRenderer") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
	property_var(meshID).Name("Mesh"),
	property_var(materialGUID).Name("Material_ID")
} property_vend_h(MeshRenderer)


struct LightSource : Object
{
	bool enableShadow = true;
	//index for light type for serializing and de-serializing
	int lightType = (int)SPOT_LIGHT;	

	// Used in point & Spot
	Vector3 lightpos;

	// Used in directional & spot
	Vector3 direction;

	// Used only in Spot
	float inner_CutOff = 50.f;
	float outer_CutOff = 70.f;

	// Used for all
	float intensity = 10.f;
	Vector3 lightingColor{1.f, 1.f, 1.f };

	bool toRender = true; // Dirtybit 
	property_vtable()
};

	property_begin_name(LightSource, "LightSource") {
	property_parent(Object).Flags(property::flags::DONTSHOW),
	property_var(enableShadow).Name("EnableShadow"), 
	property_var(lightType).Name("lightType"),
	property_var(lightpos).Name("lightpos"),
	property_var(intensity).Name("Intensity"),
	property_var(direction).Name("Direction"),
	property_var(inner_CutOff).Name("Inner Cutoff"),
	property_var(outer_CutOff).Name("Outer Cutoff"),
	property_var(lightingColor).Name("Color")
} property_vend_h(LightSource)

struct SpriteRenderer : Object
	{
		bool WorldSpace = true;
		bool ColourPicked = false;
		bool IncludeAlpha = false;
		float AlphaMultiplier = 1.f;
		bool onHover = false;
		bool onClick = false; 

		Engine::GUID<TextureAsset> SpriteTexture {0};

		property_vtable()
	};

property_begin_name(SpriteRenderer, "SpriteRenderer")
{
	property_parent(Object).Flags(property::flags::DONTSHOW),
		property_var(WorldSpace).Name("World Space"),
		property_var(ColourPicked).Name("Colour Picker Mode"),
		property_var(IncludeAlpha).Name("Include Alpha (ColourPicking)"),
		property_var(AlphaMultiplier).Name("Alpha Scalar"),
		property_var(SpriteTexture).Name("SpriteTexture"),
} property_vend_h(SpriteRenderer)

struct Canvas : Object
{
	property_vtable()
};
property_begin_name(Canvas, "Canvas")
{
	property_parent(Object).Flags(property::flags::DONTSHOW),
		//property_var(WorldSpace).Name("World Space"),
		//property_var(SpriteTexture).Name("SpriteTexture"),
} property_vend_h(Canvas)


struct Trail {
		unsigned int count{0};
		std::deque<vec3> pos;
};

struct Particle : Object
{
	Particle() {}
	Particle(const vec3& position, const vec3& velocity, const float& acceleration, float lifetime, float scale)
		: position(position), velocity(velocity), acceleration(acceleration), lifetime(lifetime), scale(scale) {}
	vec3 position;
	vec3 velocity;
	vec3 direction;
	float acceleration;
	float lifetime;
	float scale; 
	float speed;
	float noiselifetime;
	Trail trails;
};

struct ParticleComponent : Object
{
	ParticleComponent() {}
	Engine::GUID<MeshAsset> meshID{ ASSET_CUBE };
	Engine::GUID<MaterialAsset> materialGUID{ 0 };
	//Engine::GUID<TextureAsset> ParticleTexture{ 0 };

	bool particleLooping{ false };
	bool is2D{ false };
	bool trailEnabled{ false };
	bool isLocalSpace{ false };

	int numParticles_{ 1 };
	int trailSize{ 0 };

	float angle{ 30.f }; // for directional particles
	float particleLifetime_{ 3.0f };
	float particleEmissionRate_{ 100.0f };
	float particleMinScale_{ 0.1f };
	float particleMaxScale_{ 1.0f };
	float particleScaleRate_{ 0.5f };
	float speed_{ 0.5f };
	float desiredLifetime{ 5.0f };
	float noiseMovement{ 0.f };
	float noisefrequency{ 0.f };

	Vector3 direction{0.f,0.f,0.f};
	std::vector<Particle> particles_;

	property_vtable();
};

property_begin_name(ParticleComponent, "ParticleComponent")
{
	property_var(meshID).Name("Mesh"),
	property_var(materialGUID).Name("Material"),
	//property_var(ParticleTexture).Name("Particle Texture"),
	property_var(isLocalSpace).Name("Local Space"),
	property_var(direction).Name("Particle Direction"),
	property_var(angle).Name("Angle of Direction"),
	property_var(numParticles_).Name("Number Of Particles"),
	property_var(particleLifetime_).Name("Particle Lifetime"),
	property_var(particleEmissionRate_).Name("Particle Emission Rate"),
	property_var(particleMinScale_).Name("Particle Min Scale"),
	property_var(particleMaxScale_).Name("Particle Max Scale"),
	property_var(particleScaleRate_).Name("Particle Scale Rate"),
	property_var(speed_).Name("Particle Speed"),
	property_var(noiseMovement).Name("Particle Noise Movement Percentage"),
	property_var(noisefrequency).Name("Particle Noise Frequency"),
	property_var(is2D).Name("2D particle"),
	property_var(trailEnabled).Name("Trailing"),
	property_var(trailSize).Name("Trail Size"),
	property_var(particleLooping).Name("Looping")

} property_vend_h(ParticleComponent)
//
//struct Particle : Object
//{
//	Particle() {}
//	Particle(const vec3& position, const vec3& velocity, const vec3& acceleration, float lifetime)
//		: position(position), velocity(velocity), acceleration(acceleration), lifetime(lifetime) {}
//	vec3 position;
//	vec3 velocity;
//	vec3 acceleration;
//	float lifetime;
//};

struct Button : Object
{
	//char* id;
	//int x, y, width, height; 
	Button() {}
	bool is_clicked;
	////void(*on_click)(void);
	int x, y, width, height;
	std::string label;
	std::function<void()> clickHandler;
	Button(int x, int y, int width, int height, std::string label, std::function<void()> clickHandler)
		: x(x), y(y), width(width), height(height), label(label), clickHandler(clickHandler) {}

	bool isClicked(int xPos, int yPos) {
		return (xPos >= x && xPos <= x + width && yPos >= y && yPos <= y + height);
	}

	bool on_click_callback(void)
	{
		printf("Button clicked!\n");
		return 0;
	}


};

struct ButtonComponent : Object
{

	int x = 10;
	int y = 10;
	int width = 100;
	int height = 50;
	std::vector<Button> button_;

	void Init();
	void Button_update(Button* button, int mouse_x, int mouse_y, bool left_mouse_button_clicked);

	property_vtable();
};

property_begin_name(ButtonComponent, "ButtonComponent")
{
	property_var(x).Name("Buttonx"),
		property_var(y).Name("Buttony"),
		property_var(height).Name("ButtonHeight"),
		property_var(width).Name("ButtonWidth")
		//property_var(height).Name("ButtonHeight") 


} property_vend_h(ButtonComponent)

struct NavMeshAgent : Object
	{
		NavMeshAgent()
		{
			mPather.ResetPather();
		}
		float mAgentSpeed = 0.f;
		std::deque<glm::vec3> mPoints;
		AStarPather mPather;
		property_vtable();
	};

	property_begin_name(NavMeshAgent, "NavMeshAgent")
	{
		property_var(mAgentSpeed).Name("Agent Speed")
	} property_vend_h(NavMeshAgent)

#pragma endregion

//Group to store all single component arrays together and accessed easily without
//declaring multiple variable names
template<typename... Ts>
struct SingleComponentsGroup
{
	constexpr SingleComponentsGroup(TemplatePack<Ts...>) {}
	SingleComponentsGroup() = default;

	//Get the array with a given type if it is in the template pack
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
using MultiComponentsArray = ObjectsBList<Component, MAX_ENTITIES>;

//Group to store all multi component arrays together and accessed easily without
//declaring multiple variable names
template<typename... Ts>
struct MultiComponentsGroup
{
	constexpr MultiComponentsGroup(TemplatePack<Ts...>) {}
	MultiComponentsGroup() = default;

	//Get the array with a given type if it is in the template pack
	template <typename T1>
	constexpr MultiComponentsArray<T1>& GetArray()
	{
		static_assert((std::is_same_v<T1, Ts> || ...), "TESTES");
		return std::get<MultiComponentsArray<T1>>(arrays);
	}
private:
	std::tuple<MultiComponentsArray<Ts>...> arrays;
};

//Component buffer to help with deletion of objections
template<typename... Ts>
struct ComponentsBuffer
{
	constexpr ComponentsBuffer(TemplatePack<Ts...>) {}
	ComponentsBuffer() = default;

	//Get the array with a given type if it is in the template pack
	template <typename T1>
	constexpr std::vector<T1*>& GetArray()
	{
		static_assert((std::is_same_v<T1, Ts> || ...), "Type not found in ArrayGroup");
		return std::get<std::vector<T1*>>(arrays);
	}
private:
	std::tuple<std::vector<Ts*>...> arrays;
};



//Template pack of components that entities can only have one of each
//Colliders have to be before rigidbodies
using ColliderComponentTypes = TemplatePack<BoxCollider, SphereCollider, CapsuleCollider>;

using PhysicsTypes = decltype(ColliderComponentTypes::Concatenate(TemplatePack<Rigidbody, CharacterController>()));

using ExclusionColliders = TemplatePack<BoxCollider, SphereCollider, CapsuleCollider, CharacterController>;

using ExclusionPhysics = TemplatePack<Rigidbody, CharacterController>;

using SingleComponentTypes = decltype(
	TemplatePack
	<
		Transform,
		Tag,
		Animator, 
		Camera, 
		MeshRenderer, 
		LightSource , 
		SpriteRenderer, 
		Canvas, 
		ParticleComponent, 
		NavMeshAgent,
		TextRenderer
	>::Concatenate(PhysicsTypes()));

//Template pack of components that entities can only have multiple of each
using MultiComponentTypes = TemplatePack<AudioSource, Script>;

//SingleComponentsGroup initialized with template pack to know the component types
using SingleComponentsArrays = decltype(SingleComponentsGroup(SingleComponentTypes()));
//MultiComponentsGroup initialized with template pack to know the component types
using MultiComponentsArrays = decltype(MultiComponentsGroup(MultiComponentTypes()));

//Template pack of all types of components
using AllComponentTypes = decltype(SingleComponentTypes().Concatenate(MultiComponentTypes()));

//Component types that can be displayed in the editor with default behaviour
using DisplayableComponentTypes = decltype(AllComponentTypes().Pop().Pop());

//All object types meaning entity + all component types
using AllObjectTypes = decltype(TemplatePack<Entity>::Concatenate(AllComponentTypes()));

//Get type for object types
using GetType = decltype(GetTypeGroup(AllObjectTypes()));

//Register components to Components type for easier lookup
template<typename T, typename... Ts>
static void RegisterComponentsHelper()
{
	ComponentTypes.emplace(GetType::Name<T>(), GetType::E<T>());
	if constexpr (sizeof...(Ts) != 0)
	{
		RegisterComponentsHelper<Ts...>();
	}
}

//Register components to Components type for easier lookup
template<typename... Ts>
static void RegisterComponents(TemplatePack<Ts...>)
{
	RegisterComponentsHelper<Ts...>();
}

//ComponentsBuffer initialized with template pack to know the component types 
using ComponentsBufferArray = decltype(ComponentsBuffer(AllComponentTypes()));

//Use for script fields when type is ambiguous but we have the type enum
//to check through every type T in an template pack to compare with its type enum
#define GENERIC_RECURSIVE(TYPE,FUNC_NAME,FUNC) \
template<typename T, typename... Ts>\
TYPE FUNC_NAME##Iter(size_t objType, void* pObject)\
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
		return FUNC_NAME##Iter<Ts...>(objType, pObject); \
	}\
	else\
	{\
		E_ASSERT(false, "Could not match type: ", objType); \
	}\
}\
template<typename T, typename... Ts>\
TYPE FUNC_NAME##Start( TemplatePack<T,Ts...>,size_t objType, void* pObject)\
{return FUNC_NAME##Iter<T,Ts...>(objType,pObject);}\
TYPE FUNC_NAME(size_t objType, void* pObject)\
{return FUNC_NAME##Start(AllObjectTypes(), objType,pObject);}\

//Field types template pack
using FieldTypes = TemplatePack<float, double, bool, char, short, int, int64_t, uint16_t, uint32_t, uint64_t, char*, Vector2, Vector3, Vector4, std::string>;
//All field types template pack that includes all objects and field types
using AllFieldTypes = decltype(FieldTypes::Concatenate(AllObjectTypes()));

//Get enum and name for fields
using GetFieldType = decltype(GetTypeGroup(AllFieldTypes()));

#endif // !COMPONENTS_H