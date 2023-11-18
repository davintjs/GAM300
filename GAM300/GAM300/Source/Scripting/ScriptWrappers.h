/*!***************************************************************************************
\file			ScriptWrappers.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			10/09/2023

\brief
	This file helps register static functions be used as internal calls in C#

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/


#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"
#include "IOManager/InputHandler.h"
#include "Scene/SceneManager.h"
#include "ScriptingSystem.h"
#include "Scene/Identifiers.h"
#include "Audio/AudioManager.h"

#ifndef SCRIPT_WRAPPERS_H
#define SCRIPT_WRAPPERS_H

	static std::unordered_map<MonoType*, size_t> monoComponentToType;

	#define Register(METHOD) mono_add_internal_call("BeanFactory.InternalCalls::"#METHOD,METHOD)

	//DOESNT WORK YET, Checks if key was released
	static bool GetKeyUp(int keyCode)
	{
		return InputHandler::isKeyButtonPressed(keyCode);
	}

	//Checks if key was pressed
	static bool GetKeyDown(int keyCode)
	{
		return InputHandler::isKeyButtonPressed(keyCode);
	}


	static bool GetMouseDown(int mouseCode)
	{
		UNREFERENCED_PARAMETER(mouseCode);
		return InputHandler::isMouseButtonPressed_L();
	}

	//Checks if key is held
	static bool GetKey(int keyCode)
	{
		return InputHandler::isKeyButtonHolding(keyCode);
	}

	//Gets object that entity has
	static void* Get(ScriptObject<Entity> pEntity, MonoReflectionType* componentType)
	{
		Object* entityMaybe = pEntity;
		MonoType* mType = mono_reflection_type_get_type(componentType);
		auto pair = monoComponentToType.find(mType);
		if (pair == monoComponentToType.end())
		{
			if (SCRIPTING.IsScript(mono_class_from_mono_type(mType)))
			{
				//Script
				E_ASSERT(false,"Getting scripts not implemented yet!");
				return nullptr;
			}
			else
			{
				//Cant find
				//CONSOLE_ERROR(mono_type_get_name(mType), "is not a valid component!");
				return nullptr;
			}
		}
		return ScriptObject<Object>((MySceneManager.GetCurrentScene().Get(pair->second, (Object*)pEntity)));
	}

	static MonoString* GetTag(ScriptObject<Object> pObject)
	{
		Object* object = pObject;
		Tag& tag = MySceneManager.GetCurrentScene().Get<Tag>(object->EUID());
		return SCRIPTING.CreateMonoString(IDENTIFIERS.GetTagString(tag.tagName));
	}

	static void AudioSourcePlay(ScriptObject<AudioSource> audioSource)
	{
		AUDIOMANAGER.PlayComponent(audioSource);
	}

	//Gets object that entity has
	static void* AddComponent(ScriptObject<Entity> pEntity, MonoReflectionType* componentType)
	{
		MonoType* mType = mono_reflection_type_get_type(componentType);
		auto pair = monoComponentToType.find(mType);

		if (pair == monoComponentToType.end())
		{
			if (SCRIPTING.IsScript(mono_class_from_mono_type(mType)))
			{
				//Script
				std::string scriptName = mono_type_get_name(mType);
				size_t offset = scriptName.find_last_of(".");
				if (offset != std::string::npos)
					scriptName = scriptName.substr(offset + 1);
				//Get Mono Script instead
				return MySceneManager.GetCurrentScene().Add<Script>((Entity&)pEntity,scriptName.c_str());
			}
			else
			{
				//Cant find
				//CONSOLE_ERROR(mono_type_get_name(mType), "is not a valid component!");
				return nullptr;
			}
		}
		return ScriptObject<Object>((Object*)MySceneManager.GetCurrentScene().Add(pair->second, pEntity));
	}


	//Checks if entity has a component
	static bool HasComponent(Entity* pEntity, MonoReflectionType* componentType)
	{
		MonoType* managedType = mono_reflection_type_get_type(componentType);
		return pEntity->hasComponentsBitset.test(monoComponentToType[managedType]);
	}


	//Deletes a gameobject
	static void DestroyGameObject(Entity* pGameObject)
	{
		MySceneManager.GetCurrentScene().Destroy(*pGameObject);
	}

	//GENERIC_RECURSIVE(void, DestroyRecursive, MySceneManager.GetCurrentScene().Destroy(*(T*)pObject))
	//static void DestroyComponent(void* pComponent, MonoReflectionType* componentType)
	//{
	//	MonoType* managedType = mono_reflection_type_get_type(componentType);
	//	DestroyRecursive(monoComponentToType[managedType],pComponent);
	//}

	static MonoString* GetLayerName(int layer)
	{
		E_ASSERT(layer < MAX_PHYSICS_LAYERS, "Exceeded max physics layers");
		std::string& name = IDENTIFIERS.physicsLayers[layer].name;
		if (name.size() == 0)
		{
			CONSOLE_WARN("Physics Layer name is unassigned and is being used");
		}
		return SCRIPTING.CreateMonoString(name);
	}

	static int GetLayer(MonoString* mString)
	{
		std::string name = mono_string_to_utf8(mString);
		int i = 0;
		for (Layer& layer  : IDENTIFIERS.physicsLayers)
		{
			if (name == layer.name)
			{
				return i;
			}
			++i;
		}
		CONSOLE_WARN("Physics Layer ", name ,"does not exist");
		return -1;
	}

	static bool GetActive(ScriptObject<Object> object, MonoReflectionType* componentType)
	{
		MonoType* mType = mono_reflection_type_get_type(componentType);
		auto pair = monoComponentToType.find(mType);
		Scene& scene = MySceneManager.GetCurrentScene();
		if (pair == monoComponentToType.end())
		{
			if (SCRIPTING.IsScript(mono_class_from_mono_type(mType)))
			{
				ScriptObject<Script> script(object);
				return scene.IsActive<Script>(script);
			}
			else
			{
				//std::string name = mono_type_get_name(mType);
				//CONSOLE_ERROR(name, "is not a valid component!");
				return false;
			}
		}
		return scene.GetActive(pair->second, object);
	}

	static void GetMouseDelta(Vector2& mouseDelta)
	{
		mouseDelta = InputHandler::mouseDeltaNormalized();
	}

	static void SetActive(ScriptObject<Object> pObject, MonoReflectionType* componentType, bool val)
	{
		MonoType* mType = mono_reflection_type_get_type(componentType);
		auto pair = monoComponentToType.find(mType);
		Scene& scene = MySceneManager.GetCurrentScene();
		if (pair == monoComponentToType.end())
		{
			if (SCRIPTING.IsScript(mono_class_from_mono_type(mType)))
			{
				ScriptObject<Script> script(pObject);
				scene.SetActive((Script&)script, val);
				return;
			}
			else
			{
				//CONSOLE_ERROR(mono_type_get_name(mType), "is not a valid component!");
				return;
			}
		}
		Scene::SetActiveHelper helper{ pObject,val };
		return scene.SetActive(pair->second, &helper);
	}

	//Register all components to mono
	template<typename T,typename... Ts>
	static void RegisterComponent()
	{
		std::string typeName = "BeanFactory.";
		if constexpr (std::is_same_v<Entity, T>)
		{
			typeName += "GameObject";
		}
		else
		{
			typeName += GetType::Name<T>();
		}
		MonoType* managedType = mono_reflection_type_from_name(typeName.data(), SCRIPTING.GetAssemblyImage());
		if (managedType != nullptr)
		{
			monoComponentToType.emplace(managedType, GetType::E<T>());
		}
		if constexpr (sizeof...(Ts) != 0)
		{
			return RegisterComponent<Ts...>();
		}
	}

	//Register all components to mono
	template<typename... T>
	static void RegisterComponent(TemplatePack<T...>)
	{
		RegisterComponent<T...>();
	}

	//Register all components to mono
	static void RegisterComponents()
	{
		monoComponentToType.clear();
		RegisterComponent(AllObjectTypes());
	}

	//Registers all defined internal calls with mono
	static void RegisterScriptWrappers()
	{
		Register(GetKey);
		Register(GetKeyUp);
		Register(GetKeyDown);
		Register(GetMouseDown);
		Register(DestroyGameObject);
		Register(HasComponent);
		Register(Get);
		Register(GetLayer);
		Register(GetLayerName);
		Register(GetActive);
		Register(SetActive);
		Register(AddComponent);
		Register(GetMouseDelta);
		Register(AudioSourcePlay);
		Register(GetTag);
	}
#endif // !SCRIPT_WRAPPERS_H