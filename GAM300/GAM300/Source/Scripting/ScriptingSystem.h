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
#include <Scripting/script-fields.h>

#include <string>
#include <unordered_map>
#include <mutex>

extern "C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoType MonoType;
	typedef struct _MonoString MonoString;
}

#define MyScriptingSystem (*ScriptingSystem::Instance())

// Map of all the field types
static std::unordered_map<std::string, FieldType> fieldTypeMap =
{
	{ "System.Single",				FieldType::Float		},
	{ "System.Double",				FieldType::Double		},
	{ "System.Boolean",				FieldType::Bool			},
	{ "System.Char",				FieldType::Char			},
	{ "System.Int16",				FieldType::Short		},
	{ "System.Int32",				FieldType::Int			},
	{ "System.Int64",				FieldType::Long			},
	{ "System.UInt16",				FieldType::UShort		},
	{ "System.UInt32",				FieldType::UInt			},
	{ "System.UInt64",				FieldType::ULong		},
	{ "System.String",				FieldType::String		},
	{ "CopiumEngine.Vector2",		FieldType::Vector2		},
	{ "CopiumEngine.Vector3",		FieldType::Vector3		},
	{ "CopiumEngine.GameObject",	FieldType::GameObject	},
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
	ScriptClass(const std::string& _name, MonoClass* _mClass);
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
	MonoObject* instantiateClass(MonoClass * mClass);
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


	/**************************************************************************/
	/*!
		\brief
			Reflects a gameObject of ID into C#
		\param _ID
			ID of gameObject to be reflected
	*/
	/**************************************************************************/
	MonoObject* cloneInstance(MonoObject* _instance);

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
	MonoObject* createInstance(MonoClass* _mClass);

	/**************************************************************************/
	/*!
		\brief
			Creates a new file
		\param _name
			Name of the new script
	*/
	/**************************************************************************/
	std::string addEmptyScript(const std::string& _name);

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
	MonoType* getMonoTypeFromName(std::string& name);

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
	MonoString* createMonoString(const char* str);

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
	MonoObject* getFieldMonoObject(MonoClassField* mField, MonoObject* mObject);

	/**************************************************************************/
	/*!
		\brief
			Checks if this script is a scriptable object

		\param name
			name of the object to be checked

		\return 
			true if script is a scriptable object
			false if its not
	*/
	/**************************************************************************/
	bool isScriptableObject(const std::string& name);

	/**************************************************************************/
	/*!
		\brief
			Checks if the script is indeed a script

		\param name
			name of the script to check

		\return
			true if script is indeed a script
			false if not

	*/
	/**************************************************************************/
	bool isScript(const std::string& name);


	/**************************************************************************/
	/*!
		\brief
			Reloads and updates script classes
	*/
	/**************************************************************************/
	void updateScriptClasses();

	/**************************************************************************/
	/*!
		\brief
			Checks for addition or deletion of script files and updates a list
			of loaded script files
	*/
	/**************************************************************************/
	void updateScriptFiles();
	/**************************************************************************/
	/*!
		\brief
			Checks for any modified script files recompiles if there were
			any modified script files
	*/
	/**************************************************************************/
	void tryRecompileDll();
	/**************************************************************************/
	/*!
		\brief
			Creates an appDomain so that a new assembly can be loaded
	*/
	/**************************************************************************/
	void createAppDomain();
	/**************************************************************************/
	/*!
		\brief
			Deletes an appDomain and any assemblies loaded in it
	*/
	/**************************************************************************/
	void unloadAppDomain();
	/**************************************************************************/
	/*!
		\brief
			Reloads an assembly by creating a new domain
	*/
	/**************************************************************************/
	void swapDll();
	/**************************************************************************/
	/*!
		\brief
			Work for thread to check for script files and recompile if needed
	*/
	/**************************************************************************/
	void RecompileThreadWork();
	/**************************************************************************/
	/*!
		\brief
			Checks whether a script is loaded as a File
		\param filePath
			Filepath to check whether the file is loaded
	*/
	/**************************************************************************/
	bool scriptIsLoaded(const std::filesystem::path& filePath);

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
	void GetFieldValue(MonoObject* instance, MonoClassField* mClassFiend,  Field& field, void* container);

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
	void SetFieldValue(MonoObject* instance, MonoClassField* mClassFiend, Field& field, const void* value);

	template<typename T>
	void SetFieldReference(MonoObject* instance, MonoClassField* mClassFiend, T* reference);

	void CallbackScriptModified(FileModifiedEvent<FileType::SCRIPT>* pEvent);

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
	//void CallbackSceneChanging(SceneChangingEvent* pEvent);
	/*******************************************************************************
	/*!
	*
	\brief
		Callback function when a component is reflected

	\param pEvent
		pointer to the relevant event

	\return
		void
	*/
	/*******************************************************************************/
	//template <typename T>
	//void CallbackReflectComponent(ReflectComponentEvent<T>* pEvent);
	/*******************************************************************************
	/*!
	*
	\brief
		Callback function when a script is invoked

	\param pEvent
		pointer to the relevant event

	\return
		void
	*/
	/*******************************************************************************/
	//void CallbackScriptInvokeMethod(ScriptInvokeMethodEvent* pEvent);
	/*******************************************************************************
	/*!
	*
	\brief
		Callback function when a field is set

	\param pEvent
		pointer to the relevant event

	\return
		void
	*/
	/*******************************************************************************/
	//void CallbackScriptSetField(ScriptSetFieldEvent* pEvent);
	/*******************************************************************************
	/*!
	*
	\brief
		Callback function when a field is accessed

	\param pEvent
		pointer to the relevant event

	\return
		void
	*/
	/*******************************************************************************/
	//void CallbackScriptGetField(ScriptGetFieldEvent* pEvent);
	/*******************************************************************************
	/*!
	*
	\brief
		Callback function when a field reference is set

	\param pEvent
		pointer to the relevant event

	\return
		void
	*/
	/*******************************************************************************/
	//template<typename T>
	//void CallbackScriptSetFieldReference(ScriptSetFieldReferenceEvent<T>* pEvent);
	/*******************************************************************************
	/*!
	*
	\brief
		Callback when adding a new blank script to the project

	\param pEvent
		Pointer to event with script name
	*/
	/*******************************************************************************/
	//void CallbackScriptNew(ScriptNewEvent* pEvent);

	/*******************************************************************************
	/*!
	*
	\brief
		Callback function when method name is accessed

	\param pEvent
		pointer to the relevant event

	\return
		void
	*/
	/*******************************************************************************/
	//void CallbackScriptGetMethodNames(ScriptGetMethodNamesEvent* pEvent);
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
	//void CallbackStartPreview(StartPreviewEvent* pEvent);
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
	//void CallbackStopPreview(StopPreviewEvent* pEvent);
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
	//void CallbackScriptGetNames(ScriptGetNamesEvent* pEvent);

	//void CallbackReflectGameObject(ReflectGameObjectEvent* pEvent);

	//Event subscription helper
	//template<typename T, typename... Ts>
	//void SubscribeComponentBasedCallbacks(TemplatePack<T, Ts...> pack);

	/*******************************************************************************
	/*!
	*
	\brief
		Reflects GameObject into C#

	\param gameObject
		Object to reflect

	\return
		Mono Instance of gameObject
	*/
	/*******************************************************************************/
	//MonoObject* ReflectGameObject(GameObject& gameObject);
	/*******************************************************************************
	/*!
	*
	\brief
		Reflects component into C#

	\param component
		Object to reflect

	\return
		Mono Instance of gameObject
	*/
	/*******************************************************************************/
	template <typename T>
	MonoObject* ReflectComponent(T& component);
	template <>
	MonoObject* ReflectComponent(Script& component);


	//using MonoGameObjects = std::unordered_map<UUID, MonoObject*>;
	//using MonoComponents = std::unordered_map<UUID, MonoObject*>;

	//std::unordered_map<std::string, ScriptClass> scriptClassMap;
	//std::unordered_map<std::string, ScriptClass> scriptableObjectClassMap;
	//MonoGameObjects mGameObjects;
	//MonoComponents mComponents;
	//std::unordered_map<MonoType*, ComponentType> reflectionMap;
	//std::list<File>& scriptFiles;
	//std::map<std::string, std::map<std::string,ScriptableObject>> scriptableObjects;
	std::mutex compilingStateReadable;
	float timeUntilRecompile{0};
	//std::vector<uint32_t> gcHandles;
	CompilingState compilingState{ CompilingState::Wait };
	//bool inPlayMode{false};
};

	/*******************************************************************************
	/*!
	*
	\brief
		Callback function for when a field reference is set

	\param pEvent
		ptr to the relevant event

	\return
		void
	*/
	/*******************************************************************************/
	//template<typename T>
	//void ScriptingSystem::CallbackScriptSetFieldReference(ScriptSetFieldReferenceEvent<T>* pEvent)
	//{
	//	MonoObject* mScript = ReflectComponent(pEvent->script);
	//	COPIUM_ASSERT(!mScript, std::string("MONO OBJECT OF ") + pEvent->script.name + std::string(" NOT LOADED"));
	//	ScriptClass& scriptClass{ scriptClassMap[pEvent->script.name] };
	//	MonoClassField* mClassField{ scriptClass.mFields[pEvent->fieldName] };
	//	COPIUM_ASSERT(!mClassField, std::string("FIELD ") + pEvent->fieldName + " COULD NOT BE FOUND IN SCRIPT " + pEvent->script.name);
	//	SetFieldReference<T>(mScript, mClassField,pEvent->reference);
	//}
#endif // !SCRIPTING_SYSTEM_H