/*!***************************************************************************************
\file			ScriptingSystem.cpp
\project
\author			Zacharie Hong

\par			Course: GAM300
\date			10/03/2023

\brief
	This file contains the function definitions for the scripting system.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#pragma warning( disable : 26110 )
#pragma warning( disable : 26111 )
#pragma warning( disable : 4996 )
#include "Precompiled.h"
#include "Scripting/ScriptingSystem.h"
#include "Scripting/Compiler.h"
#include "Utilities/ThreadPool.h"
#include "Core/SystemInterface.h"
#include "Scripting/ScriptWrappers.h"
#include <Scene/SceneManager.h>
#include <Core/EventsManager.h>
#include <Scene/components.h>

#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include <mono/jit/jit.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/exception.h>
#include <mono/metadata/environment.h>

#include <string.h>

#define SECONDS_TO_RECOMPILE 1.f

#define TEXT_BUFFER_SIZE 2048

#define WAIT_FOR_COMPILATION while (compilingState != CompilingState::Wait) if (compilingState == CompilingState::SwapAssembly) SwapDll()

#define SCRIPT_METHOD(mClass ,methodName, paramCount) DefaultMethods[DefaultMethodTypes::methodName] = mono_class_get_method_from_name(mClass, #methodName, paramCount);

namespace
{
	MonoDomain* mRootDomain{ nullptr };		//JIT RUNTIME DOMAIN
	MonoDomain* mAppDomain{ nullptr };		//APP DOMAIN
	MonoAssembly* mCoreAssembly{ nullptr };	//ASSEMBLY OF SCRIPTS.DLL
	MonoImage* mAssemblyImage{ nullptr };	//LOADED IMAGE OF SCRIPTS.DLL
	MonoClass* mScript{ nullptr };
	MonoVTable* mTimeVtable{ nullptr };
	MonoClassField* mTimeDtField{ nullptr };
	MonoString* mStringBuffer{ nullptr };
}

namespace Utils
{
	static char* readBytes(const std::string& filepath, uint32_t* outSize)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream) return nullptr;
		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = (uint32_t)(end - stream.tellg());
		if (size == 0) return nullptr;
		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();
		*outSize = size;
		return buffer;
	}

	static MonoAssembly* loadAssembly(const std::string& assemblyPath)
	{
		uint32_t fileSize = 0;
		char* fileData = readBytes(assemblyPath, &fileSize);

		// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			// const char* errorMessage = mono_image_strerror(status);
			// Log some error message using the errorMessage data
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);

		mono_image_close(image);

		// Don't forget to free the file data
		delete[] fileData;

		return assembly;
	}

	size_t monoTypeToFieldType(MonoType* monoType)
	{
		std::string typeName = mono_type_get_name_full(monoType, MONO_TYPE_NAME_FORMAT_FULL_NAME);
		auto it = fieldTypeMap.find(typeName);
		if (it == fieldTypeMap.end())
		{
			if (mono_class_get_parent(mono_class_from_mono_type(monoType)) == mScript)
				return GetType::E<Script>();
			auto iter{ monoComponentToType.find(monoType) };
			for (auto& pair : monoComponentToType)
			{
				if (mono_type_get_name(pair.first) == typeName)
					return pair.second;
			}
			return AllFieldTypes::Size();
		}
		return (size_t)it->second;
	}
}

#pragma region Struct ScriptMethods
	ScriptClass::ScriptClass(MonoClass* _mClass) :
		mClass{ _mClass }
	{
		//void* methodIterator = nullptr;
		//while (MonoMethod* method = mono_class_get_methods(_mClass, &methodIterator))
		//{
		//	mMethods[mono_method_get_name(method)] = method;
		//}

		SCRIPT_METHOD(mClass, Awake, 0);
		SCRIPT_METHOD(mClass, Start, 0);
		SCRIPT_METHOD(mClass, Update, 0);
		SCRIPT_METHOD(mClass, LateUpdate, 0);

		SCRIPT_METHOD(mClass, OnCollisionEnter, 1);
		SCRIPT_METHOD(mClass, OnCollisionStay, 1);
		SCRIPT_METHOD(mClass, OnCollisionExit, 1);
		SCRIPT_METHOD(mClass, OnTriggerEnter, 1);
		SCRIPT_METHOD(mClass, OnTriggerStay, 1);
		SCRIPT_METHOD(mClass, OnTriggerExit, 1);

		void* iterator = nullptr;
		while (MonoClassField* field = mono_class_get_fields(mClass, &iterator))
		{
			std::string fieldName = mono_field_get_name(field);
			uint32_t flags = mono_field_get_flags(field);
			if (flags & MONO_FIELD_ATTR_STATIC || flags & MONO_FIELD_ATTR_NOT_SERIALIZED)
				continue;
			if (flags & MONO_FIELD_ATTR_PUBLIC)
			{
				MonoType* type = mono_field_get_type(field);
				size_t fieldType = Utils::monoTypeToFieldType(type);
				if (fieldType < AllFieldTypes::Size())
				{
					mFields[fieldName] = field;
				}
			}
		}
	}

#pragma endregion

#pragma region ScriptObject<Script>
ScriptObject<Script>::ScriptObject(Object* object)
{
	Scene& scene{MySceneManager.GetCurrentScene()};
	script =  SCRIPTING.mSceneScripts[scene.uuid][*object];
}

ScriptObject<Script>::operator Script& ()
{
	Scene& scene{ MySceneManager.GetCurrentScene() };
	Handle handle{ SCRIPTING.GetScriptHandle(script) };
	return scene.Get<Script>(handle);
}

ScriptObject<Script>::operator Script* ()
{
	Scene& scene{ MySceneManager.GetCurrentScene() };
	Handle handle{ SCRIPTING.GetScriptHandle(script) };
	return &scene.Get<Script>(handle);
}


ScriptObject<Script>::operator ScriptObject<Object>()
{
	return *reinterpret_cast<ScriptObject<Object>*>(this);
}

#pragma endregion

MonoImage* ScriptingSystem::GetAssemblyImage()
	{
		return mAssemblyImage;
	}

void ScriptingSystem::RecompileThreadWork()
{
	PRINT("RECOMPLING START\n");
	compilingState = CompilingState::Compiling;
	Utils::CompileDll();
	compilingState = CompilingState::SwapAssembly;
	PRINT("RECOMPLING END\n");
}

void ScriptingSystem::Init()
{
	#ifndef _BUILD
		EVENTS.Subscribe(this, &ScriptingSystem::CallbackScriptModified);
	#endif
	Subscribe(&ScriptingSystem::CallbackScriptSetField);
	Subscribe(&ScriptingSystem::CallbackScriptGetField);
	Subscribe(&ScriptingSystem::CallbackScriptGetFieldNames);

	Subscribe(&ScriptingSystem::CallbackSceneStart);
	Subscribe(&ScriptingSystem::CallbackSceneStop);
	Subscribe(&ScriptingSystem::CallbackSceneChanging);
	Subscribe(&ScriptingSystem::CallbackScriptCreated);
	SubscribeObjectDestroyed(AllObjectTypes());

	Subscribe(&ScriptingSystem::CallbackCollisionEnter);
	Subscribe(&ScriptingSystem::CallbackCollisionExit);
	Subscribe(&ScriptingSystem::CallbackTriggerEnter);
	Subscribe(&ScriptingSystem::CallbackTriggerExit);

	InitMono();
	SwapDll();
	if (mCoreAssembly == nullptr)
	{
		RecompileThreadWork();
		SwapDll();
	}
}

template<class EventType>
void ScriptingSystem::Subscribe(void (ScriptingSystem::* memberFunction)(EventType*))
{
	//events[typeid(EventType)] = new MemberFunctionHandler<ScriptingSystem, EventType>(this, memberFunction);
	EVENTS.Subscribe(this, memberFunction);
}

void ScriptingSystem::Update(float dt)
{
	if (playMode)
	{
		mono_field_static_set_value(mTimeVtable, mTimeDtField, &dt);
		InvokeAllScripts(DefaultMethodTypes::Update);
		InvokeAllScripts(DefaultMethodTypes::LateUpdate);
		InvokeAllScripts(DefaultMethodTypes::ExecuteCoroutines);
		return;
	}
#ifndef _BUILD
	if (timeUntilRecompile > 0)
	{
		timeUntilRecompile -= dt;
		if (timeUntilRecompile <= 0)
			THREADS.EnqueueTask([this] {RecompileThreadWork(); });
	}
	else if (compilingState == CompilingState::SwapAssembly)
	{
		SwapDll();
	}
#endif
}

void ScriptingSystem::Exit() 
{
	UnloadAppDomain();
	ShutdownMono();
}

template <typename... Args>
MonoObject* ScriptingSystem::InstantiateClass(MonoClass* mClass, Args&&... args)
{
	E_ASSERT(mAppDomain, "MONO APP DOMAIN NOT LOADED");
	E_ASSERT(mClass, "MONO CLASS NOT LOADED");
	MonoObject* tmp = mono_object_new(mAppDomain, mClass);
	mSceneHandles[MySceneManager.GetCurrentScene().uuid].push_back(mono_gchandle_new(tmp,true));
	mono_runtime_object_init(tmp);
	return tmp;
}

MonoString* ScriptingSystem::CreateMonoString(const std::string& str)
{
	if (!mAppDomain)
	{
		E_ASSERT(mAppDomain,"APP DOMAIN NOT LOADED");
	}
	return mono_string_new(mAppDomain, str.c_str());
}

void ScriptingSystem::UpdateScriptClasses()
{
	scriptClassMap.clear();
	const MonoTableInfo* table_info = mono_image_get_table_info(mAssemblyImage, MONO_TABLE_TYPEDEF);
	int rows = mono_table_info_get_rows(table_info);
	for (int i = 0; i < rows; i++)
	{
		MonoClass* _class = nullptr;
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
		const char* name = mono_metadata_string_heap(mAssemblyImage, cols[MONO_TYPEDEF_NAME]);
		const char* name_space = mono_metadata_string_heap(mAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
		_class = mono_class_from_name(mAssemblyImage, name_space, name);
		if (!_class)
			continue;
		if (mono_class_get_parent(_class) == mScript)
		{
			GetAssetsEvent<ScriptAsset> e;
			EVENTS.Publish(&e);
			for (auto& pair : *e.pAssets)
			{
				if (pair.second.mFilePath.stem() == name)
				{
					scriptClassMap[pair.first] = ScriptClass{ _class };

				}
			}
		}
	}
}

void ScriptingSystem::InitMono()
{
	mono_set_assemblies_path("Mono");
	mRootDomain = mono_jit_init("JITRuntime");
}

void ScriptingSystem::ShutdownMono()
{
	mono_jit_cleanup(mRootDomain);
	mRootDomain = nullptr;
}

MonoDomain* ScriptingSystem::CreateAppDomain()
{
	std::string name{ "AppDomain" };
	return mono_domain_create_appdomain(name.data(), nullptr);
}

void ScriptingSystem::ReflectFromOther(Scene& other)
{
	Scene& currScene{ MySceneManager.GetCurrentScene() };
	MonoScripts& otherScripts{ mSceneScripts[other.uuid] };
	for (auto& component : currScene.GetArray<Script>())
	{
		if (component.state == DELETED) continue;
		if (otherScripts.find(component) != otherScripts.end())
		{
			ReflectScript(component,otherScripts[component]);
		}
		else
		{
			ReflectScript(component);
		}
	}
}

void ScriptingSystem::InvokeAllScripts(size_t methodType)
{
	Scene& scene = MySceneManager.GetCurrentScene();
	auto& scriptsArray = scene.GetArray<Script>();
	for (auto it = scriptsArray.begin();it != scriptsArray.end();++it)
	{
		if (!it.IsActive())
			continue;
		Script& script = *it;
		if (!scene.IsActive(scene.Get<Entity>(script)))
			continue;
		InvokeMethod(script, methodType);
	}
}

void ScriptingSystem::UpdateReferences()
{
	PRINT("Update References\n");
	Scene& scene = MySceneManager.GetCurrentScene();
	for (auto& script : scene.GetArray<Script>())
	{
		if (script.state == DELETED) continue;

		MonoObject* mS = ReflectScript(script);
		ScriptClass& scriptClass = scriptClassMap[script.scriptId];
		for (auto& pair : scriptClass.mFields)
		{
			size_t fType = Utils::monoTypeToFieldType(mono_field_get_type(pair.second));
			if (fType < AllObjectTypes::Size())
			{
				Object* pObject{};
				Field field{ fType,sizeof(void*),&pObject};
				GetFieldValue(mS, pair.second, field);
				if (!pObject)
					continue;
				Handle handle{ (Handle)(*pObject)};
				if (scene.HasHandle(fType, &handle))
					continue;
				pObject = nullptr;
				SetFieldValue(mS, pair.second, field);
			}
		}
	}
}


template <typename... Ts>
void ScriptingSystem::SubscribeObjectDestroyed(TemplatePack<Ts...>)
{
	(Subscribe(&ScriptingSystem::CallbackObjectDestroyed<Ts>), ...);
}

template<typename T>
void ScriptingSystem::CallbackObjectDestroyed(ObjectDestroyedEvent<T>* pEvent)
{
	//SCRIPT_THREAD_EVENT(pEvent);
	if (mAppDomain)
		UpdateReferences();
}

Handle ScriptingSystem::GetScriptHandle(MonoObject* script)
{
	E_ASSERT(mScript, "Mono script is nullptr!");
	Handle handle{};
	mono_field_get_value
	(
		script,
		mono_class_get_field_from_name(mScript, "euid"),
		&handle.euid
	);
	mono_field_get_value
	(
		script,
		mono_class_get_field_from_name(mScript, "uuid"),
		&handle.uuid
	);
	return handle;
}


void ScriptingSystem::UnloadAppDomain()
{
	if (!mAppDomain)
		return;
	for (auto& sceneScripts : mSceneScripts)
	{
		for (auto& scriptPair : sceneScripts.second)
		{
			MonoClass* _class = mono_object_get_class(scriptPair.second);
			if (!_class || mono_class_get_parent(_class) != mScript)
				continue;
			mono_object_get_vtable(scriptPair.second);
			MonoVTable* vTable = mono_class_vtable(mAppDomain, _class);
			void* sfieldIterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(mScript, &sfieldIterator))
			{
				mono_field_set_value(scriptPair.second, field, nullptr);
			}

			void* fieldIterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(_class, &fieldIterator))
			{
				uint32_t flags = mono_field_get_flags(field);
				if (flags & MONO_FIELD_ATTR_STATIC)
				{
					mono_field_static_set_value(vTable, field, nullptr);
				}
				else
				{
					mono_field_set_value(scriptPair.second, field, nullptr);
				}
			}
		}
		sceneScripts.second.clear();
	}

	for (auto& pair : mSceneHandles)
	{
		for (GC_Handle handle : pair.second)
		{
			mono_gchandle_free(handle);
		}
	}
	mSceneHandles.clear();

	mono_domain_set(mRootDomain, false);
	PRINT("Unloading app domain\n");
	mono_domain_unload(mAppDomain);
	PRINT("App domain unloaded\n");
	mAppDomain = nullptr;
}

void ScriptingSystem::CacheScripts()
{
	PRINT("CACHING______________________\n");
	if (!mAppDomain)
		return;
	Scene& currScene{ MySceneManager.GetCurrentScene() };
	MonoScripts& mScripts = mSceneScripts[currScene.uuid];
	for (auto& scriptPair : mScripts)
	{
		Script& script{ currScene.Get<Script>(scriptPair.first) };
		ScriptClass& scriptClass{ scriptClassMap[script.scriptId] };
		//Reset fieldtype and buffer if the type was different
		for (auto& pair : scriptClass.mFields)
		{
			int alignment{};
			MonoType* mType = mono_field_get_type(pair.second);
			size_t fType = Utils::monoTypeToFieldType(mType);
			int fieldSize = mono_type_size(mType, &alignment);
			//POINTER
			if (fType < AllObjectTypes::Size())
			{
				fieldSize = sizeof(ScriptObject<Object>);
			}
			else if (fType == GetFieldType::E<char*>())
			{
				fieldSize = TEXT_BUFFER_SIZE;
			}
			FieldMap& fMap = cacheFields[script];
			fMap.emplace(pair.first, Field{ AllFieldTypes::Size(), (size_t)fieldSize});
			GetFieldValue(scriptPair.second, pair.second, fMap[pair.first]);
		}
	}
}

void ScriptingSystem::LoadCacheScripts()
{
	if (!MySceneManager.HasScene())
		return;
	Scene& currScene{ MySceneManager.GetCurrentScene() };
	for (auto& script : currScene.GetArray<Script>())
	{
		MonoObject* mS = ReflectScript(script);
		ScriptClass& scriptClass{ scriptClassMap[script.scriptId] };
		//Reset fieldtype and buffer if the type was different
		if (cacheFields.find(script) == cacheFields.end())
			continue;
		FieldMap& fMap = cacheFields[script];
		for (auto& pair : scriptClass.mFields)
		{
			//Cache fields did not store this field requested by the script (Aka newly added field)
			if (fMap.find(pair.first) == fMap.end())
				continue;
			//POINTER
			SetFieldValue(mS, pair.second, fMap[pair.first]);
		}
	}
	cacheFields.clear();
	PRINT("LOADED CACHE\n");
}

void ScriptingSystem::SwapDll()
{
	//Load Mono
	CacheScripts();
	PRINT("SWAPPING DLL\n");
	UnloadAppDomain();
	mAppDomain = CreateAppDomain();
	mono_domain_set(mAppDomain, false);
	mCoreAssembly = Utils::loadAssembly("scripts.dll");
	if (mCoreAssembly == nullptr)
	{
		PRINT("Failed to load scripts, fix all errors\n");
		return;
	}
	mAssemblyImage = mono_assembly_get_image(mCoreAssembly);
	RegisterScriptWrappers();
	RegisterComponents();
	mScript = mono_class_from_name(mAssemblyImage, "BeanFactory", "Script");
	MonoClass* mTime = mono_class_from_name(mAssemblyImage, "BeanFactory", "Time");
	mTimeVtable = mono_class_vtable(mAppDomain,mTime);
	mTimeDtField = mono_class_get_field_from_name(mTime,"deltaTime_");
	mStringBuffer = CreateMonoString("");
	UpdateScriptClasses();
	LoadCacheScripts();
	compilingState = CompilingState::Wait;
}

ScriptClass& ScriptingSystem::GetScriptClass(Engine::GUID<ScriptAsset> scriptID)
{
	return scriptClassMap[scriptID];
}

MonoObject* ScriptingSystem::Invoke(MonoObject* mObj, MonoMethod* mMethod, void** params)
{
	E_ASSERT(mMethod,"MONO METHOD WAS NULLPTR");
	if (mObj && mMethod && mAppDomain)
	{
		MonoObject* exception = NULL;
		E_ASSERT(mono_object_get_domain(mObj) == mAppDomain,"Mono object doesn't belong to this domain");
		MonoObject* obj = mono_runtime_invoke(mMethod, mObj, params, &exception);
		if (exception)
		{
			const char* message = mono_string_to_utf8(mono_object_to_string(exception, NULL));
			PRINT(message, '\n');
			//MyEventSystem->publish(new EditorConsoleLogEvent(message));
		}
		return obj;
	}
	return nullptr;
}

void ScriptingSystem::GetFieldValue(MonoObject* instance, MonoClassField* mClassField ,Field& field)
{
	field.fType = Utils::monoTypeToFieldType(mono_field_get_type(mClassField));

	if (field.fType == GetFieldType::E<char*>())
	{
		mono_field_get_value(instance, mClassField, &mStringBuffer);
		strcpy((char*)field.data, mono_string_to_utf8(mStringBuffer));
		return;
	}
	else if (field.fType == GetFieldType::E<Script>())
	{
		field.typeName = mono_type_get_name(mono_field_get_type(mClassField));
		size_t offset = field.typeName.find_last_of(".");
		if (offset != std::string::npos)
			field.typeName = field.typeName.substr(offset + 1);
		Scene& scene = MySceneManager.GetCurrentScene();
		mono_field_get_value(instance, mClassField, field.data);
		MonoObject* scriptRef = field.Get<MonoObject*>();
		if (!scriptRef)
		{
			field.Get<Script*>() = nullptr;
			return;
		}
		Handle handle = GetScriptHandle(scriptRef);
		field.Get<Script*>() = &scene.Get<Script>(handle);
		return;
	}
	else if (field.fType < AllObjectTypes::Size())
	{
		mono_field_get_value(instance, mClassField, field.data);
		ScriptObject<Object>& pObject = field.Get<ScriptObject<Object>>();
		field.Get<Object*>() = pObject;
		return;
	}
	//If mono object, it contains reference to type
	mono_field_get_value(instance, mClassField, field.data);
}


void ScriptingSystem::GetFieldValue(Script& script, const std::string& fieldName, Field& field)
{
	if (mAppDomain)
	{
		MonoObject* monoScript = ReflectScript(script);
		E_ASSERT(monoScript, "MONO OBJECT OF NOT LOADED");
		ScriptClass& scriptClass{ scriptClassMap[script.scriptId] };
		//Reset fieldtype and buffer if the type was different
		MonoClassField* mClassField{ scriptClass.mFields[fieldName] };
		E_ASSERT(mClassField, "FIELD ", fieldName, "COULD NOT BE FOUND IN SCRIPT");
		GetFieldValue(monoScript, mClassField, field);
	}
	else
	{
		field = cacheFields[script][fieldName];
	}
}

void ScriptingSystem::SetFieldValue(Script& script,const std::string& fieldName,Field& field)
{
	if (mAppDomain)
	{
		MonoObject* monoScript = ReflectScript(script);
		E_ASSERT(monoScript, "MONO OBJECT OF NOT LOADED");
		ScriptClass& scriptClass = scriptClassMap[script.scriptId];
		MonoClassField* mClassField{ scriptClass.mFields[fieldName] };
		E_ASSERT(mClassField, "FIELD ", fieldName, "COULD NOT BE FOUND IN SCRIPT ");
		SetFieldValue(monoScript, mClassField, field);
	}
	else
	{
		Scene & currScene{ MySceneManager.GetCurrentScene() };
		MonoScripts& mScripts = mSceneScripts[currScene.uuid];
		cacheFields[script].emplace(fieldName, field);
	}
}

void ScriptingSystem::SetFieldValue(MonoObject* instance, MonoClassField* mClassField, Field& field)
{
	//If its a string, its a C# string so create one
	//PRINT("Set field value: " << mono_field_get_name(mClassFiend));
	if (field.fType == GetFieldType::E<char*>())
	{
		char* data = (char*)field.data;
		MonoString* mString = mono_string_new(mAppDomain,data);
		mono_field_set_value(instance, mClassField, mString);
		//mono_free(mString);
		return;
	}
	if (field.fType < AllObjectTypes::Size())
	{
		Object*& pObject = field.Get<Object*>();
		//Scene& scene = MySceneManager.GetCurrentScene();
		if (pObject == nullptr)
			mono_field_set_value(instance, mClassField, nullptr);
		else if (field.fType == GetType::E<Script>())
			mono_field_set_value(instance, mClassField, ReflectScript(*(Script*)pObject));
		else
		{
			ScriptObject<Object> sObject(pObject);
			mono_field_set_value(instance, mClassField, &sObject);
		}
		return;
	}
	mono_field_set_value(instance, mClassField, field.data);
}

void ScriptingSystem::InvokePhysicsEvent(size_t methodType, PhysicsComponent& rb1, PhysicsComponent& rb2)
{
	Scene& scene = MySceneManager.GetCurrentScene();

	Entity& e1 = scene.Get<Entity>(rb1.EUID());
	Entity& e2 = scene.Get<Entity>(rb2.EUID());

	if (&e1 == nullptr || &e2 == nullptr)
	{
		return;
	}

	if (scene.Has<Script>(e1))
	{
		for (Script* script : scene.GetMulti<Script>(e1))
		{
			if (script->state == DELETED) continue;
			//Incase collision disables gameobject
			if (!&e1 || !&e2)
				break;
			if (!scene.IsActive(e1))
				break;
			if (!scene.IsActive(e2))
				break;
			if (!scene.IsActive(*script))
				continue;
			ScriptClass& scriptClass = scriptClassMap[script->scriptId];
			MonoMethod* mMethod = scriptClass.DefaultMethods[methodType];
			if (!mMethod)
				continue;
			ScriptObject<Object> object(&rb2);
			void* param{ reinterpret_cast<void*>(&object) };
			Invoke(mSceneScripts[scene.uuid][*script], mMethod, &param);
		}
	}



	if (&e2 && scene.Has<Script>(e2) && scene.IsActive(e2))
	{
		for (Script* script : scene.GetMulti<Script>(e2))
		{
			if (!&e1 || !&e2)
				break;
			//Incase collision disables gameobject
			if (!scene.IsActive(e2))
				break;
			if (!scene.IsActive(e1))
				break;
			if (!scene.IsActive(*script))
				continue;
			ScriptClass& scriptClass = scriptClassMap[script->scriptId];
			MonoMethod* mMethod = scriptClass.DefaultMethods[methodType];
			if (!mMethod)
				continue;
			ScriptObject<Object> object(&rb1);
			void* param{ reinterpret_cast<void*>(&object) };
			Invoke(mSceneScripts[scene.uuid][*script], mMethod, &param);
		}
	}
}


void ScriptingSystem::CallbackCollisionEnter(ContactAddedEvent* pEvent)
{
	InvokePhysicsEvent(DefaultMethodTypes::OnCollisionEnter,*pEvent->pc1,*pEvent->pc2);
}

void ScriptingSystem::CallbackCollisionExit(ContactRemovedEvent* pEvent)
{
	InvokePhysicsEvent(DefaultMethodTypes::OnCollisionExit, *pEvent->pc1, *pEvent->pc2);
}

void ScriptingSystem::InvokeMethod(Script& script, size_t methodType)
{
	MonoObject* mNewScript = ReflectScript(script);
	//PRINT("Script Invoking " << pEvent->script.Name() << " " << pEvent->methodName << " ,ID: " << pEvent->script.uuid);
	E_ASSERT(mNewScript, std::string("MONO OBJECT NOT LOADED"));
	ScriptClass& scriptClass{ scriptClassMap[script.scriptId] };
	MonoMethod* mMethod{ scriptClass.DefaultMethods[methodType]};
	E_ASSERT(mono_object_isinst(mNewScript, scriptClass.mClass),"Object and class mismatch!");
	if (mMethod)
		Invoke(mNewScript, mMethod, nullptr);
}


void ScriptingSystem::CallbackTriggerEnter(TriggerEnterEvent* pEvent)
{
	InvokePhysicsEvent(DefaultMethodTypes::OnTriggerEnter, *pEvent->pc1, *pEvent->pc2);
}

void ScriptingSystem::CallbackTriggerExit(TriggerRemoveEvent* pEvent)
{
	InvokePhysicsEvent(DefaultMethodTypes::OnTriggerExit, *pEvent->pc1, *pEvent->pc2);
}

void ScriptingSystem::CallbackScriptModified(FileTypeModifiedEvent<FileType::SCRIPT>* pEvent)
{
	(void)pEvent;
	timeUntilRecompile = SECONDS_TO_RECOMPILE;
}

bool ScriptingSystem::IsScript(MonoClass* monoClass)
{
	if (mono_class_is_subclass_of(monoClass, mScript, false))
		return true;
	return false;
}
//

MonoObject* ScriptingSystem::ReflectScript(Script& script, MonoObject* ref)
{
	Scene& scene = MySceneManager.GetCurrentScene();
	MonoScripts& mScripts = mSceneScripts[scene.uuid];
	auto pairIt = mScripts.find(script);
	if (pairIt == mScripts.end())
	{
		if (scriptClassMap.find(script.scriptId) == scriptClassMap.end())
			return nullptr;
		E_ASSERT(scriptClassMap.find(script.scriptId) != scriptClassMap.end(),"Script class does not exist!");
		ScriptClass& scriptClass = scriptClassMap[script.scriptId];
		MonoObject* instance = InstantiateClass(scriptClass.mClass);
		Engine::UUID euid = script.EUID();
		Engine::UUID uuid = script.UUID();
		void* param[] = {&ScriptObject<Entity>(&scene.Get<Entity>(script)),&euid,&uuid};
		MonoMethod* reflectComponent = mono_class_get_method_from_name(mScript, "Initialize", 3);
		Invoke(instance, reflectComponent, param);
		mScripts.emplace(script, instance);
		//Check fields, dont remove fields
		if (ref)
		{
			for (auto& pair : scriptClass.mFields)
			{
				static char buffer[2048]{};
				Field field{ AllFieldTypes::Size(),2048 ,buffer };
				GetFieldValue(ref, pair.second, field);
				if (field.fType < AllObjectTypes::Size())
				{
					uint32_t flags = mono_field_get_flags(pair.second);
					Object*& f = field.Get<Object*>();
					if (f)
					{
						Handle handle = { f->EUID(),f->UUID() };
						Object* pObject = (Object*)scene.GetByHandle(field.fType, &handle);
						field.Get<Object*>() = pObject;
					}
				}
				SetFieldValue(instance, pair.second, field);
			}
		}
		//Set back fields from previous scene
		return instance;
	}
	if (ref)
	{
		ScriptClass& scriptClass = scriptClassMap[script.scriptId];
		for (auto& pair : scriptClass.mFields)
		{
			static char buffer[2048]{};
			Field field{ AllFieldTypes::Size() ,2048,buffer };
			GetFieldValue(ref, pair.second, field);
			if (field.fType < AllObjectTypes::Size())
			{
				Object*& f = field.Get<Object*>();
				if (f)
				{
					Handle handle = { f->EUID(),f->UUID() };
					Object* pObject = (Object*)scene.GetByHandle(field.fType, &handle);
					field.Get<Object*>() = pObject;
				}
			}
			SetFieldValue(pairIt->second, pair.second, field);
		}
	}
	return pairIt->second;
}

void ScriptingSystem::CallbackScriptGetField(ScriptGetFieldEvent* pEvent)
{
	GetFieldValue(pEvent->script,pEvent->fieldName,pEvent->field);
}

void ScriptingSystem::CallbackScriptSetField(ScriptSetFieldEvent* pEvent)
{
	SetFieldValue(pEvent->script, pEvent->fieldName, pEvent->field);
}


void ScriptingSystem::CallbackScriptGetFieldNames(ScriptGetFieldNamesEvent* pEvent)
{
	static std::vector<const char*> fieldNames;
	pEvent->pStart = nullptr;
	pEvent->count = 0;
	fieldNames.clear();
	Scene& scene = MySceneManager.GetCurrentScene();
	if (mSceneScripts.find(scene.uuid) == mSceneScripts.end())
		return;
	MonoScripts& mScripts = mSceneScripts[scene.uuid];
	ScriptClass& scriptClass = scriptClassMap[pEvent->script.scriptId];
	for (auto& pair : scriptClass.mFields)
	{
		fieldNames.emplace_back(pair.first.c_str());
	}
	pEvent->pStart = fieldNames.data();
	pEvent->count = fieldNames.size();
}


std::string ScriptingSystem::GetScriptName(Script& script)
{
	GetFilePathEvent<ScriptAsset> e(script.scriptId);
	EVENTS.Publish(&e);
	return e.filePath.stem().string();
}

void ScriptingSystem::CallbackSceneStart(SceneStartEvent* pEvent)
{
	playMode = true;
	ReflectFromOther(MySceneManager.GetPreviousScene());
	InvokeAllScripts(DefaultMethodTypes::Awake);
	InvokeAllScripts(DefaultMethodTypes::Start);
}

void ScriptingSystem::CallbackSceneChanging(SceneChangingEvent* pEvent)
{
	UNREFERENCED_PARAMETER(pEvent);
	WAIT_FOR_COMPILATION;
}

void ScriptingSystem::CallbackScriptCreated(ObjectCreatedEvent<Script>* pEvent)
{
	if (MySceneManager.HasScene())
	{
		ReflectScript(*pEvent->pObject);
	}
}

void ScriptingSystem::CallbackSceneStop(SceneStopEvent* pEvent)
{
	playMode = false;
	for (auto& pair : scriptClassMap)
	{
		ScriptClass& sClass = pair.second;
		MonoVTable* vTable = mono_class_vtable(mAppDomain, sClass.mClass);

		void* fieldIterator = nullptr;
		while (MonoClassField* field = mono_class_get_fields(sClass.mClass, &fieldIterator))
		{
			uint32_t flags = mono_field_get_flags(field);
			if (flags & MONO_FIELD_ATTR_STATIC)
			{
				mono_field_static_set_value(vTable, field, nullptr);
			}
		}
	}
	mSceneScripts.erase(pEvent->sceneID);
	for (auto handle : mSceneHandles[pEvent->sceneID])
	{
		mono_gchandle_free(handle);
	}
	mSceneHandles.erase(pEvent->sceneID);
}
