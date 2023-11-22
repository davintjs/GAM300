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
#include "AI/NavMesh.h"
#include "AI/NavMeshBuilder.h"

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

	static int GetScrollState()
	{
		return InputHandler::getMouseScrollState();
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
	static void Get(ScriptObject<Object> pEntity, MonoReflectionType* componentType, ScriptObject<Object>& obj)
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
		Object* pObject = MySceneManager.GetCurrentScene().Get(pair->second, (Object*)pEntity);
		obj = pObject;
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
		Animator& animator = pAnimator;
		animator.ChangeState();
	}

	static void PauseAnimation(ScriptObject<Animator> pAnimator)
	{
		Animator& animator = pAnimator;
		animator.playing = false;
	}

	static void StopAnimation(ScriptObject<Animator> pAnimator)
	{
		Animator& animator = pAnimator;
		animator.m_CurrentTime = 0.f;
		animator.playing = false;
	}

	static float GetProgress(ScriptObject<Animator> pAnimator)
	{
		Animator& animator = pAnimator;
		return animator.GetProgress();
	}

	static void SetProgress(ScriptObject<Animator> pAnimator, float value)
	{
		Animator& animator = pAnimator;
		animator.SetProgress(value);
	}
	
	static void SetDefaultState(ScriptObject<Animator> pAnimator, MonoString* mString)
	{
		Animator& animator = pAnimator;
		animator.SetDefaultState(mono_string_to_utf8(mString));
	}

	static void SetState(ScriptObject<Animator> pAnimator, MonoString* mString)
	{
		Animator& animator = pAnimator;
		animator.SetState(mono_string_to_utf8(mString));
	}

	static void SetNextState(ScriptObject<Animator> pAnimator, MonoString* mString)
	{
		Animator& animator = pAnimator;
		animator.SetNextState(mono_string_to_utf8(mString));
	}

	static MonoString* GetState(ScriptObject<Animator> pAnimator)
	{
		Animator& animator = pAnimator;
		return SCRIPTING.CreateMonoString(animator.GetCurrentState()->label);
	}

#pragma endregion
	
#pragma region TRANSFORM
	static void SetTransformParent(ScriptObject<Transform> pTransform, ScriptObject<Transform> pParent)
	{
		Transform& transform = pTransform;
		Transform& parent = pParent;
		Object* obj = pParent;

		// If the parent doesnt exist, set the parent of this transform to null
		if (obj)
			transform.SetParent(&parent);
		else
			transform.SetParent(nullptr);
	}

	static void GetPosition(ScriptObject<Transform> pTransform, Vector3& position)
	{
		Transform& t = pTransform;
		position = t.GetTranslation();
	}

	static void GetRotation(ScriptObject<Transform> pTransform, Vector3& rotation)
	{
		Transform& t = pTransform;
		rotation = t.GetRotation();
	}

	static void GetScale(ScriptObject<Transform> pTransform, Vector3& scale)
	{
		Transform& t = pTransform;
		scale = t.GetScale();
	}
#pragma endregion

#pragma region PARTICLES

	static void ParticlesPlayer(ScriptObject<ParticleComponent> particleComp)
	{
		ParticleComponent& p = particleComp;
		p.particleLooping = (p.particleLooping ? false : true);
	}

#pragma endregion

	

	// Load a scene
	static void LoadScene(MonoString* mString)
	{
		// Bean: Not really elegant because we can only load scenes from the scene folder
		std::string scenePath = "Assets/Scene/";
		scenePath += mono_string_to_utf8(mString);
		scenePath += ".scene";

		MySceneManager.sceneToLoad = scenePath;
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
				return MySceneManager.GetCurrentScene().Add<Script>((Entity&)pEntity,nullptr,scriptName.c_str());
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

	static void CloneGameObject(ScriptObject<Entity> pEntity, ScriptObject<Entity>& out)
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

	// Pathfinding
	static bool FindPath(ScriptObject<NavMeshAgent> pEnemy, glm::vec3 pDest)
	{
		NavMeshAgent& _player = pEnemy;

		std::cout << "Destination : " << pDest.x << " " << pDest.y << " " << pDest.z << std::endl;
		return NAVMESHBUILDER.GetNavMesh()->FindPath(_player, pDest);
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
		Register(LoadScene);
		Register(AddComponent);
		Register(GetMouseDelta);
		Register(CloneGameObject);

		// Transform Component
		Register(SetTransformParent);
		Register(GetPosition);
		Register(GetRotation);
		Register(GetScale);

		// Audio Component
		Register(AudioSourcePlay);

		// Animator Component
		Register(PlayAnimation);
		Register(PauseAnimation);
		Register(StopAnimation);
		Register(GetProgress);
		Register(SetProgress);
		Register(SetDefaultState);
		Register(SetState);
		Register(SetNextState);
		Register(GetState);

		// Particle Component
		Register(ParticlesPlayer);

		// Tag Component
		Register(GetTag);
		Register(FindPath);
		Register(GetScrollState);
	}
#endif // !SCRIPT_WRAPPERS_H