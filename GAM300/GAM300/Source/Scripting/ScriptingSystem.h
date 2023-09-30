/*!***************************************************************************************
\file			scripting-system.h
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			10/03/2023

\brief
	This file holds the declaration of functions for scripting-system.cpp

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#ifndef SCRIPTING_SYSTEM_H
#define SCRIPTING_SYSTEM_H

#include "Core\SystemInterface.h"
#include <Core/events.h>
#include <Scene/Components.h>

#include <string>
#include <unordered_map>
#include <mutex>
#include <Core/FileTypes.h>
#include <vector>

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
	{ "System.String",				GetFieldType::E<std::string>()},
	{ "GlmSharp.vec2",				GetFieldType::E<Vector2>()},
	{ "GlmSharp.vec3",				GetFieldType::E<Vector3>()}
};


enum class CompilingState
{
	Compiling,
	SwapAssembly,
	Wait,
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
	std::unordered_map<std::string, MonoMethod*> mMethods;
	std::unordered_map<std::string, MonoClassField*> mFields;

};

ENGINE_SYSTEM(ScriptingSystem)
{
public:
	/**************************************************************************/
	/*!
		\brief
			Called at the start of engine runtime, starts a thread to 
			start compilation and track change of files
	*/
	/**************************************************************************/
	void Init();

	/**************************************************************************/
	/*!
		\brief
			Called every frame
	*/
	/**************************************************************************/
	void Update(float dt);

	/**************************************************************************/
	/*!
		\brief
			Called at the engine exit
	*/
	/**************************************************************************/
	void Exit();

	/**************************************************************************/
	/*!
		\brief
			Creates an instance of a monoClass
		\param mClass
			Class to create instance or clone from
		\return
			Pointer to the clone
	*/
	/**************************************************************************/

	template <typename... Args>
	MonoObject* InstantiateClass(MonoClass * mClass, Args&&... args);
	/**************************************************************************/
	/*!
		\brief
			Initializes mono by creating a root domain
	*/
	/**************************************************************************/
	void InitMono();

	/**************************************************************************/
	/*!
		\brief
			Cleans up mono and its domains
	*/
	/**************************************************************************/
	void ShutdownMono();

	/**************************************************************************/
	/*!
		\brief
			Cleans up mono and its domains
		\param mObj
			Instance to invoke from
		\param mMethod
			Method to invoke
		\param params
			Parameters to pass into mono function
	*/
	/**************************************************************************/
	MonoObject* invoke(MonoObject * mObj, MonoMethod * mMethod, void** params = nullptr);

	void InvokeMethod(Script & script,const std::string& method);


	/**************************************************************************/
	/*!
		\brief
			Reflects a gameObject of ID into C#
		\param _ID
			ID of gameObject to be reflected
	*/
	/**************************************************************************/
	MonoObject* CloneInstance(MonoObject* _instance);

	/*******************************************************************************
	/*!
	\brief
		Creates an instance of a given class
	\param _mClass
		Class to make object from
	\return
		MonoObject of a class
	*/
	/*******************************************************************************/
	MonoObject* CreateInstance(MonoClass* _mClass);

	/**************************************************************************/
	/*!
		\brief
			Creates a new file
		\param _name
			Name of the new script
	*/
	/**************************************************************************/
	std::string AddEmptyScript(const std::string& _name);

	/*******************************************************************************
	/*!
	\brief
		Finds a type in C# mono using a string and returns the monoType
	\param name
		Name of type to find
	\return
		MonoType found
	*/
	/*******************************************************************************/
	MonoType* GetMonoTypeFromName(std::string& name);

	/*******************************************************************************
	/*!
	\brief
		Converts a C string into a mono string
	\param str
		Str to convert into mono string
	\return
		Mono version of given string
	*/
	/*******************************************************************************/
	MonoString* CreateMonoString(const char* str);

	/**************************************************************************/
	/*!
		\brief
			Creates a collision data for scripts
		\param collided
			Rhs gameobject
		\param collidee
			Lhs gameObject
	*/
	/**************************************************************************/
	MonoObject* GetFieldMonoObject(MonoClassField* mField, MonoObject* mObject);

	/**************************************************************************/
	/*!
		\brief
			Reloads and updates script classes
	*/
	/**************************************************************************/
	void UpdateScriptClasses();

	/**************************************************************************/
	/*!
		\brief
			Creates an appDomain so that a new assembly can be loaded
	*/
	/**************************************************************************/
	MonoDomain* CreateAppDomain();
	/**************************************************************************/
	/*!
		\brief
			Reloads an assembly by creating a new domain
	*/
	/**************************************************************************/
	void SwapDll();
	/**************************************************************************/
	/*!
		\brief
			Work for thread to check for script files and recompile if needed
	*/
	/**************************************************************************/
	void RecompileThreadWork();
	/*******************************************************************************
	/*!
	*
	\brief
		Gets a field from a C# field using its name
	\param name
		Name of the field
	\param buffer
		Buffer to store the values, needs to be type casted
	\return
		False if operation failed, true if it was successful
	*/
	/*******************************************************************************/
	void GetFieldValue(MonoObject* instance, MonoClassField* mClassFiend,  Field& field);
	/*******************************************************************************
	/*!
	*
	\brief
		Sets a field from a C# field using its name
	\param name
		Name of the field
	\param value
		Value to write into C# memory space
	\return
		False if operation failed, true if it was successful
	*/
	/*******************************************************************************/
	void SetFieldValue(MonoObject* instance, MonoClassField* mClassFiend, Field& field);

	void CallbackScriptModified(FileTypeModifiedEvent<FileType::SCRIPT>* pEvent);

	bool IsScript(MonoClass* monoClass);

	void ThreadWork();

	//Updates by setting field values back into C#
	void CallbackScriptSetField(ScriptSetFieldEvent* pEvent);

	void CallbackScriptGetField(ScriptGetFieldEvent* pEvent);

	void CallbackScriptGetFieldNames(ScriptGetFieldNamesEvent* pEvent);
	/*******************************************************************************
	/*!
	*
	\brief
		Callback function when a scene is opened

	\param pEvent
		pointer to the relevant event 

	\return
		void
	*/
	/*******************************************************************************/
	void CallbackSceneChanging(SceneChangingEvent* pEvent);

	/*******************************************************************************
	/*!
	*
	\brief
		Callback function when preview is started

	\param pEvent
		pointer to the relevant event

	\return
		void
	*/
	/*******************************************************************************/
	void CallbackSceneStart(SceneStartEvent* pEvent);
	/*******************************************************************************
	/*!
	*
	\brief
		Callback function when preview is started

	\param pEvent
		pointer to the relevant event

	\return
		void
	*/
	/*******************************************************************************/
	void CallbackSceneCleanup(SceneCleanupEvent* pEvent);
	/*******************************************************************************
	/*!
	*
	\brief
		Callback function when preview is started

	\param pEvent
		pointer to the relevant event

	\return
		void
	*/
	/*******************************************************************************/
	void CallbackGetScriptNames(GetScriptNamesEvent* pEvent);


	void CallbackScriptCreated(ObjectCreatedEvent<Script>* pEvent);

	template <typename... Ts>
	void SubscribeObjectDestroyed(TemplatePack<Ts...>);

	template<typename T>
	void CallbackObjectDestroyed(ObjectDestroyedEvent<T>* pEvent);

	MonoObject* ReflectScript(Script& component, MonoObject* ref = nullptr);

	void ReflectFromOther(Scene& other);

	MonoImage* GetAssemblyImage();

	void InvokeAllScripts(const std::string& funcName);

	void UpdateReferences();


	using FieldMap = std::unordered_map<std::string, Field>;
	void CacheScripts();
	std::unordered_map<Handle, FieldMap> cacheFields;
	void LoadCacheScripts();

	//Mapping script to mono script
	using MonoScripts = std::unordered_map<Handle, MonoObject*>;

	std::unordered_map<std::string, ScriptClass> scriptClassMap;
	float timeUntilRecompile{0};
	CompilingState compilingState{ CompilingState::Wait };

	std::vector<Handle> reflectionQueue;

	std::unordered_map<Engine::UUID, MonoScripts> mSceneScripts;

	bool objectDestroyed = false;

	enum class LogicState
	{
		START,
		UPDATE,
		EXIT,
		CLEANUP,
		NONE
	};

	static std::unordered_map<LogicState, std::string> logicStateNames;

	LogicState logicState;

	std::atomic_bool ran;
};
#endif // !SCRIPTING_SYSTEM_H