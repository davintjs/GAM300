/*!***************************************************************************************
****
\file			scripting-system.cpp
\project
\author			Zacharie Hong
\co-authors

\par			Course: GAM250
\par			Section:
\date			10/03/2022

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

namespace
{
	MonoDomain* mRootDomain{ nullptr };		//JIT RUNTIME DOMAIN
	MonoDomain* mAppDomain{ nullptr };		//APP DOMAIN
	MonoAssembly* mCoreAssembly{ nullptr };	//ASSEMBLY OF SCRIPTS.DLL
	MonoImage* mAssemblyImage{ nullptr };	//LOADED IMAGE OF SCRIPTS.DLL
	MonoClass* mScript{ nullptr };
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
		void* methodIterator = nullptr;
		while (MonoMethod* method = mono_class_get_methods(_mClass, &methodIterator))
		{
			mMethods[mono_method_get_name(method)] = method;
		}

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
std::string ScriptingSystem::AddEmptyScript(const std::string& _name)
{
	std::string filePath{ "Assets" + _name + ".cs"};
	std::ofstream file(filePath);
	file << "using BeanFactory;\n";
	file << "using System;\n\n";
	file << "public class " << _name << ": BeanScript\n{\n";
	file << "\tvoid Start()\n\t{\n\n\t}\n";
	file << "\tvoid Update()\n\t{\n\n\t}\n";
	file << "}\n";
	file.close();
	return filePath;
}

MonoType* ScriptingSystem::GetMonoTypeFromName(std::string& name)
{
	return mono_reflection_type_from_name(name.data(), mAssemblyImage);
}

void ScriptingSystem::RecompileThreadWork()
{
	//ACQUIRE_SCOPED_LOCK(Assets);
	compilingState = CompilingState::Compiling;
	Utils::CompileDll();
	compilingState = CompilingState::SwapAssembly;
	PRINT("RECOMPLING END\n");
}

void ScriptingSystem::Init()
{
	logicState = LogicState::NONE;
	#ifdef _BUILD
		SwapDll();
	#else
		THREADS.EnqueueTask([this] {ThreadWork(); });
		EVENTS.Subscribe(this, &ScriptingSystem::CallbackScriptModified);
		THREADS.EnqueueTask([this] {RecompileThreadWork(); });
	#endif
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackScriptSetField);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackScriptGetField);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackScriptGetFieldNames);


	EVENTS.Subscribe(this, &ScriptingSystem::CallbackGetScriptNames);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackSceneStart);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackSceneCleanup);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackSceneChanging);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackScriptCreated);
	SubscribeObjectDestroyed(AllObjectTypes());
}

void ScriptingSystem::Update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (logicState != LogicState::NONE)
	{
		//Sync logic thread with main thread
		ran = false;
		//Logic thread running because ran is set to false
		while (ran == false);
	}
}

void ScriptingSystem::Exit() {}

template <typename... Args>
MonoObject* ScriptingSystem::InstantiateClass(MonoClass* mClass, Args&&... args)
{
	E_ASSERT(mAppDomain, "MONO APP DOMAIN NOT LOADED");
	E_ASSERT(mClass, "MONO CLASS NOT LOADED");
	MonoObject* tmp = mono_object_new(mAppDomain, mClass);
	mono_runtime_object_init(tmp);
	return tmp;
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
			scriptClassMap[name] = ScriptClass{_class};
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

void ScriptingSystem::InvokeAllScripts(const std::string& funcName)
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
		InvokeMethod(script, funcName);
	}
}

void ScriptingSystem::UpdateReferences()
{
	Scene& scene = MySceneManager.GetCurrentScene();
	for (auto& script : scene.GetArray<Script>())
	{
		MonoObject* mS = ReflectScript(script);
		ScriptClass& scriptClass = scriptClassMap[script.name];
		for (auto& pair : scriptClass.mFields)
		{
			size_t fType = Utils::monoTypeToFieldType(mono_field_get_type(pair.second));
			if (fType < AllObjectTypes::Size())
			{
				Object* pObject;
				Field field{ fType,&pObject };
				GetFieldValue(mS, pair.second, field);
				if (!pObject || scene.HasHandle(fType, pObject))
					continue;
				mono_field_set_value(mS, pair.second, nullptr);

			}
		}
	}
}


template <typename... Ts>
void ScriptingSystem::SubscribeObjectDestroyed(TemplatePack<Ts...>)
{
	(EVENTS.Subscribe(this, &ScriptingSystem::CallbackObjectDestroyed<Ts>), ...);
}

template<typename T>
void ScriptingSystem::CallbackObjectDestroyed(ObjectDestroyedEvent<T>* pEvent)
{
	UNREFERENCED_PARAMETER(pEvent);
	ACQUIRE_SCOPED_LOCK(UpdateRef);
	objectDestroyed = true;
}

void ScriptingSystem::ThreadWork()
{
	InitMono();
	while (!THREADS.HasStopped())
	{
		if (mAppDomain && MySceneManager.HasScene())
		{
			Scene& scene = MySceneManager.GetCurrentScene();
			{
				ACQUIRE_SCOPED_LOCK(ScriptQueue);
				for (Handle& handle : reflectionQueue)
				{
					ReflectScript(scene.Get<Script>(handle));
				}
				reflectionQueue.clear();
			}
			{
				ACQUIRE_SCOPED_LOCK(UpdateRef);
				if (objectDestroyed)
				{
					UpdateReferences();
					objectDestroyed = false;
				}
			}
		}

		if (logicState != LogicState::NONE)
		{
			if (ran)
				continue;
			ACQUIRE_SCOPED_LOCK(Mono);
			if (logicState == LogicState::UPDATE)
			{
				InvokeAllScripts("Update");
				InvokeAllScripts("LateUpdate");
			}
			else if (logicState == LogicState::START)
			{
				PRINT("STARTED\n");
				ReflectFromOther(MySceneManager.GetPreviousScene());
				InvokeAllScripts("Awake");
				InvokeAllScripts("Start");
				logicState = LogicState::UPDATE;
			}
			else if (logicState == LogicState::EXIT)
			{
				InvokeAllScripts("Exit");
				mSceneScripts.erase(MySceneManager.GetCurrentScene().uuid);
				logicState = LogicState::NONE;
				reflectionQueue.clear();
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
			ACQUIRE_SCOPED_LOCK(Mono);
			timeUntilRecompile -= 1;
			if (timeUntilRecompile <= 0)
			{
				THREADS.EnqueueTask([this] {RecompileThreadWork(); });
			}
		}
		else if (compilingState == CompilingState::SwapAssembly)
		{
			ACQUIRE_SCOPED_LOCK(Mono);
			CacheScripts();
			SwapDll();
			LoadCacheScripts();
			compilingState = CompilingState::Wait;
		}
		#endif
	}
	mono_domain_set(mRootDomain,false);
	if (mAppDomain)
		mono_domain_unload(mAppDomain);
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
		ScriptClass& scriptClass{ scriptClassMap[script.name] };
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
			else if (fType == GetFieldType::E<std::string>())
			{
				fieldSize = TEXT_BUFFER_SIZE;
			}
			FieldMap& fMap = cacheFields[script];
			fMap.emplace(pair.first, Field{ AllFieldTypes::Size(), new char[fieldSize] });
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
		ScriptClass& scriptClass{ scriptClassMap[script.name] };
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
		for (auto& pair : fMap)
		{
			delete pair.second.data;
		}
	}
	cacheFields.clear();
	PRINT("LOADED CACHE\n");
}

void ScriptingSystem::SwapDll()
{
	//Load Mono
	PRINT("SWAPPING DLL\n");
	Scene& currScene{ MySceneManager.GetCurrentScene() };
	mSceneScripts[currScene.uuid].clear();
	if (mAppDomain)
	{
		mono_domain_set(mRootDomain, false);
		mono_domain_unload(mAppDomain);
	}
	mAppDomain = CreateAppDomain();
	mono_domain_set(mAppDomain, false);
	mCoreAssembly = Utils::loadAssembly("scripts.dll");
	if (mCoreAssembly == nullptr)
	{
		PRINT("Failed to load scripts, fix all errors\n");
		compilingState = CompilingState::Wait;
		return;
	}
	mAssemblyImage = mono_assembly_get_image(mCoreAssembly);
	RegisterScriptWrappers();
	RegisterComponents();
	mScript = mono_class_from_name(mAssemblyImage, "BeanFactory", "Script");
	UpdateScriptClasses();
}

MonoObject* ScriptingSystem::invoke(MonoObject* mObj, MonoMethod* mMethod, void** params)
{
	E_ASSERT(mMethod,"MONO METHOD WAS NULLPTR");
	if (mObj && mMethod && mAppDomain)
	{
		try
		{
			MonoObject* exception = NULL;
			MonoObject* obj = mono_runtime_invoke(mMethod, mObj, params, &exception);
			//if (exception)
			//{
			//	const char* message = mono_string_to_utf8(mono_object_to_string(exception, NULL));
			//	PRINT(message, '\n');
			//	//MyEventSystem->publish(new EditorConsoleLogEvent(message));
			//}
			return obj;
		}
		catch (...)
		{


		}
	}
	return nullptr;
}

MonoObject* ScriptingSystem::CloneInstance(MonoObject* _instance)
{
	if (!_instance)
		return nullptr;
	return mono_object_clone(_instance);
}

MonoObject* ScriptingSystem::CreateInstance(MonoClass* _mClass)
{
	return mono_object_new(mAppDomain,_mClass);
}

MonoString* ScriptingSystem::CreateMonoString(const char* str)
{
	if (!mAppDomain)
	{
		PRINT("APP DOMAIN NOT LOADED");
	}
	return mono_string_new(mAppDomain, str);
}

void ScriptingSystem::GetFieldValue(MonoObject* instance, MonoClassField* mClassField ,Field& field)
{
	field.fType = Utils::monoTypeToFieldType(mono_field_get_type(mClassField));

	if (field.fType == GetFieldType::E<std::string>())
	{
		MonoString* mono_string = CreateMonoString("");
		mono_field_get_value(instance, mClassField, &mono_string);
		char* str = mono_string_to_utf8(mono_string);
		strcpy_s((char*)field.data, strlen(str)+1, str);
		mono_free(mono_string);
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
		Engine::UUID euid;
		Engine::UUID uuid;
		mono_field_get_value
		(
			scriptRef,
			mono_class_get_field_from_name(mScript, "euid"),
			&euid
		);
		mono_field_get_value
		(
			scriptRef,
			mono_class_get_field_from_name(mScript, "uuid"),
			&uuid
		);
		field.Get<Script*>() = &scene.Get<Script>(euid,uuid);
		return;
	}
	//If mono object, it contains reference to type
	mono_field_get_value(instance, mClassField, field.data);
}

void ScriptingSystem::SetFieldValue(MonoObject* instance, MonoClassField* mClassField, Field& field)
{
	//If its a string, its a C# string so create one
	//PRINT("Set field value: " << mono_field_get_name(mClassFiend));
	//if (field.fType == GetFieldType::E<std::string>())
	//{
	//	MonoString* mono_string = CreateMonoString(reinterpret_cast<const char*>(value));
	//	mono_field_set_value(instance, mClassField, mono_string);
	//	return;
	//}
	if (field.fType < AllObjectTypes::Size())
	{
		Object* pObject = *(Object**)field.data;
		//Scene& scene = MySceneManager.GetCurrentScene();
		if (pObject == nullptr)
			mono_field_set_value(instance, mClassField, nullptr);
		else if (field.fType == GetType::E<Script>())
			mono_field_set_value(instance, mClassField, ReflectScript(*(Script*)pObject));
		else
			mono_field_set_value(instance, mClassField, pObject);
		return;
	}
	mono_field_set_value(instance, mClassField, field.data);
}


void ScriptingSystem::InvokeMethod(Script& script, const std::string& method)
{
	MonoObject* mNewScript = ReflectScript(script);
	//PRINT("Script Invoking " << pEvent->script.Name() << " " << pEvent->methodName << " ,ID: " << pEvent->script.uuid);
	E_ASSERT(mNewScript, std::string("MONO OBJECT OF ") + script.name + std::string(" NOT LOADED"));
	ScriptClass& scriptClass{ scriptClassMap[script.name] };
	MonoMethod* mMethod{ mono_class_get_method_from_name (scriptClass.mClass,method.c_str(),0)};
	if (!mMethod && mono_class_get_parent(scriptClass.mClass) == mScript)
	{
		mMethod = mono_class_get_method_from_name(mScript, method.c_str(), 0);
		if (!mMethod)
			return;
	}
	E_ASSERT(mMethod, std::string("MONO METHOD ") + method + std::string(" IN SCRIPT ") + script.name + std::string(" NOT FOUND"));
	invoke(mNewScript, mMethod, nullptr);
}

void ScriptingSystem::CallbackScriptModified(FileTypeModifiedEvent<FileType::SCRIPT>* pEvent)
{
	(void)pEvent;
	ACQUIRE_SCOPED_LOCK(Mono);
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
		if (scriptClassMap.find(script.name) == scriptClassMap.end())
			return nullptr;
		ScriptClass& scriptClass = scriptClassMap[script.name];
		MonoObject* instance = InstantiateClass(scriptClass.mClass);
		Engine::UUID euid = script.EUID();
		Engine::UUID uuid = script.UUID();
		void* param[] = {&scene.Get<Entity>(script),&euid,&uuid };
		MonoMethod* reflectComponent = mono_class_get_method_from_name(mScript, "Initialize", 3);
		invoke(instance, reflectComponent, param);
		mScripts.emplace(script, instance);
		//Check fields, dont remove fields
		if (ref)
		{
			for (auto& pair : scriptClass.mFields)
			{
				static char buffer[2048];
				Field field{ AllFieldTypes::Size() ,buffer };
				GetFieldValue(ref, pair.second, field);
				if (field.fType == AllFieldTypes::Size())
				{
					continue;
				}
				SetFieldValue(instance, pair.second, field);
			}
		}
		//Set back fields from previous scene
		return instance;
	}
	/*return (*pairIt).second;*/
	return pairIt->second;
}

void ScriptingSystem::CallbackScriptGetField(ScriptGetFieldEvent* pEvent)
{
	MonoObject* script = ReflectScript(pEvent->script);
	E_ASSERT(script, "MONO OBJECT OF ", pEvent->script.name," NOT LOADED");
	ScriptClass& scriptClass{ scriptClassMap[pEvent->script.name] };
	//Reset fieldtype and buffer if the type was different
	MonoClassField* mClassField{scriptClass.mFields[pEvent->fieldName]};
	E_ASSERT(mClassField, "FIELD ", pEvent->fieldName, "COULD NOT BE FOUND IN SCRIPT ", pEvent->script.name);
	GetFieldValue(script,mClassField,pEvent->field);
}

void ScriptingSystem::CallbackScriptSetField(ScriptSetFieldEvent* pEvent)
{
	MonoObject* script = ReflectScript(pEvent->script);
	E_ASSERT(script,"MONO OBJECT OF ",pEvent->script.name,"NOT LOADED");
	ScriptClass& scriptClass = scriptClassMap[pEvent->script.name];
	MonoClassField* mClassField{ scriptClass.mFields[pEvent->fieldName] };
	E_ASSERT(mClassField, "FIELD ",pEvent->fieldName,"COULD NOT BE FOUND IN SCRIPT ",pEvent->script.name);
	SetFieldValue(script, mClassField, pEvent->field);
}


void ScriptingSystem::CallbackScriptGetFieldNames(ScriptGetFieldNamesEvent* pEvent)
{
	static std::vector<const char*> fieldNames;
	pEvent->pStart = nullptr;
	pEvent->count = 0;
	fieldNames.clear();
	if (mAppDomain == nullptr || mCoreAssembly == nullptr || mAssemblyImage == nullptr)
		return;
	Scene& scene = MySceneManager.GetCurrentScene();
	if (mSceneScripts.find(scene.uuid) == mSceneScripts.end())
		return;
	MonoScripts& mScripts = mSceneScripts[scene.uuid];
	if (mScripts.find(pEvent->script) == mScripts.end())
		return;
	//MonoObject* mScript = ReflectScript(pEvent->script);
	ScriptClass& scriptClass = scriptClassMap[pEvent->script.name];
	for (auto& pair : scriptClass.mFields)
	{
		fieldNames.emplace_back(pair.first.c_str());
	}
	pEvent->pStart = fieldNames.data();
	pEvent->count = fieldNames.size();
}

void ScriptingSystem::CallbackSceneStart(SceneStartEvent* pEvent)
{
	UNREFERENCED_PARAMETER(pEvent);
	while (mAppDomain == nullptr || compilingState != CompilingState::Wait);
	logicState = LogicState::START;
	ran = true;
}

void ScriptingSystem::CallbackSceneChanging(SceneChangingEvent* pEvent)
{
	UNREFERENCED_PARAMETER(pEvent);
	while (compilingState != CompilingState::Wait);
}

void ScriptingSystem::CallbackScriptCreated(ObjectCreatedEvent<Script>* pEvent)
{
	{
		ACQUIRE_SCOPED_LOCK(ScriptQueue);
		reflectionQueue.push_back(*pEvent->pObject);
	}
	while (!reflectionQueue.empty());
}


void ScriptingSystem::CallbackSceneCleanup(SceneCleanupEvent* pEvent)
{
	(void)pEvent;
	logicState = LogicState::EXIT;
	ran = false;
	while (ran == false);
}

void ScriptingSystem::CallbackGetScriptNames(GetScriptNamesEvent* pEvent)
{
	static std::vector<const char*> names;
	names.clear();
	for (auto& pair : scriptClassMap)
	{
		if (IsScript(pair.second.mClass))
			names.push_back(pair.first.c_str());
	}
	pEvent->arr = names.data();
	pEvent->count = names.size();
}
