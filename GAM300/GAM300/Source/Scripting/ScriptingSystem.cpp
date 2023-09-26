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
	ScriptClass::ScriptClass(const std::string& _name, MonoClass* _mClass) :
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
	//MyEventSystem->subscribe(this,&ScriptingSystem::CallbackSceneChanging);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackScriptInvokeMethod);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackScriptGetMethodNames);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackScriptGetField);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackScriptGetNames);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackScriptNew);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackSceneStart);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackScriptSetField);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackSceneCleanup);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackSceneStop);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackStopPreview);
}

void ScriptingSystem::Update(float dt)
{
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
	gcHandles.push_back(mono_gchandle_new(tmp,true));
	mono_runtime_object_init(tmp);
	return tmp;
}

void ScriptingSystem::UpdateScriptClasses()
{
	scriptClassMap.clear();
	const MonoTableInfo* table_info = mono_image_get_table_info(mAssemblyImage, MONO_TABLE_TYPEDEF);
	int rows = mono_table_info_get_rows(table_info);
	/* For each row, get some of its values */
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
			scriptClassMap[name] = ScriptClass{ name,_class };
		}
		else if (mono_class_get_parent(_class) == mono_class_from_name(mAssemblyImage, name_space, "Component"))
		{
			if (_class == mScript)
				continue;
			scriptClassMap[name] = ScriptClass{ name,_class };
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

void ScriptingSystem::CreateAppDomain()
{
	static char appName[] = "AppDomain";
	mAppDomain = mono_domain_create_appdomain(appName, nullptr);
	mono_domain_set(mAppDomain, false);
}

void ScriptingSystem::UnloadAppDomain()
{
	if (mAppDomain)
	{
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
			MonoVTable* vTable = nullptr;
			if (mono_class_get_parent(_class) == mScript)
			{
				vTable = mono_class_vtable(mAppDomain, _class);
				scriptClassMap[name] = ScriptClass{ name,_class };

			}
			else if (mono_class_get_parent(_class) == mono_class_from_name(mAssemblyImage, name_space, "Component"))
			{
				if (_class == mScript)
					continue;
				vTable = mono_class_vtable(mAppDomain, _class);
				scriptClassMap[name] = ScriptClass{ name,_class };
			}

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
			}
		}
		mono_domain_set(mRootDomain, false);
		mono_domain_unload(mAppDomain);
		mAppDomain = nullptr;
	}
}

template <typename T, typename... Ts>
struct ReflectExistingStruct
{
	ReflectExistingStruct(TemplatePack<T,Ts...> pack) {};
	ReflectExistingStruct() 
	{
		Scene& scene{ MySceneManager.GetCurrentScene()};
		for (auto& component : scene.GetArray<Script>())
		{
			SCRIPTING.ReflectScript(component);
		}
	}
};

using ReflectAll = decltype(ReflectExistingStruct(AllComponentTypes()));

void ScriptingSystem::InvokeAllScripts(const std::string& funcName)
{
	Scene& scene = MySceneManager.GetCurrentScene();
	auto& scriptsArray = scene.GetArray<Script>();
	for (auto it = scriptsArray.begin();it != scriptsArray.end();++it)
	{
		Script& script = *it;
		for (auto& field : script.fields)
		{
			if (field.second.fType >= AllObjectTypes::Size())
				continue;
			Entity& entity = scene.Get<Entity>(script);
			//Reference got deleted
			Handle* pHandle = (Handle*)field.second.data;
			if (!scene.HasHandle(field.second.fType, pHandle))
			{
				memset(field.second.data, 0, field.second.GetSize());
				ScriptSetFieldEvent e{ script,field.first.c_str()};
				CallbackScriptSetField(&e);
			}
		}

		if (!it.IsActive())
			continue;
		if (!scene.IsActive(scene.Get<Entity>(script)))
			continue;
		InvokeMethod(script, funcName);
	}
}

void ScriptingSystem::ThreadWork()
{
	InitMono();
	while (!THREADS.HasStopped())
	{	
		if (logicState != LogicState::NONE)
		{
			if (ran)
				continue;
			if (logicState == LogicState::UPDATE)
			{
				InvokeAllScripts("Update");
				InvokeAllScripts("LateUpdate");
			}
			else if (logicState == LogicState::START)
			{
				for (uint32_t hand : gcHandles)
				{
					mono_gchandle_free(hand);
				}
				gcHandles.clear();
				mComponents.clear();
				ReflectAll();
				InvokeAllScripts("Awake");
				InvokeAllScripts("Start");
				logicState = LogicState::UPDATE;
			}
			else if (logicState == LogicState::EXIT)
			{
				InvokeAllScripts("Exit");
				logicState = LogicState::CLEANUP;
			}
			else
			{
				for (uint32_t hand : gcHandles)
				{
					mono_gchandle_free(hand);
				}
				gcHandles.clear();
				mComponents.clear();
				ReflectAll();
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
			ACQUIRE_SCOPED_LOCK(Mono);
			ACQUIRE_SCOPED_LOCK(Assets);
			timeUntilRecompile -= 1;
			if (timeUntilRecompile <= 0)
			{
				THREADS.EnqueueTask([this] {RecompileThreadWork(); });
			}
		}
		else if (compilingState == CompilingState::SwapAssembly)
		{
			SwapDll();
		}
		#endif
	}
	for (uint32_t hand : gcHandles)
	{
		mono_gchandle_free(hand);
	}
	gcHandles.clear();
	UnloadAppDomain();
	
	ShutdownMono();
	PRINT("MONO THREAD ENDED!\n");
}

void ScriptingSystem::SwapDll()
{
	//Load Mono
	PRINT("SWAPPING DLL\n");
	//ACQUIRE_SCOPED_LOCK(Assets);
	ACQUIRE_SCOPED_LOCK(Mono);
	for (uint32_t hand : gcHandles)
	{
		mono_gchandle_free(hand);
	}
	gcHandles.clear();
	mComponents.clear();
	UnloadAppDomain();
	CreateAppDomain();
	mCoreAssembly = Utils::loadAssembly("scripts.dll");
	mAssemblyImage = mono_assembly_get_image(mCoreAssembly);
	mScript = mono_class_from_name(mAssemblyImage, "BeanFactory", "Script");
	RegisterScriptWrappers();
	RegisterComponents();
	UpdateScriptClasses();
	ReflectAll();
	compilingState = CompilingState::Wait;
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

void ScriptingSystem::GetFieldValue(MonoObject* instance, MonoClassField* mClassFiend ,Field& field, void* container)
{
	if (field.fType == GetFieldType::E<std::string>())
	{
		MonoString* mono_string = CreateMonoString("");
		mono_field_get_value(instance, mClassFiend, &mono_string);
		char* str = mono_string_to_utf8(mono_string);
		strcpy_s((char*)container, strlen(str)+1, str);
		return;
	}
	mono_field_get_value(instance, mClassFiend, container);
	return;
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
		Engine::UUID euid = *(Engine::UUID*)field.data;
		Scene& scene = MySceneManager.GetCurrentScene();
		if (euid == 0)
		{
			mono_field_set_value(instance, mClassField, nullptr);
			return;
		}
		void* obj = scene.GetByUUID(field.fType, (void*)euid);
		if (field.fType == GetType::E<Script>())
		{
			mono_field_set_value(instance, mClassField, ReflectScript(*(Script*)obj));
		}
		else
		{
			mono_field_set_value(instance, mClassField, obj);
		}
		return;
	}
	mono_field_set_value(instance, mClassField, (void*)field.data);
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

MonoObject* ScriptingSystem::ReflectScript(Script& script)
{
	auto pairIt = mComponents.find(&script);
	if (pairIt == mComponents.end())
	{
		if (scriptClassMap.find(script.name) == scriptClassMap.end())
			return nullptr;
		ScriptClass& scriptClass = scriptClassMap[script.name];
		Scene& scene = MySceneManager.Instance().GetCurrentScene();
		MonoObject* instance = InstantiateClass(scriptClass.mClass);
		void* param = &scene.Get<Entity>(script);
		MonoMethod* reflectComponent = mono_class_get_method_from_name(mScript, "Initialize", 1);
		invoke(instance, reflectComponent, &param);
		mComponents.emplace(&script, instance);
		//Check fields, dont remove fields, but change them if their type is different

		std::list<std::string> validFieldNames{};

		for (auto& pair : script.fields)
		{
			validFieldNames.push_back(pair.first);
		}
		for (auto& pair : scriptClass.mFields)
		{
			MonoClassField* mField = pair.second;
			MonoType* type = mono_field_get_type(mField);
			size_t fieldType = Utils::monoTypeToFieldType(type);
			const char* fieldName = pair.first.c_str();
			std::string typeName = mono_type_get_name(type);
			auto nameField{ script.fields.find(fieldName) };
			int alignment{};
			int fieldSize = mono_type_size(type, &alignment);
			if (fieldType < AllObjectTypes::Size())
			{
				fieldSize = sizeof(Object);
			}
			else if (fieldType == GetFieldType::E<std::string>())
			{
				fieldSize = TEXT_BUFFER_SIZE;
			}
			//Field has not been created onto script yet
			if (nameField == script.fields.end())
			{
				Field newField = Field((size_t)fieldType, fieldSize, nullptr);
				size_t offset = typeName.find_last_of(".");
				if (offset != std::string::npos)
					newField.typeName = typeName.substr(offset + 1);
				else
					newField.typeName = typeName;
				memset(newField.data,0,newField.GetSize());
				script.fields[fieldName] = std::move(newField);
			}
			//Field exists
			else
			{
				validFieldNames.remove(fieldName);
				Field& field = nameField->second;
				//If the field type is not the same
				if (field.fType != (size_t)fieldType)
				{
					field.Resize(fieldSize);
					mono_field_get_value(instance, mField, field.data);
				}
				//Field exists, setback the values first
				else
				{
					//Look at this again
					SetFieldValue(instance, mField, field);
				}
			}
		}
		for (auto& name : validFieldNames)
		{
			script.fields.erase(name);
		}
		return instance;
	}
	/*return (*pairIt).second;*/
	return pairIt->second;
}

//
//
//
//void ScriptingSystem::CallbackScriptGetField(ScriptGetFieldEvent* pEvent)
//{
//	MonoObject* mScript = ReflectComponent(pEvent->script);
//	COPIUM_ASSERT(!mScript, std::string("MONO OBJECT OF ") + pEvent->script.name + std::string(" NOT LOADED"));
//	ScriptClass& scriptClass{ scriptClassMap[pEvent->script.name] };
//	//Reset fieldtype and buffer if the type was different
//	MonoClassField* mClassField{scriptClass.mFields[pEvent->fieldName]};
//	COPIUM_ASSERT(!mClassField, std::string("FIELD ") + pEvent->fieldName + "COULD NOT BE FOUND IN SCRIPT " + pEvent->script.name);
//	GetFieldValue(mScript,mClassField,pEvent->script.fieldDataReferences[pEvent->fieldName],pEvent->container);
//}
//
void ScriptingSystem::CallbackScriptSetField(ScriptSetFieldEvent* pEvent)
{
	MonoObject* mScript = ReflectScript(pEvent->script);
	E_ASSERT(mScript,"MONO OBJECT OF ",pEvent->script.name,"NOT LOADED");
	ScriptClass& scriptClass = scriptClassMap[pEvent->script.name];
	MonoClassField* mClassField{ scriptClass.mFields[pEvent->fieldName] };
	E_ASSERT(mClassField, "FIELD ",pEvent->fieldName,"COULD NOT BE FOUND IN SCRIPT ",pEvent->script.name);
	Field& field = pEvent->script.fields[pEvent->fieldName];
	SetFieldValue(mScript, mClassField, field);
}
//
//
//void ScriptingSystem::CallbackScriptGetMethodNames(ScriptGetMethodNamesEvent* pEvent)
//{
//	static std::vector<const char*> functionNames{};
//	functionNames.clear();
//	ScriptClass& scriptClass{ scriptClassMap[pEvent->script.name] };
//	for (auto& pair : scriptClass.mFields)
//	{
//		functionNames.push_back(pair.first.c_str());
//	}
//	pEvent->arraySize = functionNames.size();
//	pEvent->namesArray = functionNames.data();
//}
//
//void ScriptingSystem::CallbackReflectGameObject(ReflectGameObjectEvent* pEvent)
//{
//	ReflectGameObject(pEvent->gameObject);
//}
//

void ScriptingSystem::CallbackSceneStart(SceneStartEvent* pEvent)
{
	ACQUIRE_UNIQUE_LOCK(Mono, [this] {return mAppDomain != nullptr; });
	logicState = LogicState::START;
	ran = true;
}
void ScriptingSystem::CallbackSceneCleanup(SceneCleanupEvent* pEvent)
{
	logicState = LogicState::EXIT;
	ran = false;
	while (ran == false);
}

void ScriptingSystem::CallbackSceneStop(SceneStopEvent* pEvent)
{
	logicState = LogicState::CLEANUP;
	ran = false;
	while (ran == false);
}


//
//void ScriptingSystem::CallbackScriptGetNames(ScriptGetNamesEvent* pEvent)
//{
//	for (auto& pair : scriptClassMap)
//	{
//		if ((int)NAME_TO_CTYPE[pair.first] == 0)
//			pEvent->names.push_back(pair.first.c_str());
//	}
//}
//
//
//void ScriptingSystem::CallbackScriptNew(ScriptNewEvent* pEvent)
//{
//	std::string filePath = addEmptyScript(pEvent->name);
//	MyEventSystem->publish(new FileAccessEvent(filePath.c_str()));
//}