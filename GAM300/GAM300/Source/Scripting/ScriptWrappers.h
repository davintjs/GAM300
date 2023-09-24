﻿/*!***************************************************************************************
\file			script-wrappers.h
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			10/03/2022

\brief
	This file helps register static functions be used as internal calls in C#

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/


#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"
#include "IOManager/InputHandler.h"
#include "Scene/SceneManager.h"
#include "ScriptingSystem.h"


#ifndef SCRIPT_WRAPPERS_H
#define SCRIPT_WRAPPERS_H

	static std::unordered_map<MonoType*, size_t> monoComponentToType;


	#define Register(METHOD) mono_add_internal_call("BeanFactory.InternalCalls::"#METHOD,METHOD)
	/*******************************************************************************
	/*!
	\brief
		Set window fullscreen mode
	\param _fullscreen
		If true, is fullscreen else false
	*/
	/*******************************************************************************/
	//static void SetFullscreenMode(bool _fullscreen)
	//{
	//	MyWindowSystem.Fullscreen(_fullscreen, 1600, 900);
	//}

	/*******************************************************************************
	/*!
	\brief
		To be implemented

	\param keyCode

	\return
	*/
	/*******************************************************************************/
	static bool GetKeyUp(int keyCode)
	{
		return InputHandler::isKeyButtonPressed(keyCode);
	}
	/*******************************************************************************
	/*!
	\brief
		To be implemented

	\param keyCode

	\return

	*/
	/*******************************************************************************/
	static bool GetKeyDown(int keyCode)
	{
		return InputHandler::isKeyButtonPressed(keyCode);
	}

	/*******************************************************************************
	/*!
	\brief
		To be implemented

	\param keyCode

	\return

	*/
	/*******************************************************************************/
	static bool GetMouseDown(int mouseCode)
	{
		return InputHandler::isMouseButtonPressed_L();
	}

	/*******************************************************************************
	/*!
	\brief
		To be implemented

	\param keyCode

	\return

	*/
	/*******************************************************************************/
	//static void GetMousePosition(Copium::Math::Vec2* pos)
	//{
	//	*pos = inputSystem.get_mouseposition();
	//}

	/*******************************************************************************
	/*!
	\brief
		Checks if a key was held
	\param keyCode
		Keycode to listen to
	\return
		True if key held
	*/
	/*******************************************************************************/
	static bool GetKey(int keyCode)
	{
		return InputHandler::isKeyButtonHolding(keyCode);
	}

	///*******************************************************************************
	///*!
	//\brief
	//	Gets the position of a GameObject via its id
	//\param _ID
	//	_ID of gameObject to look for
	//\param[out] translation
	//	Stores the value of translation of the queried gameObj
	//*/
	///*******************************************************************************/
	//static void GetTranslation(GameObject* pGameObject, Math::Vec3* translation)
	//{
	//	*translation = pGameObject->transform.position;
	//}

	///*******************************************************************************
	///*!
	//\brief
	//	Sets the position of a GameObject via its id
	//\param _ID
	//	_ID of gameObject to look for
	//\param val
	//	Value of Vec3 to set
	//*/
	///*******************************************************************************/
	//static void SetTranslation(GameObject* pGameObject, Math::Vec3* val)
	//{
	//	pGameObject->transform.position = *val;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Adds force to a gameobject with a rigidbody component
	//	\param _ID
	//		ID of gameObject to look for
	//	\param force
	//		Force to add
	//*/
	///*******************************************************************************/
	//static void RigidbodyAddForce(Rigidbody2D* pRb, Math::Vec2* force)
	//{
	//	pRb->force += *force;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Get for C#
	//	\param gameObjID
	//		ID of gameObject to look for component
	//	\param componentType
	//		Mono type of the gameObject to get
	//	\return
	//		MonoObject to be returned to the script asking for it
	//*/
	///*******************************************************************************/v
	static void* Get(Object* pEntity, MonoReflectionType* componentType)
	{
		MonoType* mType = mono_reflection_type_get_type(componentType);
		auto pair = monoComponentToType.find(mType);
		if (pair == monoComponentToType.end())
		{
			PRINT("CANT FIND LAH CHIBAI\n");
		}
		size_t addr = reinterpret_cast<size_t>(MySceneManager.GetCurrentScene().Get(pair->second, pEntity));
		//addr += sizeof(Object);
		return reinterpret_cast<void*>(addr);
	}
	//	Component* component{ nullptr };
	//	switch (cType)
	//	{
	//	case(ComponentType::Animator):
	//	{
	//		component = pGameObject->Get<Animator>();
	//		break;
	//	}
	//	case(ComponentType::AudioSource):
	//	{
	//		component = pGameObject->Get<AudioSource>();
	//		break;
	//	}
	//	case(ComponentType::BoxCollider2D):
	//	{
	//		component = pGameObject->Get<BoxCollider2D>();
	//		break;
	//	}
	//	case(ComponentType::Button):
	//	{
	//		component = pGameObject->Get<Button>();
	//		break;
	//	}
	//	case(ComponentType::Camera):
	//	{
	//		component = pGameObject->Get<Camera>();
	//		break;
	//	}
	//	case(ComponentType::Image):
	//	{
	//		component = pGameObject->Get<Image>();
	//		break;
	//	}
	//	case(ComponentType::Rigidbody2D):
	//	{
	//		component = pGameObject->Get<Rigidbody2D>();
	//		break;
	//	}
	//	case(ComponentType::SpriteRenderer):
	//	{
	//		component = pGameObject->Get<SpriteRenderer>();
	//		break;
	//	}
	//	case(ComponentType::Script):
	//	{
	//		//Different scripts
	//		component = pGameObject->Get<Script>();
	//		break;
	//	}
	//	case(ComponentType::Text):
	//	{
	//		component = pGameObject->Get<Text>();
	//		break;
	//	}
	//	case(ComponentType::SortingGroup):
	//	{
	//		component = pGameObject->Get<SortingGroup>();
	//		break;
	//	}
	//	}
	//	if (component)
	//	{
	//		return scriptingSystem.mComponents[component->uuid];
	//	}
	//	return nullptr;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Sets the parent to a child by uuid
	//	\param newParentID
	//		UUID of parent gameobject
	//	\param childID
	//		UUID of child gameobject
	//*/
	///*******************************************************************************/
	//static void SetParent(GameObject* parent, GameObject* child)
	//{
	//	if (parent)
	//		child->transform.SetParent(&parent->transform);
	//	else
	//		child->transform.SetParent(nullptr);
	//}

	///*******************************************************************************
	///*!
	//\brief
	//	Sets the velocity of a rigidbody
	//\param _ID
	//	GameObject of ID with a rigidbody
	//\param velocity
	//	Velocity to set rigidbody to
	//*/
	///*******************************************************************************/
	//static void RigidbodySetVelocity(Rigidbody2D* pRb, Math::Vec2* velocity)
	//{
	//	pRb->velocity=*velocity;
	//}

	///*******************************************************************************
	///*!
	//\brief
	//	Gets the velocity of a rigidbody
	//\param _ID
	//	GameObject of ID with a rigidbody
	//\param velocity
	//	Velocity to store rigidbody's velocity
	//*/
	///*******************************************************************************/
	//static void RigidbodyGetVelocity(Rigidbody2D* pRb, Math::Vec2* velocity)
	//{
	//	*velocity = pRb->velocity;
	//}

	///*******************************************************************************
	///*!
	//\brief
	//	Checks if a gameobject of given ID has a component
	//\param _ID
	//	GameObject of ID to check for component
	//\param componentType
	//	Type of component
	//\return 
	//	True if gameobject of ID has component of type
	//*/
	///*******************************************************************************/
	static bool HasComponent(Entity* pEntity, MonoReflectionType* componentType)
	{
		MonoType* managedType = mono_reflection_type_get_type(componentType);
		return pEntity->hasComponentsBitset.test(monoComponentToType[managedType]);
	}

	////To be implemented
	//static UUID Add(UUID UUID, MonoReflectionType* componentType)
	//{
	//	//GameObject* gameObj = sceneManager.FindGameObjectByID(UUID);
	//	//if (gameObj == nullptr)
	//	//{
	//	//	PRINT("CANT FIND GAMEOBJECT");
	//	//	return false;
	//	//}
	//	//MonoType* managedType = mono_reflection_type_get_type(componentType);
	//	//ComponentType cType = s_EntityHasComponentFuncs[mono_type_get_name(managedType)];
	//	//return gameObj->Add(cType)->id;
	//	return 0;
	//}
	//
	///*******************************************************************************
	///*!
	//\brief
	//	Gets local scale of a gameObject
	//\param _ID
	//	GameObject of ID to get scale of
	//\param scale
	//	Scale pointer to retrieve values
	//*/
	///*******************************************************************************/
	//static void GetLocalScale(GameObject* pGameObj, Math::Vec3* scale)
	//{
	//	*scale = pGameObj->transform.scale;
	//}

	//static void GetGlobalScale(GameObject* pGameObj, Math::Vec3* scale)
	//{
	//	*scale = pGameObj->transform.GetWorldScale();
	//}

	//static void GetGlobalPosition(GameObject* pGameObj, Math::Vec3* scale)
	//{
	//	*scale = pGameObj->transform.GetWorldPosition();
	//}

	//static void GetGameNDC(glm::vec2* pos)
	//{
	//	*pos = sceneManager.mainCamera->get_game_ndc();
	//}


	///*******************************************************************************
	///*!
	//\brief
	//	Sets local scale of a gameObject
	//\param _ID
	//	GameObject of ID to set scale
	//\param scale
	//	Scale pointer to store values
	//*/
	///*******************************************************************************/
	//static void SetLocalScale(GameObject* pGameObj, Math::Vec3* scale)
	//{
	//	pGameObj->transform.scale = *scale;
	//}

	///*******************************************************************************
	///*!
	//\brief
	//	Gets local rotation of a gameObject
	//\param _ID
	//	GameObject of ID to get rotation
	//\param rotation
	//	Rotation pointer to retrieve values
	//*/
	///*******************************************************************************/
	//static void GetRotation(GameObject* pGameObj, Math::Vec3* rotation)
	//{
	//	pGameObj->transform.rotation = *rotation;
	//}

	///*******************************************************************************
	///*!
	//\brief
	//	Sets local rotation of a gameObject
	//\param _ID
	//	GameObject of ID to set rotation
	//\param rotation
	//	Rotation pointer to store values
	//*/
	///*******************************************************************************/
	//static void SetRotation(GameObject* pGameObj, Math::Vec3* rotation)
	//{
	//	pGameObj->transform.rotation = *rotation;
	//}

	///*******************************************************************************
	///*!
	//\brief
	//	Set active a gameObject
	//\param _ID
	//	GameObject of ID to set active
	//\param _active
	//	Bool to set active to
	//*/
	///*******************************************************************************/
	//static void SetActive(GameObject* pGameObj, bool _active)
	//{
	//	pGameObj->SetActive(_active);
	//}

	///*******************************************************************************
	///*!
	//\brief
	//	Get active a gameObject
	//\param _ID
	//	GameObject of ID to get active
	//\return 
	//	Bool to whether gameobject was active
	//*/
	///*******************************************************************************/
	//static bool GetActive(GameObject* pGameObj)
	//{
	//	return pGameObj->active;
	//}


	///*******************************************************************************
	///*!
	//\brief
	//	Check if component is enabled

	//\param cid
	//	id of the component to check

	//\param componentType
	//	Type of component

	//\return
	//	whether the component is enabled
	//*/
	///*******************************************************************************/
	//static bool GetEnabled(Component* pComponent)
	//{
	//	return pComponent->enabled;
	//}

	///*******************************************************************************
	///*!
	//\brief
	//	Set the component's enabled value

	//\param cid
	//	id of the component to set

	//\param val
	//	the value to set

	//\param componentType
	//	Type of component
	//*/
	///*******************************************************************************/
	//static void SetComponentEnabled(Component* pComponent, bool val)
	//{
	//	pComponent->enabled = val;
	//}


	///*******************************************************************************
	///*!
	//\brief
	//	Quits the game or preview mode depending on macros defined
	//*/
	///*******************************************************************************/
	//static void QuitGame()
	//{
	//	//if (sceneManager.endPreview())
	//	//	messageSystem.dispatch(MESSAGE_TYPE::MT_STOP_PREVIEW);
	//	quit_engine();
	//	#ifdef GAMEMODE
	//	quit_engine();
	//	#else
	//	
	//	#endif
	//	//Scene manager quit
	//}

	///*******************************************************************************
	///*!
	//\brief
	//	Gets the string of a text component
	//\param gameObjID
	//	ID of gameObject that has this component
	//\param compID
	//	Component ID of text component
	//\param str
	//	String of text component to store
	//*/
	///*******************************************************************************/
	//static void GetTextString(Text* pText, MonoString*& str)
	//{
	//	str = scriptingSystem.createMonoString(pText->content);
	//}

	///*******************************************************************************
	///*!
	//\brief
	//	Sets the string of a text component
	//\param gameObjID
	//	ID of gameObject that has this component
	//\param compID
	//	Component ID of text component
	//\param str
	//	String to set text component
	//*/
	///*******************************************************************************/
	//static void SetTextString(Text* pText, MonoString* str)
	//{
	//	char* monoStr = mono_string_to_utf8(str);
	//	strcpy(pText->content, monoStr);
	//	mono_free(monoStr);
	//}

	///*******************************************************************************
	///*!
	//\brief
	//	Clones a gameObject
	//\param ID
	//	ID of gameObject to be cloned
	//\return
	//	GameObject ID of the cloned gameObject
	//*/
	///*******************************************************************************/
	//static UUID CloneGameObject(GameObject* pGameObject)
	//{
	//	if (pGameObject)
	//	{
	//		//GameObject* clone = MyGOF.(*toBeCloned);
	//		//PRINT("CLONED OBJECT: " << clone->uuid);
	//		//if (clone)
	//		//	return clone->uuid;
	//	}
	//	return 0;
	//}

	///*******************************************************************************
	///*!
	//\brief
	//	Creates a blank gameObject
	//\return
	//	GameObject ID of the new gameObject
	//*/
	///*******************************************************************************/
	//static GameObject* InstantiateGameObject()
	//{
	//	Scene* scene = MySceneManager.get_current_scene();
	//	COPIUM_ASSERT(!scene, "SCENE NOT LOADED");
	//	GameObject& clone = MyGOF.Instantiate(*scene);
	//	return &clone;
	//}

	/*******************************************************************************
	/*!
	\brief
		Destroys a gameobject by ID
	\param ID
		GameObject ID of the gameObject to delete
	*/
	/*******************************************************************************/
	static void DestroyGameObject(Entity* pGameObject)
	{
		MySceneManager.GetCurrentScene().Destroy(*pGameObject);
	}

	GENERIC_RECURSIVE(void, DestroyRecursive, MySceneManager.GetCurrentScene().Destroy(*(T*)pObject))
	static void DestroyComponent(void* pComponent, MonoReflectionType* componentType)
	{
		MonoType* managedType = mono_reflection_type_get_type(componentType);
		DestroyRecursive(monoComponentToType[managedType],pComponent);
	}

	///*******************************************************************************
	///*!
	//\brief
	//	Loads a scene by name
	//\param str
	//	Name of scene(filename)
	//*/
	///*******************************************************************************/
	//static void LoadScene(MonoString* str)
	//{
	//	static std::string name{};
	//	char* monoStr = mono_string_to_utf8(str);
	//	name = monoStr;
	//	name += ".scene";
	//	namespace fs = std::filesystem;
	//	for (const fs::directory_entry& p : fs::recursive_directory_iterator(Paths::projectPath))
	//	{
	//		const fs::path& pathRef{ p.path() };
	//		if (pathRef.extension() != ".scene")
	//			continue;
	//		if (pathRef.filename().string() == name)
	//		{
	//			PRINT("LOADING " << pathRef.string());
	//			MyEditorSystem.sceneChangeName = pathRef.string();
	//			return;
	//		}
	//	}
	//	PRINT("NO SCENE WITH THE NAME COULD BE FOUND");
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Gets the fps
	//	\return
	//		FPS count
	//*/
	///*******************************************************************************/
	//static float GetFPS()
	//{
	//	float tmp{ 0.f };

	//	if (MyFrameRateController.getFPS() > std::numeric_limits<float>::max())
	//		tmp = std::numeric_limits<float>::max();
	//	else
	//		tmp = (float)MyFrameRateController.getFPS();

	//	return tmp;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Play sound

	//	\param ID
	//		id of the game object that contains an audio source component
	//*/
	///*******************************************************************************/
	//static void AudioSourcePlay(AudioSource* pAudioSource)
	//{
	//	pAudioSource->play_sound();
	//}

	//static unsigned GetSoundLength(AudioSource* pAudioSource)
	//{
	//	return soundSystem.GetSoundLength(pAudioSource->alias);
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Stops an audio source

	//	\param ID
	//		id of the game object that contains an audio source component
	//*/
	///*******************************************************************************/
	//static void AudioSourceStop(AudioSource* pAudioSource)
	//{
	//	pAudioSource->stop_sound();
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Sets all volume

	//	\param volume
	//		Target Volume
	//*/
	///*******************************************************************************/
	//static void SetAllVolume(float volume)
	//{
	//	soundSystem.SetAllVolume(volume);
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Mutes or Unmutes all volume

	//	\param mute
	//		Target bool
	//*/
	///*******************************************************************************/
	//static void AudioMute(bool mute)
	//{
	//	soundSystem.Mute(mute);
	//}


	///*******************************************************************************
	///*!
	//	\brief
	//		Sets volume of an audio source

	//	\param ID
	//		ID of audio source

	//	\param volume
	//		Volume to set
	//*/
	///*******************************************************************************/
	//static void AudioSourceSetVolume(AudioSource* pAudioSource, float volume)
	//{
	//	pAudioSource->volume = volume;
	//	SoundSystem::Instance()->soundList[pAudioSource->alias].first->setVolume(volume);
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Gets volume of an audio source

	//	\param ID
	//		ID of audio source

	//	\return 
	//		Volume of audio source
	//*/
	///*******************************************************************************/
	//static float AudioSourceGetVolume(AudioSource* pAudioSource)
	//{
	//	return pAudioSource->volume;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Pause all animations
	//*/
	///*******************************************************************************/
	//static void PauseAllAnimation()
	//{
	//	animationSystem.PauseAllAnimation();
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Play all animations
	//*/
	///*******************************************************************************/
	//static void PlayAllAnimation()
	//{
	//	animationSystem.PlayAllAnimation();
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Gets color of text

	//	\param ID
	//		ID of text

	//	\parama color
	//		Pointer to store color into
	//*/
	///*******************************************************************************/
	//static void GetTextColor(Text* pText, glm::vec4* color)
	//{
	//	*color = pText->color;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Sets color of text

	//	\param ID
	//		ID of text

	//	\parama color
	//		Pointer to retrieve color from
	//*/
	///*******************************************************************************/
	//static void SetTextColor(Text* pText, glm::vec4* color)
	//{
	//	pText->color = *color;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Gets hover color of button

	//	\param ID
	//		ID of button

	//	\parama color
	//		Pointer to store color into
	//*/
	///*******************************************************************************/
	//static void GetButtonHoverColor(Button* pButton, glm::vec4* color)
	//{
	//	*color = pButton->hoverColor;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Sets hover color of button

	//	\param ID
	//		ID of button

	//	\parama color
	//		Pointer to set color into
	//*/
	///*******************************************************************************/
	//static void SetButtonHoverColor(Button* pButton, glm::vec4* color)
	//{
	//	pButton->hoverColor = *color;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Gets clicked color of button

	//	\param ID
	//		ID of button

	//	\parama color
	//		Pointer to get color from
	//*/
	///*******************************************************************************/
	//static void GetButtonClickedColor(Button* pButton, glm::vec4* color)
	//{
	//	*color = pButton->clickedColor;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Sets clicked color of button

	//	\param ID
	//		ID of button

	//	\parama color
	//		Pointer to set color into
	//*/
	///*******************************************************************************/
	//static void SetButtonClickedColor(Button* pButton, glm::vec4* color)
	//{
	//	pButton->clickedColor = *color;
	//}


	///*******************************************************************************
	///*!
	//	\brief
	//		Gets color of sprite renderer

	//	\param ID
	//		ID of sprite renderer

	//	\parama color
	//		Pointer to store color in
	//*/
	///*******************************************************************************/
	//static void GetSpriteRendererColor(SpriteRenderer* pSpriteRenderer, glm::vec4* color)
	//{
	//	*color = pSpriteRenderer->sprite.color;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Sets color of sprite renderer

	//	\param ID
	//		ID of sprite renderer

	//	\parama color
	//		Pointer to get color from
	//*/
	///*******************************************************************************/
	//static void SetSpriteRendererColor(SpriteRenderer* pSpriteRenderer, glm::vec4* color)
	//{
	//	pSpriteRenderer->sprite.color = *color;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Gets color of image

	//	\param ID
	//		ID of image

	//	\parama color
	//		Pointer to store color in
	//*/
	///*******************************************************************************/
	//static void GetImageColor(Image* pImage, glm::vec4* color)
	//{
	//	*color = pImage->sprite.color;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Sets color of image

	//	\param ID
	//		ID of image

	//	\parama color
	//		Pointer to get color from
	//*/
	///*******************************************************************************/
	//static void SetImageColor(Image* pImage, glm::vec4* color)
	//{
	//	pImage->sprite.color = *color;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Plays animation of a animator

	//	\param ID
	//		ID of gameObject with animator
	//*/
	///*******************************************************************************/
	//static void PlayAnimation(Animator* pAnimator)
	//{
	//	pAnimator->PlayAnimation();
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Pauses animation of a animator

	//	\param ID
	//		ID of gameObject with animator
	//*/
	///*******************************************************************************/
	//static void PauseAnimation(Animator* pAnimator)
	//{
	//	pAnimator->PauseAnimation();
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Sets animation delay of an animator

	//	\param componentID
	//		ID of animator

	//	\param timeDelay
	//		Amount of delay to set
	//*/
	///*******************************************************************************/
	//static void SetAnimatorDelay(Animator* pAnimator, float timeDelay)
	//{
	//	pAnimator->animations[0].timeDelay = timeDelay;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Gets animation delay of an animator

	//	\param componentID
	//		ID of animator

	//	\return
	//		Amount of delay
	//*/
	///*******************************************************************************/
	//static float GetAnimatorDelay(Animator* pAnimator)
	//{
	//	return (float)pAnimator->animations[0].timeDelay;
	//}

	//static void SetFrame(Animator* pAnimator, int frame)
	//{
	//	pAnimator->GetCurrentAnimation()->SetFrame(frame);
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Gets color of animation

	//	\param pSpriteRenderer
	//		Reference to animator

	//	\parama color
	//		Pointer to store color in
	//*/
	///*******************************************************************************/
	//static void GetAnimationColor(Animator* pAnimator, glm::vec4* color)
	//{
	//	*color = pAnimator->GetCurrentAnimation()->color;
	//}

	///*******************************************************************************
	///*!
	//	\brief
	//		Sets color of animation

	//	\param pAnimator
	//		Reference to the animator

	//	\parama color
	//		Pointer to get color from
	//*/
	///*******************************************************************************/
	//static void SetAnimationColor(Animator* pAnimator, glm::vec4* color)
	//{
	//	pAnimator->GetCurrentAnimation()->color = *color;
	//}

	//static void StopAnimation(Animator* pAnimator)
	//{
	//	pAnimator->PauseAnimation();
	//}


	///*******************************************************************************
	///*!
	//	\brief
	//		Logs a message to editor

	//	\param message
	//		Mono string with message to be converted to c string
	//*/
	///*******************************************************************************/
	//static void Log(MonoString* message)
	//{
	//	char* str = mono_string_to_utf8(message);
	//	MyEventSystem->publish(new EditorConsoleLogEvent(str));
	//	mono_free(str);
	//}

	template<typename T,typename... Ts>
	static void RegisterComponent()
	{
		std::string typeName = "BeanFactory.";
		typeName += GetType::Name<T>();
		MonoType* managedType = mono_reflection_type_from_name(typeName.data(), SCRIPTING.GetAssemblyImage());
		if (managedType != nullptr)
		{
			E_ASSERT(managedType, "Could not find component type");
			monoComponentToType.emplace(managedType, GetType::E<T>());
		}
		if constexpr (sizeof...(Ts) != 0)
		{
			return RegisterComponent<Ts...>();
		}
	}

	template<typename... Component>
	static void RegisterComponent(TemplatePack<Component...>)
	{
		RegisterComponent<Component...>();
	}

	static void RegisterComponents()
	{
		monoComponentToType.clear();
		RegisterComponent(AllComponentTypes());
	}

	/*******************************************************************************
	/*!
	\brief
		Registers all defined internal calls with mono
	*/
	/*******************************************************************************/
	static void RegisterScriptWrappers()
	{
		//Register(SetFullscreenMode);
		Register(GetKey);
		Register(GetKeyUp);
		Register(GetKeyDown);
		Register(GetMouseDown);
		Register(DestroyGameObject);
		//Register(GetMousePosition);
		//Register(GetTranslation);
		//Register(SetTranslation);
		//Register(GetGlobalPosition);
		//Register(GetGlobalScale);
		Register(HasComponent);
		Register(Get);
		//Register(RigidbodyAddForce);
		//Register(RigidbodyGetVelocity);
		//Register(RigidbodySetVelocity);
		//Register(GetGameNDC);
		//Register(SetLocalScale);
		//Register(GetLocalScale);
		//Register(GetRotation);
		//Register(SetRotation);
		//Register(GetDeltaTime);
		//Register(SetActive);
		//Register(GetActive);
		//Register(GetTextString);
		//Register(SetTextString);
		//Register(LoadScene);
		//Register(CloneGameObject);
		//Register(InstantiateGameObject);
		//Register(DestroyGameObject);
		//Register(QuitGame);
		//Register(GetButtonState);
		//Register(Add);
		//Register(AudioSourcePlay);
		//Register(AudioSourceStop);
		//Register(AudioSourceSetVolume);
		//Register(AudioSourceGetVolume);
		//Register(SetAllVolume);
		//Register(AudioMute);
		//Register(GetSoundLength);
		//Register(PauseAllAnimation);
		//Register(PlayAllAnimation);
		//Register(StopAnimation);
		//Register(GetEnabled);
		//Register(SetComponentEnabled);
		//Register(SetParent);
		//Register(GetFPS);
		//Register(Log);
		//Register(GetSpriteRendererColor);
		//Register(SetSpriteRendererColor);
		//Register(GetAnimationColor);
		//Register(SetAnimationColor);
		//Register(GetImageColor);
		//Register(SetImageColor);
		//Register(GetTextColor);
		//Register(SetTextColor);
		//Register(PlayAnimation);
		//Register(PauseAnimation);
		//Register(SetFrame);
		//Register(SetAnimatorDelay);
		//Register(GetAnimatorDelay);
		//Register(GetButtonHoverColor);
		//Register(SetButtonHoverColor);
		//Register(GetButtonClickedColor);
		//Register(SetButtonClickedColor);
	}
#endif // !SCRIPT_WRAPPERS_H