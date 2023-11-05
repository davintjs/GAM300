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
#include "mono/metadata/tabledefs.h"
#include <mono/jit/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/exception.h>
#include <mono/metadata/environment.h>

#include <string.h>

#define SECONDS_TO_RECOMPILE 1.f

#define TEXT_BUFFER_SIZE 2048

#define SCRIPT_THREAD_EVENT(Event) { if (SCRIPTING_THREAD_ID != std::this_thread::get_id()) { scriptingEvent = Event; while(scriptingEvent){ACQUIRE_SCOPED_LOCK(Mono); }; return;}};

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
	//CollisionPRINT("LOOPIN\n");
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
			if (flags & FIELD_ATTRIBUTE_STATIC || flags & FIELD_ATTRIBUTE_NOT_SERIALIZED)
				continue;
			if (flags & FIELD_ATTRIBUTE_PUBLIC)
			{
				MonoType* type = mono_field_get_type(field);
				size_t fieldType = Utils::monoTypeToFieldType(type);
				//PRINT(mono_type_get_name(type) << (int)fieldType);
				if (fieldType < AllFieldTypes::Size())
				{
					mFields[fieldName] = field;
				}
			}
		}
	}

#pragma endregion

MonoImage* ScriptingSystem::GetAssemblyImage()
	{
		return mAssemblyImage;
	}

void ScriptingSystem::RecompileThreadWork()
{
	compilingState = CompilingState::Compiling;
	Utils::CompileDll();
	compilingState = CompilingState::SwapAssembly;
	PRINT("RECOMPLING END\n");
}

void ScriptingSystem::Init()
{
	logicState = LogicState::NONE;
	THREADS.EnqueueTask([this] {ThreadWork(); });
	#ifndef _BUILD
		EVENTS.Subscribe(this, &ScriptingSystem::CallbackScriptModified);
	#endif
	Subscribe(&ScriptingSystem::CallbackScriptSetField);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackScriptGetField);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackScriptGetFieldNames);

	EVENTS.Subscribe(this, &ScriptingSystem::CallbackSceneStart);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackSceneCleanup);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackSceneChanging);
	Subscribe(&ScriptingSystem::CallbackScriptCreated);
	SubscribeObjectDestroyed(AllObjectTypes());


	EVENTS.Subscribe(this, &ScriptingSystem::CallbackSceneChanging);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackApplicationExit);
	Subscribe(&ScriptingSystem::CallbackCollisionEnter);
	Subscribe(&ScriptingSystem::CallbackCollisionExit);

}

template<class EventType>
void ScriptingSystem::Subscribe(void (ScriptingSystem::* memberFunction)(EventType*))
{
	events[typeid(EventType)] = new MemberFunctionHandler<ScriptingSystem, EventType>(this, memberFunction);
	EVENTS.Subscribe(this, memberFunction);
}

void ScriptingSystem::Update(float dt)
{
	if (logicState != LogicState::NONE)
	{
		mono_field_static_set_value(mTimeVtable,mTimeDtField,&dt);

		//Sync logic thread with main thread
		ran = false;
		//Logic thread running because ran is set to false
		while (ran == false) { ACQUIRE_SCOPED_LOCK(Mono); };
	}
}

void ScriptingSystem::Exit() 
{
	for (auto& keyPair : events)
	{
		delete keyPair.second;
	}
}

template <typename... Args>
MonoObject* ScriptingSystem::InstantiateClass(MonoClass* mClass, Args&&... args)
{
	E_ASSERT(mAppDomain, "MONO APP DOMAIN NOT LOADED");
	E_ASSERT(mClass, "MONO CLASS NOT LOADED");
	MonoObject* tmp = mono_object_new(mAppDomain, mClass);
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
		//MonoVTable* vTable = nullptr;
		if (mono_class_get_parent(_class) == mScript)
		{
			//mono_class_v
			//vTable = mono_class_vtable(mAppDomain, _class);
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
		//else if (mono_class_get_parent(_class) == mono_class_from_name(mAssemblyImage, name_space, "Component"))
		//{
		//	if (_class == mScript)
		//		continue;
		//	vTable = mono_class_vtable(mAppDomain, _class);
		//	scriptClassMap[name] = ScriptClass{ name,_class };
		//}

		//if (!vTable)
		//	continue;

		//void* fieldIterator = nullptr;
		//while (MonoClassField* field = mono_class_get_fields(_class, &fieldIterator))
		//{
		//	uint32_t flags = mono_field_get_flags(field);
		//	if (flags & FIELD_ATTRIBUTE_STATIC)
		//	{
		//		mono_field_static_set_value(vTable, field, nullptr);
		//	}
		//}
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
	Scene& scene = MySceneManager.GetCurrentScene();
	for (auto& script : scene.GetArray<Script>())
	{
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
	SCRIPT_THREAD_EVENT(pEvent);
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
	const MonoTableInfo* table_info = mono_image_get_table_info(mAssemblyImage, MONO_TABLE_TYPEDEF);
	int rows = mono_table_info_get_rows(table_info);
	for (auto& sceneScripts : mSceneScripts)
	{
		for (auto& scriptPair : sceneScripts.second)
		{
			MonoClass* _class = mono_object_get_class(scriptPair.second);
			if (!_class || mono_class_get_parent(_class) != mScript)
				continue;
			MonoVTable* vTable = mono_class_vtable(mAppDomain, _class);
			if (!vTable)
				continue;
			void* fieldIterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(_class, &fieldIterator))
			{
				uint32_t flags = mono_field_get_flags(field);
				if (flags & FIELD_ATTRIBUTE_STATIC)
				{
					mono_field_static_set_value(vTable, field, nullptr);
				}
				else
				{
					mono_field_set_value(scriptPair.second, field, nullptr);
				}
			}
		}
	}
	mono_domain_set(mRootDomain, false);
	#ifdef _DEBUG
		mono_domain_unload(mAppDomain);
	#endif
	mAppDomain = nullptr;
}

void ScriptingSystem::ThreadWork()
{
	SCRIPTING_THREAD_ID = std::this_thread::get_id();
	InitMono();
	SwapDll();
	if (mCoreAssembly == nullptr)
	{
		RecompileThreadWork();
	}

	while (!THREADS.HasStopped())
	{
		ACQUIRE_SCOPED_LOCK(Mono);
		if (scriptingEvent)
		{
			events[typeid(*scriptingEvent)]->exec(scriptingEvent);
			scriptingEvent = nullptr;
		}

		if (logicState != LogicState::NONE)
		{
			if (ran)
				continue;
			if (logicState == LogicState::UPDATE)
			{
				InvokeAllScripts(DefaultMethodTypes::Update);
				InvokeAllScripts(DefaultMethodTypes::LateUpdate);
				InvokeAllScripts(DefaultMethodTypes::ExecuteCoroutines);
			}
			else if (logicState == LogicState::START)
			{
				#ifndef _BUILD
					ReflectFromOther(MySceneManager.GetPreviousScene());
				#endif
				InvokeAllScripts(DefaultMethodTypes::Awake);
				InvokeAllScripts(DefaultMethodTypes::Start);
				logicState = LogicState::UPDATE;
			}
			else if (logicState == LogicState::EXIT)
			{
				mSceneScripts.erase(MySceneManager.GetCurrentScene().uuid);
				logicState = LogicState::NONE;
			}
			//FINISHED RUNNING
			ran = true;
			continue;
		}

		#ifndef _BUILD
		//Pause timer when recompiling
		if (timeUntilRecompile > 0)
		{
			Sleep(1000);
			timeUntilRecompile -= 1;
			if (timeUntilRecompile <= 0)
			{
				THREADS.EnqueueTask([this] {RecompileThreadWork(); });
			}
		}
		else if (compilingState == CompilingState::SwapAssembly)
		{
			CacheScripts();
			if (MySceneManager.HasScene())
			{
				Scene& currScene{ MySceneManager.GetCurrentScene() };
				mSceneScripts[currScene.uuid].clear();
			}
			SwapDll();
			LoadCacheScripts();
			compilingState = CompilingState::Wait;
		}
		#endif
	}
	UnloadAppDomain();
	ShutdownMono();
	PRINT("MONO THREAD ENDED!\n");
}

void ScriptingSystem::CacheScripts()
{
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
				fieldSize = sizeof(uint64_t);
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
			//MonoType* mType = mono_field_get_type(pair.second);
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
}

MonoObject* ScriptingSystem::Invoke(MonoObject* mObj, MonoMethod* mMethod, void** params)
{
	E_ASSERT(mMethod,"MONO METHOD WAS NULLPTR");
	if (mObj && mMethod && mAppDomain)
	{
		try
		{
			MonoObject* exception = NULL;
			MonoObject* obj = mono_runtime_invoke(mMethod, mObj, params, &exception);
			if (exception)
			{
				const char* message = mono_string_to_utf8(mono_object_to_string(exception, NULL));
				PRINT(message, '\n');
				//MyEventSystem->publish(new EditorConsoleLogEvent(message));
			}
			return obj;
		}
		catch (...)
		{


		}
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
		Object*& pObject = *(Object**)field.data;
		if (!pObject)
			return;
		pObject = (Object*)((size_t)pObject - 8);
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
		Object*& pObject = *(Object**)field.data;
		//Scene& scene = MySceneManager.GetCurrentScene();
		if (pObject == nullptr)
			mono_field_set_value(instance, mClassField, nullptr);
		else if (field.fType == GetType::E<Script>())
			mono_field_set_value(instance, mClassField, ReflectScript(*(Script*)pObject));
		else
		{
			pObject = (Object*)(size_t(pObject) + 8);
			mono_field_set_value(instance, mClassField, pObject);
		}
		return;
	}
	mono_field_set_value(instance, mClassField, field.data);
}

void ScriptingSystem::InvokePhysicsEvent(size_t methodType, Rigidbody* rb1, Rigidbody* rb2)
{
	Scene& scene = MySceneManager.GetCurrentScene();

	Entity& e1 = scene.Get<Entity>(rb1->EUID());
	Entity& e2 = scene.Get<Entity>(rb2->EUID());

	//if (scene.Has<Script>(e1))
	//{
	//	for (Script* script : scene.GetMulti<Script>(e1))
	//	{
	//		//Incase collision disables gameobject
	//		if (!scene.IsActive(e1))
	//			break;
	//		if (!scene.IsActive(*script))
	//			continue;
	//		ScriptClass& scriptClass = scriptClassMap[script->scriptId];
	//		MonoMethod* mMethod = scriptClass.DefaultMethods[methodType];
	//		if (!mMethod)
	//			continue;
	//		voi param = reinterpret_cast<void*>(reinterpret_cast<size_t>(rb2) - 8);
	//		Invoke(mSceneScripts[scene.uuid][*script], mMethod, &param);
	//	}
	//}

	//if (scene.Has<Script>(e2) && scene.IsActive(e2))
	//{
	//	for (Script* script : scene.GetMulti<Script>(e2))
	//	{
	//		//Incase collision disables gameobject
	//		if (!scene.IsActive(e2))
	//			break;
	//		if (!scene.IsActive(*script))
	//			continue;
	//		ScriptClass& scriptClass = scriptClassMap[script->scriptId];
	//		MonoMethod* mMethod = scriptClass.DefaultMethods[methodType];
	//		if (!mMethod)
	//			continue;
	//		void* param = reinterpret_cast<void*>(reinterpret_cast<size_t>(rb1) - 8);
	//		Invoke(mSceneScripts[scene.uuid][*script], mMethod, &param);
	//	}
	//}
}


void ScriptingSystem::CallbackCollisionEnter(ContactAddedEvent* pEvent)
{
	SCRIPT_THREAD_EVENT(pEvent);
	InvokePhysicsEvent(DefaultMethodTypes::OnCollisionEnter,pEvent->rb1,pEvent->rb2);
}

void ScriptingSystem::CallbackCollisionExit(ContactRemovedEvent* pEvent)
{
	SCRIPT_THREAD_EVENT(pEvent);
	InvokePhysicsEvent(DefaultMethodTypes::OnCollisionExit, pEvent->rb1, pEvent->rb2);
}

void ScriptingSystem::InvokeMethod(Script& script, size_t methodType)
{
	MonoObject* mNewScript = ReflectScript(script);
	//PRINT("Script Invoking " << pEvent->script.Name() << " " << pEvent->methodName << " ,ID: " << pEvent->script.uuid);
	E_ASSERT(mNewScript, std::string("MONO OBJECT NOT LOADED"));
	ScriptClass& scriptClass{ scriptClassMap[script.scriptId] };
	MonoMethod* mMethod{ scriptClass.DefaultMethods[methodType]};
	if (mMethod)
		Invoke(mNewScript, mMethod, nullptr);
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
		ScriptClass& scriptClass = scriptClassMap[script.scriptId];
		MonoObject* instance = InstantiateClass(scriptClass.mClass);
		Engine::UUID euid = script.EUID();
		Engine::UUID uuid = script.UUID();
		void* param[] = {&scene.Get<Entity>(script),&euid,&uuid };
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
					Object*& f = field.Get<Object*>();
					if (f)
					{
						Handle handle = { f->EUID(),f->UUID() };
						void* pObject = scene.GetByHandle(field.fType, &handle);
						field.data = &pObject;
					}
				}
				else if (field.fType == AllFieldTypes::Size())
				{
					continue;
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
					void* pObject = scene.GetByHandle(field.fType, &handle);
					field.data = &pObject;
				}
			}
			else if (field.fType == AllFieldTypes::Size())
			{
				continue;
			}
			SetFieldValue(pairIt->second, pair.second, field);
		}
	}
	/*return (*pairIt).second;*/
	return pairIt->second;
}

void ScriptingSystem::CallbackScriptGetField(ScriptGetFieldEvent* pEvent)
{
	ACQUIRE_SCOPED_LOCK(Mono);
	GetFieldValue(pEvent->script,pEvent->fieldName,pEvent->field);
}

void ScriptingSystem::CallbackScriptSetField(ScriptSetFieldEvent* pEvent)
{
	SCRIPT_THREAD_EVENT(pEvent);
	SetFieldValue(pEvent->script, pEvent->fieldName, pEvent->field);
}


void ScriptingSystem::CallbackScriptGetFieldNames(ScriptGetFieldNamesEvent* pEvent)
{
	ACQUIRE_SCOPED_LOCK(Mono);
	static std::vector<const char*> fieldNames;
	pEvent->pStart = nullptr;
	pEvent->count = 0;
	fieldNames.clear();
	if (mAppDomain == nullptr || mCoreAssembly == nullptr || mAssemblyImage == nullptr)
	{
		return;
	}
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
	UNREFERENCED_PARAMETER(pEvent);
	while (mAppDomain == nullptr || compilingState != CompilingState::Wait){ ACQUIRE_SCOPED_LOCK(Mono); };
	{
		ACQUIRE_SCOPED_LOCK(Mono);
		logicState = LogicState::START;
		ran = true;
	}
}

void ScriptingSystem::CallbackSceneChanging(SceneChangingEvent* pEvent)
{
	UNREFERENCED_PARAMETER(pEvent);
	while (compilingState != CompilingState::Wait) { ACQUIRE_SCOPED_LOCK(Mono); };
}

void ScriptingSystem::CallbackScriptCreated(ObjectCreatedEvent<Script>* pEvent)
{
	SCRIPT_THREAD_EVENT(pEvent);
	if (MySceneManager.HasScene())
	{
		ReflectScript(*pEvent->pObject);
	}
}

void ScriptingSystem::CallbackSceneCleanup(SceneCleanupEvent* pEvent)
{
	(void)pEvent;
	logicState = LogicState::EXIT;
	ran = false;
	while (ran == false) { ACQUIRE_SCOPED_LOCK(Mono); };
}

void ScriptingSystem::CallbackApplicationExit(ApplicationExitEvent* pEvent)
{
	(void)pEvent;
	logicState = LogicState::EXIT;
	ran = false;
	while (ran == false) { ACQUIRE_SCOPED_LOCK(Mono); };
}