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

namespace
{
	MonoDomain* mRootDomain{ nullptr };		//JIT RUNTIME DOMAIN
	MonoDomain* mAppDomain{ nullptr };		//APP DOMAIN
	MonoAssembly* mCoreAssembly{ nullptr };	//ASSEMBLY OF SCRIPTS.DLL
	MonoImage* mAssemblyImage{ nullptr };	//LOADED IMAGE OF SCRIPTS.DLL
	MonoClass* mGameObject{ nullptr };
	MonoClass* mComponent{ nullptr };
	MonoClass* mScript{ nullptr };
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
	PRINT("RECOMPLING\n");
	//compilingStateReadable.lock();
	//while (compilingState == CompilingState::SwapAssembly);
	ACQUIRE_SCOPED_LOCK("Assets");
	compilingState = CompilingState::Compiling;
	//Critical section
	Utils::CompileDll();
	compilingState = CompilingState::SwapAssembly;
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
	//ENABLE FOR EDITOR MODE
	EVENTS.Subscribe(this, &ScriptingSystem::CallbackScriptModified);
	THREADS.EnqueueTask([this] {RecompileThreadWork(); });
	ACQUIRE_UNIQUE_LOCK("Assets",[this]{return compilingState == CompilingState::SwapAssembly;});
	//ENABLE FOR PLAY MODE
	SwapDll();
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
	//Pause timer when recompiling
	if (timeUntilRecompile > 0)
	{
		timeUntilRecompile -= dt;
		if (timeUntilRecompile < 0)
		{
			THREADS.EnqueueTask([this] {RecompileThreadWork(); });
		}
	}
	else if (compilingState == CompilingState::SwapAssembly)
	{
		ACQUIRE_SCOPED_LOCK("Assets");
		SwapDll();
	}
}

void ScriptingSystem::Exit()
{
	for (uint32_t hand : gcHandles)
	{
		mono_gchandle_free(hand);
	}
	gcHandles.clear();
	UnloadAppDomain();
	ShutdownMono();
}

MonoObject* ScriptingSystem::InstantiateClass(MonoClass* mClass)
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
			reflectionMap[mono_class_get_type(_class)] = GetComponentType::E<Script>();
		}
		else if(mono_class_get_parent(_class) == mScriptableObject)
		{
			//scriptableObjectClassMap[name] = ScriptClass{ name,_class };
		}
		else if (mono_class_get_parent(_class) == mono_class_from_name(mAssemblyImage, name_space, "Component"))
		{
			if (_class == mScript)
				continue;
			scriptClassMap[name] = ScriptClass{ name,_class };
			reflectionMap[mono_class_get_type(_class)] = ComponentTypes[name];
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
				reflectionMap[mono_class_get_type(_class)] = GetComponentType::E<Script>();

			}
			else if (mono_class_get_parent(_class) == mScriptableObject)
			{
				vTable = mono_class_vtable(mAppDomain, _class);
				//scriptableObjectClassMap[name] = ScriptClass{ name,_class };
			}
			else if (mono_class_get_parent(_class) == mono_class_from_name(mAssemblyImage, name_space, "Component"))
			{
				if (_class == mScript)
					continue;
				vTable = mono_class_vtable(mAppDomain, _class);
				scriptClassMap[name] = ScriptClass{ name,_class };
				reflectionMap[mono_class_get_type(_class)] = ComponentTypes[name];

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
		Reflect<T, Ts...>(scene);
	}
	template <typename T1, typename... T1s>
	void Reflect(Scene& scene)
	{
		for (T1& component : scene.GetComponentsArray<T1>())
		{
			SCRIPTING.ReflectComponent(component);
		}
		if constexpr (sizeof...(T1s) != 0)
			Reflect<T1s...>(scene);
	}
};

using ReflectAll = decltype(ReflectExistingStruct(AllComponentTypes()));

void ScriptingSystem::SwapDll()
{
	for (uint32_t hand : gcHandles)
	{
		mono_gchandle_free(hand);
	}
	gcHandles.clear();
	RegisterScriptWrappers();
	mComponents.clear();
	UnloadAppDomain();
	CreateAppDomain();
	mCoreAssembly = Utils::loadAssembly("scripts.dll");
	mAssemblyImage = mono_assembly_get_image(mCoreAssembly);
	mGameObject = mono_class_from_name(mAssemblyImage, "BeanFactory", "GameObject");
	mScript = mono_class_from_name(mAssemblyImage, "BeanFactory", "Script");
	mCollision2D = mono_class_from_name(mAssemblyImage, "BeanFactory", "Collision2D");
	mScriptableObject = mono_class_from_name(mAssemblyImage, "BeanFactory", "ScriptableObject");
	mComponent = mono_class_from_name(mAssemblyImage, "BeanFactory", "Component");
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

MonoObject* ScriptingSystem::GetFieldMonoObject(MonoClassField* mField, MonoObject* mObject)
{
	if (mAppDomain == nullptr)
	{
		PRINT("APP DOMAIN WAS NULL");
		return nullptr;
	}
	return mono_field_get_value_object(mAppDomain, mField, mObject);
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
	if (field.fType == FieldType::String)
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

void ScriptingSystem::SetFieldValue(MonoObject* instance, MonoClassField* mClassFiend, Field& field, const void* value)
{
	//THIS FUNCTION ONLY WORKS FOR BASIC TYPES
	field = value;
	//If its a string, its a C# string so create one
	//PRINT("Set field value: " << mono_field_get_name(mClassFiend));
	if (field.fType == FieldType::String)
	{
		MonoString* mono_string = CreateMonoString(reinterpret_cast<const char*>(value));
		mono_field_set_value(instance, mClassFiend, mono_string);
		return;
	}
	mono_field_set_value(instance, mClassFiend, (void*)value);
	return;
}

template<typename T>
void ScriptingSystem::SetFieldReference(MonoObject* instance, MonoClassField* mClassFiend, T* reference)
{
	//When you set a reference, you need to create a MonoObject of it first

	if (reference == nullptr)
	{
		mono_field_set_value(instance, mClassFiend, nullptr);
		return;
	}
	//ZACH: Trying to set a component reference
	if constexpr (AllComponentTypes::Has<T>())
	{
		mono_field_set_value(instance, mClassFiend, ReflectComponent(*reference));
	}
	//ZACH: Trying to set a gameobject reference
	else if  constexpr (std::is_same<T,Entity>())
	{
		//mono_field_set_value(instance, mClassFiend, ReflectEntity(*reference));
	}
}


void ScriptingSystem::InvokeMethod(Script& script, const std::string& method)
{
	//MonoObject* mNewScript = ReflectComponent(script);
	//PRINT("Script Invoking " << pEvent->script.Name() << " " << pEvent->methodName << " ,ID: " << pEvent->script.uuid);
	//E_ASSERT(mNewScript, std::string("MONO OBJECT OF ") + script.name + std::string(" NOT LOADED"));
	ScriptClass& scriptClass{ scriptClassMap[script.name] };
	MonoMethod* mMethod{ mono_class_get_method_from_name (scriptClass.mClass,method.c_str(),0)};
	if (!mMethod && mono_class_get_parent(scriptClass.mClass) == mScript)
	{
		mMethod = mono_class_get_method_from_name(mScript, method.c_str(), 0);
		if (!mMethod)
			return;
	}
	//E_ASSERT(mMethod, std::string("MONO METHOD ") + method + std::string(" IN SCRIPT ") + script.name + std::string(" NOT FOUND"));
	invoke((MonoObject*)(&script), mMethod, nullptr);
}

void ScriptingSystem::CallbackScriptModified(FileTypeModifiedEvent<FileType::SCRIPT>* pEvent)
{
	timeUntilRecompile = SECONDS_TO_RECOMPILE;
}

template <typename T>
MonoObject* ScriptingSystem::ReflectComponent(T& component)
{
	auto pairIt = mComponents.find(&component);
	if (pairIt == mComponents.end())
	{
		Scene& scene = MySceneManager.GetCurrentScene();
		size_t address = (size_t)&component;
		if (scriptClassMap.find(GetComponentType::Name<T>()) == scriptClassMap.end())
		{
			return nullptr;
		}
		ScriptClass& scriptClass = scriptClassMap[GetComponentType::Name<T>()];
		MonoObject* instance = InstantiateClass(scriptClass.mClass);
		if constexpr (std::is_same<T, Transform>())
		{
			void* params[2] = { &scene.GetEntity(component),&address};
			//E_ASSERT(mComponent, "MonoObject was null");
			//invoke(instance, reflectComponent, params);
			mComponents.emplace(&component, instance);
		}
		return instance;
	}
	return (*pairIt).second;
}

bool ScriptingSystem::IsScript(MonoClass* monoClass)
{
	if (mono_class_is_subclass_of(monoClass, mScript, false))
		return true;
	return false;
}
//
template <>
MonoObject* ScriptingSystem::ReflectComponent(Script& component)
{
	auto pairIt = mComponents.find(&component);
	if (pairIt == mComponents.end())
	{
		if (scriptClassMap.find(component.name) == scriptClassMap.end())
			return nullptr;
		size_t address = (size_t)&component;
		ScriptClass& scriptClass = scriptClassMap[component.name];
		Scene& scene = MySceneManager.Instance().GetCurrentScene();
		MonoObject* instance = InstantiateClass(scriptClass.mClass);
		void* params[2] = { &scene.GetEntity(component),&address };
		//PRINT("C++ COMPONENT: " << (size_t)&component);
		E_ASSERT(mComponent, "MonoObject was null");
		mComponents.emplace(&component, instance);
		//Check fields, dont remove fields, but change them if their type is different

		std::list<std::string> validFieldNames{};

		//for (auto& pair : component.fieldDataReferences)
		//{
		//	validFieldNames.push_back(pair.first);
		//}

		////PRINT("Creating: " << component.Name() << " of id: " << component.uuid);
		//Script& script{ *reinterpret_cast<Script*>(&component) };
		//for (auto& pair : scriptClass.mFields)
		//{
		//	MonoClassField* mField = pair.second;
		//	MonoType* type = mono_field_get_type(mField);
		//	FieldType fieldType = Utils::monoTypeToFieldType(type);
		//	const char* fieldName = pair.first.c_str();
		//	std::string typeName = mono_type_get_name(type);
		//	//PRINT(typeName << " " << (int)fieldType);
		//	auto nameField{ script.fieldDataReferences.find(fieldName) };
		//	int alignment{};
		//	int fieldSize = mono_type_size(type, &alignment);
		//	//POINTER
		//	if (fieldType == FieldType::Component || fieldType == FieldType::GameObject)
		//	{
		//		fieldSize = sizeof(uint64_t);
		//	}
		//	else if(fieldType == FieldType::String)
		//	{
		//		fieldSize = TEXT_BUFFER_SIZE;
		//	}


		//	//Field has not been created onto script yet
		//	if (nameField == script.fieldDataReferences.end())
		//	{
		//		Field newField = Field(fieldType, fieldSize, nullptr);
		//		size_t offset = typeName.find_last_of(".");
		//		if (offset != std::string::npos)
		//			newField.typeName = typeName.substr(offset + 1);
		//		else
		//			newField.typeName = typeName;
		//		if (fieldType == FieldType::Component || fieldType == FieldType::GameObject)
		//		{
		//			newField = std::numeric_limits<uint64_t>::max();
		//		}
		//		else
		//		{
		//			MyEventSystem->publish(new ScriptGetFieldEvent(script, fieldName, newField.data));
		//		}
		//		script.fieldDataReferences[fieldName] = std::move(newField);
		//	}
			//Field exists
			//else
			//{
			//	validFieldNames.remove(fieldName);
			//	Field& field = nameField->second;
			//	//If the field type is not the same
			//	if (field.fType != fieldType)
			//	{
			//		field.Resize(fieldSize);
			//		mono_field_get_value(instance, mField, field.data);
			//	}
			//	//Field exists, setback the values first
			//	else
			//	{
			//			
			//		if (field.fType == FieldType::GameObject)
			//		{
			//			GameObject* reference = script.fieldGameObjReferences[fieldName];
			//			MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, reference));
			//		}
			//		else if (field.fType == FieldType::Component)
			//		{
			//			Component* reference = script.fieldComponentReferences[fieldName];
			//			switch ((ComponentType)field.fType)
			//			{
			//			case(ComponentType::Animator):
			//			{
			//				MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Animator*)reference));
			//				break;
			//			}
			//			case(ComponentType::AudioSource):
			//			{
			//				MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (AudioSource*)reference));
			//				break;
			//			}
			//			case(ComponentType::BoxCollider2D):
			//			{
			//				MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (BoxCollider2D*)reference));
			//				break;
			//			}
			//			case(ComponentType::Button):
			//			{
			//				MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Button*)reference));
			//				break;
			//			}
			//			case(ComponentType::Camera):
			//			{
			//				MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Camera*)reference));
			//				break;
			//			}
			//			case(ComponentType::Image):
			//			{
			//				MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Image*)reference));
			//				break;
			//			}
			//			case(ComponentType::Rigidbody2D):
			//			{
			//				MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Rigidbody2D*)reference));
			//				break;
			//			}
			//			case(ComponentType::SpriteRenderer):
			//			{
			//				MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (SpriteRenderer*)reference));
			//				break;
			//			}
			//			case(ComponentType::Script):
			//			{
			//				MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Script*)reference));
			//				break;
			//			}
			//			case(ComponentType::Text):
			//			{
			//				MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (Text*)reference));
			//				break;
			//			}
			//			case(ComponentType::SortingGroup):
			//			{
			//				MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, fieldName, (SortingGroup*)reference));
			//				break;
			//			}
			//			}
			//		}
			//		else
			//		{
			//			SetFieldValue(instance, mField, field,field.data);
			//		}
			//	}
			//	//CHECK TYPENAME
			//	//else if (field.typeName)
			//}
		//}
		//for (auto& name : validFieldNames)
		//{
		//	FieldType fType = script.fieldDataReferences[name].fType;
		//	if (fType == FieldType::GameObject)
		//	{
		//		component.fieldGameObjReferences.erase(name);
		//	}
		//	else if (fType >= FieldType::Component)
		//	{
		//		component.fieldComponentReferences.erase(name);
		//	}
		//	component.fieldDataReferences.erase(name);
		//	//PRINT("INVALID FIELD: " << name);
		//}
		return instance;
	}
	return pairIt->second;
}



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