/*!***************************************************************************************
\file			ScriptingSystem.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\date			10/03/2023

\brief
	This file holds the declaration of functions for the scripting system using Mono C#

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#ifndef SCRIPTING_SYSTEM_H
#define SCRIPTING_SYSTEM_H

#include "Core\SystemInterface.h"
#include <Core/Events.h>
#include <Scene/Components.h>

#include <string>
#include <unordered_map>
#include <mutex>
#include <Core/FileTypes.h>
#include <vector>
#include <queue>

struct Script;
struct Entity;
struct Handle;

extern "C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoType MonoType;
	typedef struct _MonoString MonoString;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoDomain MonoDomain;
	typedef struct _MonoImage MonoImage;
}

#define SCRIPTING ScriptingSystem::Instance()

// Map of all the field types
static std::unordered_map<std::string, size_t> fieldTypeMap =
{
	{ "System.Single",				GetFieldType::E<float>()},
	{ "System.Double",				GetFieldType::E<double>()},
	{ "System.Boolean",				GetFieldType::E<bool>()},
	{ "System.Char",				GetFieldType::E<char>()},
	{ "System.Int16",				GetFieldType::E<short>()},
	{ "System.Int32",				GetFieldType::E<int>()},
	{ "System.Int64",				GetFieldType::E<int64_t>()},
	{ "System.UInt16",				GetFieldType::E<uint16_t>()},
	{ "System.UInt32",				GetFieldType::E<uint32_t>()},
	{ "System.UInt64",				GetFieldType::E<uint64_t>()},
	{ "System.String",				GetFieldType::E<char*>()},
	{ "GlmSharp.vec2",				GetFieldType::E<Vector2>()},
	{ "GlmSharp.vec3",				GetFieldType::E<Vector3>()}
};


enum class CompilingState
{
	Compiling,
	SwapAssembly,
	Wait,
};

enum class LogicState
{
	START,
	UPDATE,
	EXIT,
	CLEANUP,
	NONE
};


namespace DefaultMethodTypes
{
	enum
	{
		Awake = 0,
		Start,
		Update,
		LateUpdate,
		ExecuteCoroutines,
		OnCollisionEnter,
		OnCollisionStay,
		OnCollisionExit,
		OnTriggerEnter,
		OnTriggerStay,
		OnTriggerExit,
		SIZE
	};
}

//Pass this into scripts
template<typename T>
struct ScriptObject
{
	ScriptObject(Object* object)
	{
		static_assert(std::is_base_of_v<Object, T>);
		memoryAddress = reinterpret_cast<size_t>(object) + memOffset;
	}

	operator T& ()
	{
		return *reinterpret_cast<T*>(memoryAddress - (memOffset));
	}

	operator Object* ()
	{
		if (memoryAddress == 0)
			return nullptr;
		return reinterpret_cast<Object*>(memoryAddress - (memOffset));
	}

	void* operator& ()
	{
		return reinterpret_cast<void*>(memoryAddress);
	}
private:
	static size_t constexpr memOffset = sizeof(Object) - 16;
	size_t memoryAddress;
};

template<>
struct ScriptObject<Script>
{
	ScriptObject(Object* object);
	operator Script& ();
	operator Script* ();

private:
	MonoObject* script;
};

struct ScriptClass
{
	ScriptClass() = default;
	/**************************************************************************/
	/*!
		\brief
			Stores a monoClass and retrieves all the default functions

		\param _mClass
			Class to load functions from
	*/
	/**************************************************************************/
	ScriptClass(MonoClass* _mClass);
	MonoClass* mClass{};
	//std::unordered_map<std::string, MonoMethod*> mMethods;
	std::unordered_map<std::string, MonoClassField*> mFields;

	//Collision
	MonoMethod* DefaultMethods[DefaultMethodTypes::SIZE]{nullptr};
};

ENGINE_SYSTEM(ScriptingSystem)
{
public:
	//Starts another thread to invoke scripting behaviour
	
	void Init();

	//Synchronises with the the other thread to ensure it only runs onces per frame
	void Update(float dt);

	//Does nothing
	void Exit();

	//Creates an instance of a monoClass
	template <typename... Args>
	MonoObject* InstantiateClass(MonoClass * mClass, Args&&... args);

	//Initializes mono by creating a root domain
	void InitMono();

	std::string GetScriptName(Script& script);

	//Cleans up mono and its domains
	void ShutdownMono();

	//Invokes a mono method on an instance
	MonoObject* Invoke(MonoObject * mObj, MonoMethod * mMethod, void** params = nullptr);

	//Invokes a mono method on a script
	void InvokeMethod(Script & script, size_t methodType);

	//Updates all script classes after reloading dll
	void UpdateScriptClasses();

	//Creates a new app domain
	MonoDomain* CreateAppDomain();

	void UnloadAppDomain();

	//Mono String from string
	MonoString* CreateMonoString(const std::string&);

	Handle GetScriptHandle(MonoObject * script);

	//Reloads an assembly by creating a new domain
	void SwapDll();

	//Recompiles assembly
	void RecompileThreadWork();

	//Gets data from C# field into given field
	void GetFieldValue(MonoObject* instance, MonoClassField* mClassFiend,  Field& field);

	//Sets data from field into C# field
	void SetFieldValue(MonoObject* instance, MonoClassField* mClassFiend, Field& field);

	void GetFieldValue(Script & script, const std::string & fieldName, Field & field);


	void SetFieldValue(Script & script, const std::string & fieldName, Field & field);

	//Checks whether a mono class is script
	bool IsScript(MonoClass* monoClass);

	//Thread work to run update loop of logic in play mode
	//recompile/swap assembly while in editor mode
	void ThreadWork();

	//Callback when a script file is modified to start recompilation
	void CallbackScriptModified(FileTypeModifiedEvent<FileType::SCRIPT>*pEvent);

	//Callback when a script file is modified to start recompilation
	void CallbackScriptSetField(ScriptSetFieldEvent* pEvent);

	//Callback when a script file is modified to start recompilation
	void CallbackScriptGetField(ScriptGetFieldEvent* pEvent);
	
	//Callback to get all script fields
	void CallbackScriptGetFieldNames(ScriptGetFieldNamesEvent* pEvent);
		
	//Callback function when a scene is loaded
	void CallbackSceneChanging(SceneChangingEvent* pEvent);

	//Callback function when a scene has just started
	void CallbackSceneStart(SceneStartEvent* pEvent);

	//Callback function when a scene is about to end
	void CallbackSceneCleanup(SceneCleanupEvent* pEvent);

	//Callback function to when a script is created
	void CallbackScriptCreated(ObjectCreatedEvent<Script>* pEvent);

	//Callback function to when a script is created
	void CallbackLoadScene(LoadSceneEvent*pEvent);

	void CallbackCollisionEnter(ContactAddedEvent* pEvent);

	void CallbackCollisionExit(ContactRemovedEvent* pEvent);

	void CallbackTriggerEnter(TriggerEnterEvent * pEvent);

	void CallbackTriggerExit(TriggerRemoveEvent * pEvent);

	//Helper to subscribe to all objects deletion
	template <typename... Ts>
	void SubscribeObjectDestroyed(TemplatePack<Ts...>);

	//Callback function to when a object is deleted
	template<typename T>
	void CallbackObjectDestroyed(ObjectDestroyedEvent<T>* pEvent);


	void CallbackApplicationExit(ApplicationExitEvent*pEvent);

	//Get the script if it is reflected already, 
	//else instantiate a MonoObject and store it
	MonoObject* ReflectScript(Script& component, MonoObject* ref = nullptr);

	//Reflect from another scene, copy construction of Sceene
	void ReflectFromOther(Scene& other);

	//Gets the assembly image
	MonoImage* GetAssemblyImage();

	//Tell all scripts to invoke a function if they are active
	void InvokeAllScripts(size_t methodType);

	//Updates all the references in the scripts when an object is deleted
	void UpdateReferences();

	//Cache scripts to prepare for DLL reloading
	void CacheScripts();
	
	//Load cache back into scripts after dll is reloaded
	void LoadCacheScripts();

	//Mapping script to mono script
	using MonoScripts = std::unordered_map<Handle, MonoObject*>;

	//Field name to field
	using FieldMap = std::unordered_map<std::string, Field>;

	using GC_Handle = uint32_t;
	using GC_Handles = std::vector<GC_Handle>;

	//Script guid to script class
	std::unordered_map<Engine::GUID<ScriptAsset>, ScriptClass> scriptClassMap;
	//Scene uuid to mono scripts
	std::unordered_map<Engine::UUID, MonoScripts> mSceneScripts;
	std::unordered_map<Engine::UUID, GC_Handles> mSceneHandles;
	//Cached fields
	std::unordered_map<Handle, FieldMap> cacheFields;

	void InvokePhysicsEvent(size_t colType, PhysicsComponent& rb1, PhysicsComponent& rb2);

	IEvent* scriptingEvent;

	CompilingState compilingState{ CompilingState::Wait };
	LogicState logicState;
	std::thread::id SCRIPTING_THREAD_ID;
	float timeUntilRecompile{ 0 };
	std::atomic_bool ran;

	std::map<std::type_index, IEventHandler*> events;
	template<class EventType>
	void Subscribe(void(ScriptingSystem::* memberFunction)(EventType*));
};
#endif // !SCRIPTING_SYSTEM_H