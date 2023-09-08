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
//#include "Scripting/script-wrappers.h"
#include <Scene/SceneManager.h>
#include <Core/EventsManager.h>
#include <Scene/components.h>

#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"
#include <mono/jit/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/exception.h>
#include <mutex>
#include <mono/metadata/environment.h>


#define SECONDS_TO_RECOMPILE 1.f

namespace
{
	MonoDomain* mRootDomain{ nullptr };		//JIT RUNTIME DOMAIN
	MonoDomain* mAppDomain{ nullptr };		//APP DOMAIN
	MonoAssembly* mCoreAssembly{ nullptr };	//ASSEMBLY OF SCRIPTS.DLL
	MonoImage* mAssemblyImage{ nullptr };	//LOADED IMAGE OF SCRIPTS.DLL
	MonoClass* mGameObject{ nullptr };
	MonoClass* mComponent{ nullptr };
	MonoClass* mCopiumScript{ nullptr };
	MonoClass* mCollision2D{ nullptr };
	MonoClass* mScriptableObject{};
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

	FieldType monoTypeToFieldType(MonoType* monoType)
	{
		//std::string typeName = mono_type_get_name_full(monoType, MONO_TYPE_NAME_FORMAT_FULL_NAME);
		//auto it = fieldTypeMap.find(typeName);
		//if (it == fieldTypeMap.end())
		//{
		//	if (typeName.find_first_of("CopiumEngine.") == 0)
		//	{
		//		std::string componentName = typeName.substr(typeName.find_last_of('.') + 1);

		//		if (mono_class_get_parent(mono_class_from_mono_type(monoType)) == mCopiumScript)
		//			return (FieldType)ComponentType::Script;
		//		auto iter{ NAME_TO_CTYPE.find(componentName) };
		//		if (iter == NAME_TO_CTYPE.end())
		//		{
		//			return FieldType::None;
		//		}
		//		//PRINT(componentName << " is a component");
		//		return (FieldType)iter->second;
		//	}
		//	if (mono_class_get_parent(mono_class_from_mono_type(monoType)) == mCopiumScript)
		//		return (FieldType)ComponentType::Script;
		//	//PRINT(typeName << "is none type");
		//	return FieldType::None;
		//}
		//return it->second;
		return FieldType::None;
	}
}

	bool scriptIsLoaded(const std::filesystem::path&);
	bool scriptPathExists(const std::filesystem::path& filePath);
	//ScriptingEngine Namespace Functions

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
				FieldType fieldType = Utils::monoTypeToFieldType(type);
				//PRINT(mono_type_get_name(type) << (int)fieldType);
				if (fieldType != FieldType::None)
				{
					mFields[fieldName] = field;
				}
			}
		}
	}

#pragma endregion

bool ScriptingSystem::isScriptableObject(const std::string& name)
{
	return false;
	//return scriptableObjectClassMap[name].mClass;
}

bool ScriptingSystem::isScript(const std::string& name)
{
	return false;
	//return scriptClassMap[name].mClass;
}

std::string ScriptingSystem::addEmptyScript(const std::string& _name)
{
	//std::string filePath{ Paths::assetPath + "\\Scripts\\" + _name + ".cs" };
	//std::ofstream file(filePath);
	//file << "using CopiumEngine;\n";
	//file << "using System;\n\n";
	//file << "public class " << _name << ": CopiumScript\n{\n";
	//file << "\tvoid Start()\n\t{\n\n\t}\n";
	//file << "\tvoid Update()\n\t{\n\n\t}\n";
	//file << "}\n";
	//file.close();
	//return filePath;
	return std::string();
}

MonoType* ScriptingSystem::getMonoTypeFromName(std::string& name)
{
	return mono_reflection_type_from_name(name.data(), mAssemblyImage);
}

void ScriptingSystem::RecompileThreadWork()
{
	PRINT("RECOMPLING\n");
	//compilingStateReadable.lock();
	//while (compilingState == CompilingState::SwapAssembly);
	//compilingState = CompilingState::Compiling;
	//Critical section
	//while (!THREADS.AcquireMutex(MutexType::FileSystem));
	//tryRecompileDll();
	//THREADS.returnMutex(MutexType::FileSystem);
	Utils::CompileDll();
	//compilingState = CompilingState::SwapAssembly;
	//Critical section End
	//compilingStateReadable.unlock();
	PRINT("RECOMPLING END\n");
}

//template<typename T, typename... Ts>
//void ScriptingSystem::SubscribeComponentBasedCallbacks(TemplatePack<T, Ts...> pack)
//{
//	MyEventSystem->subscribe(this, &ScriptingSystem::CallbackScriptSetFieldReference<T>);
//	MyEventSystem->subscribe(this, &ScriptingSystem::CallbackReflectComponent<T>);
//	if constexpr (sizeof...(Ts) != 0)
//	{
//		SubscribeComponentBasedCallbacks(TemplatePack<Ts...>());
//	}
//}

void ScriptingSystem::Init()
{
	InitMono();
	//registerScriptWrappers();
	//ENABLE FOR EDITOR MODE
	//THREADS.AddThread(&ScriptingSystem::RecompileThreadWork, this);
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackScriptModified);
	//ENABLE FOR PLAY MODE
	//swapDll();
	//MyEventSystem->subscribe(this,&ScriptingSystem::CallbackSceneChanging);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackScriptInvokeMethod);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackScriptGetMethodNames);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackScriptSetField);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackScriptGetField);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackScriptGetNames);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackScriptNew);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackReflectGameObject);
	//SubscribeComponentBasedCallbacks(ComponentTypes());
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackScriptSetFieldReference<GameObject>);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackStartPreview);
	//MyEventSystem->subscribe(this, &ScriptingSystem::CallbackStopPreview);
}

void ScriptingSystem::Update(float dt)
{
	//MyEventSystem->publish(new EditorConsoleLogEvent(std::string("STATE: " + std::to_string((int)compilingState))));
	//MyEventSystem->publish(new EditorConsoleLogEvent(std::string("PLAYMODE: " + std::to_string(inPlayMode))));
	//if (compilingState == CompilingState::SwapAssembly && !inPlayMode)
	//{
	//	swapDll();
	//	compilingState = CompilingState::Wait;
	//}

	//Pause timer when recompiling
	if (timeUntilRecompile > 0)
	{
		timeUntilRecompile -= dt;
		if (timeUntilRecompile < 0)
		{
			PRINT("START RECOMPLING\n");
			THREADS.EnqueueTask([this] {RecompileThreadWork(); });
		}
	}
}

void ScriptingSystem::Exit()
{
	//for (uint32_t hand : gcHandles)
	//{
	//	mono_gchandle_free(hand);
	//}
	//gcHandles.clear();
	//unloadAppDomain();
	ShutdownMono();
}

MonoObject* ScriptingSystem::instantiateClass(MonoClass* mClass)
{
	//COPIUM_ASSERT(mAppDomain == nullptr, "MONO APP DOMAIN NOT LOADED");
	//COPIUM_ASSERT(mClass == nullptr, "MONO CLASS NOT LOADED");
	//		
	//MonoObject* tmp = mono_object_new(mAppDomain, mClass);
	//gcHandles.push_back(mono_gchandle_new(tmp,true));
	//mono_runtime_object_init(tmp);
	//return tmp;
	return nullptr;
}

void ScriptingSystem::updateScriptClasses()
{
	//scriptClassMap.clear();

	//const MonoTableInfo* table_info = mono_image_get_table_info(mAssemblyImage, MONO_TABLE_TYPEDEF);

	//int rows = mono_table_info_get_rows(table_info);

	///* For each row, get some of its values */
	//for (int i = 0; i < rows; i++)
	//{
	//	MonoClass* _class = nullptr;
	//	uint32_t cols[MONO_TYPEDEF_SIZE];
	//	mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
	//	const char* name = mono_metadata_string_heap(mAssemblyImage, cols[MONO_TYPEDEF_NAME]);
	//	const char* name_space = mono_metadata_string_heap(mAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
	//	_class = mono_class_from_name(mAssemblyImage, name_space, name);
	//	if (!_class)
	//		continue;
	//	if (mono_class_get_parent(_class) == mCopiumScript)
	//	{
	//		scriptClassMap[name] = ScriptClass{ name,_class };
	//		reflectionMap[mono_class_get_type(_class)] = ComponentType::Script;
	//	}
	//	else if(mono_class_get_parent(_class) == mScriptableObject)
	//	{
	//		scriptableObjectClassMap[name] = ScriptClass{ name,_class };
	//	}
	//	else if (mono_class_get_parent(_class) == mono_class_from_name(mAssemblyImage, name_space, "Component"))
	//	{
	//		if (_class == mCopiumScript)
	//			continue;
	//		scriptClassMap[name] = ScriptClass{ name,_class };
	//		reflectionMap[mono_class_get_type(_class)] = NAME_TO_CTYPE[name];
	//	}
	//}
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

void ScriptingSystem::createAppDomain()
{
	static char appName[] = "AppDomain";
	mAppDomain = mono_domain_create_appdomain(appName, nullptr);
	mono_domain_set(mAppDomain, false);
}

void ScriptingSystem::unloadAppDomain()
{
	/*if (mAppDomain)
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
			if (mono_class_get_parent(_class) == mCopiumScript)
			{
				vTable = mono_class_vtable(mAppDomain, _class);
				scriptClassMap[name] = ScriptClass{ name,_class };
				reflectionMap[mono_class_get_type(_class)] = ComponentType::Script;

			}
			else if (mono_class_get_parent(_class) == mScriptableObject)
			{
				vTable = mono_class_vtable(mAppDomain, _class);
				scriptableObjectClassMap[name] = ScriptClass{ name,_class };
			}
			else if (mono_class_get_parent(_class) == mono_class_from_name(mAssemblyImage, name_space, "Component"))
			{
				if (_class == mCopiumScript)
					continue;
				vTable = mono_class_vtable(mAppDomain, _class);
				scriptClassMap[name] = ScriptClass{ name,_class };
				reflectionMap[mono_class_get_type(_class)] = NAME_TO_CTYPE[name];

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
	}*/
}

template <typename T, typename... Ts>
struct ReflectExistingStruct
{
	ReflectExistingStruct(TemplatePack<T,Ts...> pack) {};
	ReflectExistingStruct() 
	{
		//Scene* pScene{ MySceneManager.get_current_scene() };
		//if (!pScene)
		//	return;
		//for (GameObject& go : pScene->gameObjects)
		//{
		//	MyScriptingSystem.ReflectGameObject(go);
		//}
		//Reflect<T, Ts...>(*pScene);
	}
	template <typename T1, typename... T1s>
	void Reflect(Scene& scene)
	{
		//for (T1& component : scene.componentArrays.GetArray<T1>())
		//{
		//	MyScriptingSystem.ReflectComponent(component);
		//}
		//if constexpr (sizeof...(T1s) != 0)
		//	Reflect<T1s...>(scene);
	}
};

//using ReflectAll = decltype(ReflectExistingStruct(ComponentTypes()));

void ScriptingSystem::swapDll()
{
	//MyEventSystem->publish(new EditorConsoleLogEvent("SWAPPING DLL"));

	//PRINT("CLEARING HANDLES!");
	//for (uint32_t hand : gcHandles)
	//{
	//	mono_gchandle_free(hand);

	//}
	//gcHandles.clear();
	//registerScriptWrappers();


	//PRINT("UNLOADING APP DOMAIN!");
	//mGameObjects.clear();
	//mComponents.clear();
	//unloadAppDomain();
	//PRINT("RECREATING APP DOMAIN!");
	//createAppDomain();
	//PRINT("LOADING ASSEMBLY!");
	//mCoreAssembly = Utils::loadAssembly(Paths::scriptsAssemblyPath);
	//mAssemblyImage = mono_assembly_get_image(mCoreAssembly);
	//mGameObject = mono_class_from_name(mAssemblyImage, "CopiumEngine", "GameObject");
	//mCopiumScript = mono_class_from_name(mAssemblyImage, "CopiumEngine", "CopiumScript");
	//mCollision2D = mono_class_from_name(mAssemblyImage, "CopiumEngine", "Collision2D");
	//mScriptableObject = mono_class_from_name(mAssemblyImage, "CopiumEngine", "ScriptableObject");
	//mComponent = mono_class_from_name(mAssemblyImage, "CopiumEngine", "Component");
	//updateScriptClasses();
	//if (MySceneManager.get_current_scene())
	//{
	//	PRINT("REFLECTING!");
	//	ReflectAll();
	//}
	

		
	//COPIUM_ASSERT(!mGameObject, "GameObject C# script could not be loaded");
	//COPIUM_ASSERT(!mCopiumScript, "CopiumScript C# script could not be loaded");
	//COPIUM_ASSERT(!mCollision2D, "Collision2D C# script could not be loaded");
	//COPIUM_ASSERT(!mScriptableObject, "ScriptableObject C# script could not be loaded");
	//COPIUM_ASSERT(!mScriptableObject, "Scene C# script could not be loaded");
	//messageSystem.dispatch(MESSAGE_TYPE::MT_CREATE_CS_GAMEOBJECT);
	//messageSystem.dispatch(MESSAGE_TYPE::MT_SCRIPTING_UPDATED);
	//MyEventSystem->publish(new EditorConsoleLogEvent("END SWAP DLL"));
}

MonoObject* ScriptingSystem::invoke(MonoObject* mObj, MonoMethod* mMethod, void** params)
{
	//COPIUM_ASSERT(mMethod==nullptr,"MONO METHOD WAS NULLPTR");
	//if (mObj && mMethod && mAppDomain)
	//{
	//	try
	//	{
	//		MonoObject* exception = NULL;
	//		MonoObject* obj = mono_runtime_invoke(mMethod, mObj, params, &exception);
	//		if (exception)
	//		{
	//			const char* message = mono_string_to_utf8(mono_object_to_string(exception, NULL));
	//			MyEventSystem->publish(new EditorConsoleLogEvent(message));
	//		}
	//		return obj;
	//	}
	//	catch (...)
	//	{

	//	}
	//}
	return nullptr;
}

MonoObject* ScriptingSystem::getFieldMonoObject(MonoClassField* mField, MonoObject* mObject)
{
	if (mAppDomain == nullptr)
	{
		PRINT("APP DOMAIN WAS NULL");
		return nullptr;
	}
	return mono_field_get_value_object(mAppDomain, mField, mObject);
}

void ScriptingSystem::updateScriptFiles()
{
	//Check for new files
	//namespace fs = std::filesystem;
	//using scriptFileListIter = std::list<File>::iterator;
	//scriptFileListIter scriptFilesIt = scriptFiles.begin();
	//static std::list<File*> maskScriptFiles;
	//maskScriptFiles.resize(scriptFiles.size());
	//for (File& file : scriptFiles)
	//{
	//	maskScriptFiles.push_back(&file);
	//}
	//for (const fs::directory_entry& p : fs::recursive_directory_iterator(Paths::projectPath))
	//{
	//	const fs::path& pathRef{ p.path() };
	//	if (pathRef.extension() != ".cs")
	//		continue;

	//	//Detect new scripts
	//	if (!scriptIsLoaded(pathRef))
	//	{
	//		scriptFiles.emplace(scriptFilesIt, File(pathRef));
	//	}
	//	//Script was already loaded
	//	else
	//	{
	//		//Set scripts to be masked
	//		for (File* scriptFile : maskScriptFiles)
	//		{
	//			if (scriptFile && scriptFile->filePath == pathRef)
	//			{
	//				maskScriptFiles.remove(scriptFile);
	//				break;
	//			}
	//		}
	//		++scriptFilesIt;
	//	}
	//}
}

MonoObject* ScriptingSystem::cloneInstance(MonoObject* _instance)
{
	if (!_instance)
		return nullptr;
	return mono_object_clone(_instance);
}

MonoObject* ScriptingSystem::createInstance(MonoClass* _mClass)
{
	return mono_object_new(mAppDomain,_mClass);
}

bool ScriptingSystem::scriptIsLoaded(const std::filesystem::path& filePath)
{
	//using scriptFileListIter = std::list<File>::iterator;
	//for (scriptFileListIter it = scriptFiles.begin(); it != scriptFiles.end(); ++it)
	//{
	//	if (it->filePath == filePath) return true;
	//}
	return false;
}

MonoString* ScriptingSystem::createMonoString(const char* str)
{
	if (!mAppDomain)
	{
		PRINT("APP DOMAIN NOT LOADED");
	}
	return mono_string_new(mAppDomain, str);
}

void ScriptingSystem::GetFieldValue(MonoObject* instance, MonoClassField* mClassFiend ,Field& field, void* container)
{
	//PRINT("Get field value: " << mono_field_get_name(mClassFiend));
	//if (field.fType == FieldType::String)
	//{
	//	MonoString* mono_string = createMonoString("");
	//	mono_field_get_value(instance, mClassFiend, &mono_string);
	//	char* str = mono_string_to_utf8(mono_string);
	//	strcpy((char*)container, str);
	//	return;
	//}
	//mono_field_get_value(instance, mClassFiend, container);
	return;
}

void ScriptingSystem::SetFieldValue(MonoObject* instance, MonoClassField* mClassFiend, Field& field, const void* value)
{
	//THIS FUNCTION ONLY WORKS FOR BASIC TYPES
	field = value;
	//If its a string, its a C# string so create one
	//PRINT("Set field value: " << mono_field_get_name(mClassFiend));
	if (field.fType == FieldType::String)
	{
		MonoString* mono_string = createMonoString(reinterpret_cast<const char*>(value));
		mono_field_set_value(instance, mClassFiend, mono_string);
		return;
	}
	mono_field_set_value(instance, mClassFiend, (void*)value);
	return;
}

//template<typename T>
//void ScriptingSystem::SetFieldReference(MonoObject* instance, MonoClassField* mClassFiend, T* reference)
//{
//	//When you set a reference, you need to create a MonoObject of it first
//
//	//PRINT("set field ref: " << mono_field_get_name(mClassFiend));
//	//ZACH: If setting to nullptr, no point checking
//	if (reference == nullptr)
//	{
//		mono_field_set_value(instance, mClassFiend, nullptr);
//		return;
//	}
//	//ZACH: Trying to set a component reference
//	if constexpr (ComponentTypes::has<T>())
//	{
//		mono_field_set_value(instance, mClassFiend, ReflectComponent(*reference));
//	}
//	//ZACH: Trying to set a gameobject reference
//	else if (std::is_same<T,GameObject>())
//	{
//		mono_field_set_value(instance, mClassFiend, ReflectGameObject(*reference));
//	}
//	else
//	{
//		static_assert(true);
//	}
//}

void ScriptingSystem::CallbackScriptModified(FileModifiedEvent<FileType::SCRIPT>* pEvent)
{
	PRINT("SCRIPT CHANGE DETECTED!\n");
	timeUntilRecompile = SECONDS_TO_RECOMPILE;
}

//MonoObject* ScriptingSystem::ReflectGameObject(GameObject& gameObj)
//{
//	auto pairIt = mGameObjects.find(gameObj.uuid);
//	if (pairIt == mGameObjects.end())
//	{
//		size_t address = (size_t)&gameObj;
//		void* param = &address;
//		MonoMethod* reflectGameObject = mono_class_get_method_from_name(mGameObject, "Initialize", 1);
//		MonoObject* instance{instantiateClass(mGameObject)};
//		invoke(instance, reflectGameObject, &param);
//		mGameObjects.emplace(gameObj.uuid, instance);
//		return instance;
//	}
//	return (*pairIt).second;
//}

//template <typename T>
//MonoObject* ScriptingSystem::ReflectComponent(T& component)
//{
//	auto pairIt = mComponents.find(component.uuid);
//	if (pairIt == mComponents.end())
//	{
//		size_t address = (size_t)&component;
//		ScriptClass& scriptClass = scriptClassMap[GetComponentType<T>::name];
//		MonoObject* gameObjectInstance{ReflectGameObject(component.gameObj)};
//		MonoObject* instance = instantiateClass(scriptClass.mClass);
//		void* params[2] = { gameObjectInstance,&address };
//		MonoMethod* reflectComponent = mono_class_get_method_from_name(mComponent, "Initialize", 2);
//		COPIUM_ASSERT(!mComponent, "MonoObject was null");
//		invoke(instance, reflectComponent, params);
//		mComponents.emplace(component.uuid, instance);
//		return instance;
//	}
//	return (*pairIt).second;
//}
//
//template <>
//MonoObject* ScriptingSystem::ReflectComponent(Script& component)
//{
//	auto pairIt = mComponents.find(component.uuid);
//	if (pairIt == mComponents.end())
//	{
//		if (scriptClassMap.find(component.Name()) == scriptClassMap.end())
//			return nullptr;
//		size_t address = (size_t)&component;
//		ScriptClass& scriptClass = scriptClassMap[component.Name()];
//		MonoObject* gameObjectInstance{ ReflectGameObject(component.gameObj) };
//		MonoObject* instance = instantiateClass(scriptClass.mClass);
//		void* params[2] = { gameObjectInstance,&address };
//		//PRINT("C++ COMPONENT: " << (size_t)&component);
//		MonoMethod* reflectComponent = mono_class_get_method_from_name(mComponent, "Initialize", 2);
//		COPIUM_ASSERT(!mComponent, "MonoObject was null");
//		invoke(instance, reflectComponent, params);
//		mComponents.emplace(component.uuid, instance);
//		//Check fields, dont remove fields, but change them if their type is different
//
//		std::list<std::string> validFieldNames{};
//
//		for (auto& pair : component.fieldDataReferences)
//		{
//			validFieldNames.push_back(pair.first);
//		}
//
//		//PRINT("Creating: " << component.Name() << " of id: " << component.uuid);
//		Script& script{ *reinterpret_cast<Script*>(&component) };
//		for (auto& pair : scriptClass.mFields)
//		{
//			MonoClassField* mField = pair.second;
//			MonoType* type = mono_field_get_type(mField);
//			FieldType fieldType = Utils::monoTypeToFieldType(type);
//			const char* fieldName = pair.first.c_str();
//			std::string typeName = mono_type_get_name(type);
//			//PRINT(typeName << " " << (int)fieldType);
//			auto nameField{ script.fieldDataReferences.find(fieldName) };
//			int alignment{};
//			int fieldSize = mono_type_size(type, &alignment);
//			//POINTER
//			if (fieldType == FieldType::Component || fieldType == FieldType::GameObject)
//			{
//				fieldSize = sizeof(uint64_t);
//			}
//			else if(fieldType == FieldType::String)
//			{
//				fieldSize = TEXT_BUFFER_SIZE;
//			}
//
//
//			//Field has not been created onto script yet
//			if (nameField == script.fieldDataReferences.end())
//			{
//				Field newField = Field(fieldType, fieldSize, nullptr);
//				size_t offset = typeName.find_last_of(".");
//				if (offset != std::string::npos)
//					newField.typeName = typeName.substr(offset + 1);
//				else
//					newField.typeName = typeName;
//				if (fieldType == FieldType::Component || fieldType == FieldType::GameObject)
//				{
//					newField = std::numeric_limits<uint64_t>::max();
//				}
//				else
//				{
//					MyEventSystem->publish(new ScriptGetFieldEvent(script, fieldName, newField.data));
//				}
//				script.fieldDataReferences[fieldName] = std::move(newField);
//			}
//			//Field exists
//			else
//			{
//				validFieldNames.remove(fieldName);
//				Field& field = nameField->second;
//				//If the field type is not the same
//				if (field.fType != fieldType)
//				{
//					field.Resize(fieldSize);
//					mono_field_get_value(instance, mField, field.data);
//				}
//				//Field exists, setback the values first
//				else
//				{
//						
//					if (field.fType == FieldType::GameObject)
//					{
//						GameObject* reference = script.fieldGameObjReferences[fieldName];
//						MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, reference));
//					}
//					else if (field.fType == FieldType::Component)
//					{
//						Component* reference = script.fieldComponentReferences[fieldName];
//						switch ((ComponentType)field.fType)
//						{
//						case(ComponentType::Animator):
//						{
//							MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Animator*)reference));
//							break;
//						}
//						case(ComponentType::AudioSource):
//						{
//							MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (AudioSource*)reference));
//							break;
//						}
//						case(ComponentType::BoxCollider2D):
//						{
//							MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (BoxCollider2D*)reference));
//							break;
//						}
//						case(ComponentType::Button):
//						{
//							MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Button*)reference));
//							break;
//						}
//						case(ComponentType::Camera):
//						{
//							MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Camera*)reference));
//							break;
//						}
//						case(ComponentType::Image):
//						{
//							MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Image*)reference));
//							break;
//						}
//						case(ComponentType::Rigidbody2D):
//						{
//							MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Rigidbody2D*)reference));
//							break;
//						}
//						case(ComponentType::SpriteRenderer):
//						{
//							MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (SpriteRenderer*)reference));
//							break;
//						}
//						case(ComponentType::Script):
//						{
//							MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Script*)reference));
//							break;
//						}
//						case(ComponentType::Text):
//						{
//							MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Text*)reference));
//							break;
//						}
//						case(ComponentType::SortingGroup):
//						{
//							MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (SortingGroup*)reference));
//							break;
//						}
//						}
//					}
//					else
//					{
//						SetFieldValue(instance, mField, field,field.data);
//					}
//				}
//				//CHECK TYPENAME
//				//else if (field.typeName)
//			}
//		}
//		for (auto& name : validFieldNames)
//		{
//			FieldType fType = script.fieldDataReferences[name].fType;
//			if (fType == FieldType::GameObject)
//			{
//				component.fieldGameObjReferences.erase(name);
//			}
//			else if (fType >= FieldType::Component)
//			{
//				component.fieldComponentReferences.erase(name);
//			}
//			component.fieldDataReferences.erase(name);
//			//PRINT("INVALID FIELD: " << name);
//		}
//		return instance;
//	}
//	return (*pairIt).second;
//}

//void ScriptingSystem::CallbackSceneChanging(SceneChangingEvent* pEvent)
//{
//	//If there is no assembly loaded at all
//	if (mAssemblyImage == nullptr)
//	{
//		//Wait if it is still compiling
//		compilingStateReadable.lock();
//		while (compilingState == CompilingState::Compiling) {
//			PRINT("COMPILING!!");
//		};
//		//If it finished compiling and needs to swap
//		if (compilingState == CompilingState::SwapAssembly)
//		{
//			PRINT("SWAP ASSEMBLY!!");
//			//Swap dll and set back to wait for compiling
//			swapDll();
//			compilingState = CompilingState::Wait;
//		}
//		compilingStateReadable.unlock();
//	}
//	for (uint32_t hand : gcHandles)
//	{
//		mono_gchandle_free(hand);
//	}
//	gcHandles.clear();
//	mGameObjects.clear();
//	mComponents.clear();
//	PRINT("CREATING NEW SCENE!");
//	ReflectAll();
//}
//
//template <typename T>
//void ScriptingSystem::CallbackReflectComponent(ReflectComponentEvent<T>* pEvent)
//{
//	ReflectComponent(pEvent->component);
//}
//
//void ScriptingSystem::CallbackScriptInvokeMethod(ScriptInvokeMethodEvent* pEvent)
//{
//	MonoObject* mScript = ReflectComponent(pEvent->script);
//	//PRINT("Script Invoking " << pEvent->script.Name() << " " << pEvent->methodName << " ,ID: " << pEvent->script.uuid);
//	COPIUM_ASSERT(!mScript, std::string("MONO OBJECT OF ") + pEvent->script.name + std::string(" NOT LOADED"));
//	ScriptClass& scriptClass{ scriptClassMap[pEvent->script.name] };
//	MonoMethod* mMethod{ mono_class_get_method_from_name (scriptClass.mClass,pEvent->methodName.c_str(),(int)pEvent->paramCount)};
//	if (!mMethod && mono_class_get_parent(scriptClass.mClass) == mCopiumScript)
//	{
//		mMethod = mono_class_get_method_from_name(mCopiumScript, pEvent->methodName.c_str(), (int)pEvent->paramCount);
//		if (!mMethod)
//			return;
//	}
//	COPIUM_ASSERT(!mMethod, std::string("MONO METHOD ") + pEvent->methodName + std::string(" IN SCRIPT ") + pEvent->script.name + std::string(" NOT FOUND"));
//	invoke(mScript,mMethod,pEvent->params);
//}
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
//void ScriptingSystem::CallbackScriptSetField(ScriptSetFieldEvent* pEvent)
//{
//	MonoObject* mScript = ReflectComponent(pEvent->script);
//	COPIUM_ASSERT(!mScript, std::string("MONO OBJECT OF ") + pEvent->script.name + std::string(" NOT LOADED"));
//	ScriptClass& scriptClass{ scriptClassMap[pEvent->script.name] };
//	MonoClassField* mClassField{ scriptClass.mFields[pEvent->fieldName] };
//	COPIUM_ASSERT(!mClassField, std::string("FIELD ") + pEvent->fieldName + "COULD NOT BE FOUND IN SCRIPT " + pEvent->script.name);
//	SetFieldValue(mScript, mClassField, pEvent->script.fieldDataReferences[pEvent->fieldName], pEvent->data);
//}
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
//void ScriptingSystem::CallbackStartPreview(StartPreviewEvent* pEvent)
//{
//	inPlayMode = true;
//	for (uint32_t hand : gcHandles)
//	{
//		mono_gchandle_free(hand);
//	}
//	gcHandles.clear();
//	mGameObjects.clear();
//	mComponents.clear();
//	ReflectAll();
//}
//
//void ScriptingSystem::CallbackStopPreview(StopPreviewEvent* pEvent)
//{
//	inPlayMode = false;
//	for (uint32_t hand : gcHandles)
//	{
//		mono_gchandle_free(hand);
//	}
//	gcHandles.clear();
//	mGameObjects.clear();
//	mComponents.clear();
//	ReflectAll();
//}
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