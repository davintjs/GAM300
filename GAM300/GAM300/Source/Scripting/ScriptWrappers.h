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
#include "Graphics/Animation/BaseAnimator.h"

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
	static void Get(ScriptObject<Object> pEntity, MonoReflectionType* componentType, void*& obj)
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
				obj = nullptr;
				return;
			}
			else
			{
				//Cant find
				//CONSOLE_ERROR(mono_type_get_name(mType), "is not a valid component!");
				obj = nullptr;
				return;
			}
		}
		if (pair->second == GetType::E<Entity>())
		{
			PRINT("Getting entity\n");
		}
		Object* pObject = MySceneManager.GetCurrentScene().Get(pair->second, (Object*)pEntity);
		ScriptObject<Object> object{ pObject };
		obj = &object;
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

#pragma region ANIMATOR
	static void PlayAnimation(ScriptObject<Animator> pAnimator)
	{
		Scene& currentScene = MySceneManager.GetCurrentScene();
		for (auto& animator : currentScene.GetArray<Animator>())
		{
			if (animator.EUID() == (*pAnimator).EUID())
			{
				animator.ChangeState();
				break;
			}
		}
	}

	static void PauseAnimation(ScriptObject<Animator> pAnimator)
	{
		Scene& currentScene = MySceneManager.GetCurrentScene();
		for (auto& animator : currentScene.GetArray<Animator>())
		{
			if (animator.EUID() == (*pAnimator).EUID())
			{
				animator.playing = false;
				break;
			}
		}
	}

	static void StopAnimation(ScriptObject<Animator> pAnimator)
	{
		Scene& currentScene = MySceneManager.GetCurrentScene();
		for (auto& animator : currentScene.GetArray<Animator>())
		{
			if (animator.EUID() == (*pAnimator).EUID())
			{
				animator.m_CurrentTime = 0.f;
				animator.playing = false;
				break;
			}
		}
	}

	static float GetProgress(ScriptObject<Animator> pAnimator)
	{
		Scene& currentScene = MySceneManager.GetCurrentScene();
		for (auto& animator : currentScene.GetArray<Animator>())
		{
			if (animator.EUID() == (*pAnimator).EUID())
				return animator.GetProgress();
		}

		return 0.f;
	}

	static bool IsCurrentState(ScriptObject<Animator> pAnimator, MonoString* mString)
	{
		std::string state = mono_string_to_utf8(mString);
		Scene& currentScene = MySceneManager.GetCurrentScene();
		for (auto& animator : currentScene.GetArray<Animator>())
		{
			if (animator.EUID() == (*pAnimator).EUID())
			{
				if(animator.GetCurrentState())
					return !animator.GetCurrentState()->label.compare(state);
			}
		}

		return false;
	}
	
	static void SetDefaultState(ScriptObject<Animator> pAnimator, MonoString* mString)
	{
		std::string defaultState = mono_string_to_utf8(mString);
		Scene& currentScene = MySceneManager.GetCurrentScene();
		for (auto& animator : currentScene.GetArray<Animator>())
		{
			if (animator.EUID() == (*pAnimator).EUID())
			{
				animator.SetDefaultState(defaultState);
				break;
			}
		}
	}

	static void SetState(ScriptObject<Animator> pAnimator, MonoString* mString)
	{
		std::string state = mono_string_to_utf8(mString);
		Scene& currentScene = MySceneManager.GetCurrentScene();
		for (auto& animator : currentScene.GetArray<Animator>())
		{
			if (animator.EUID() == (*pAnimator).EUID())
			{
				animator.SetState(state);
				break;
			}
		}
	}

	static void SetNextState(ScriptObject<Animator> pAnimator, MonoString* mString)
	{
		std::string nextState = mono_string_to_utf8(mString);
		Scene& currentScene = MySceneManager.GetCurrentScene();
		for (auto& animator : currentScene.GetArray<Animator>())
		{
			if (animator.EUID() == (*pAnimator).EUID())
			{
				animator.SetNextState(nextState);
				break;
			}
		}
	}
#pragma endregion
	
	static void SetTransformParent(ScriptObject<Transform> pTransform, ScriptObject<Transform> pParent)
	{
		Scene& currentScene = MySceneManager.GetCurrentScene();

		Transform *transform, *parent = nullptr;
		for (auto& t : currentScene.GetArray<Transform>())
		{
			if (t.EUID() == (*pTransform).EUID())
				transform = &t;

			if (t.EUID() == (*pParent).EUID())
				parent = &t;
		}

		// If the parent doesnt exist, set the parent of this transform to null
		if(transform)
			transform->SetParent(parent);
	}

	// Load a scene
	static void LoadScene(MonoString* mString)
	{
		// Bean: Not really elegant because we can only load scenes from the scene folder
		std::string scenePath = "Assets/Scene/";
		scenePath += mono_string_to_utf8(mString);
		scenePath += ".scene";
		
		LoadSceneEvent e(scenePath);
		EVENTS.Publish(&e);
	}

	//Gets object that entity has
	static void* AddComponent(ScriptObject<Object> pEntity, MonoReflectionType* componentType)
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
	static void HasComponent(ScriptObject<Entity> pEntity, MonoReflectionType* componentType, bool& output)
	{
		
		MonoType* managedType = mono_reflection_type_get_type(componentType);
		if (monoComponentToType.find(managedType) != monoComponentToType.end())
		{
			Object* entity(pEntity);
			if (!entity)
			{
				PRINT("Has component when gameobject is null!\n");
				output = false;
				return;
			}
			output = ((Entity&)pEntity).hasComponentsBitset.test(monoComponentToType[managedType]);
			return;
		}
		PRINT(mono_type_get_name(managedType), "is invalid", '\n');
		output = false;
	}

	static void CloneGameObject(ScriptObject<Entity> pEntity, ScriptObject<Entity> out)
	{
		Object* obj{ pEntity };
		out = &MySceneManager.GetCurrentScene().Clone((Entity&)pEntity);
	}


	//Deletes a gameobject
	static void DestroyGameObject(ScriptObject<Entity> pEntity)
	{
		PRINT("DESTROYING GAME OBJECT\n");
		MySceneManager.GetCurrentScene().Destroy<Entity>(pEntity);
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
		Register(SetTransformParent);
		Register(LoadScene);
		Register(AddComponent);
		Register(GetMouseDelta);
		Register(AudioSourcePlay);
		Register(CloneGameObject);
		Register(PlayAnimation);
		Register(PauseAnimation);
		Register(StopAnimation);
		Register(GetProgress);
		Register(IsCurrentState);
		Register(SetDefaultState);
		Register(SetState);
		Register(SetNextState);
		Register(GetTag);
	}
#endif // !SCRIPT_WRAPPERS_H