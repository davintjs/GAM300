/*!***************************************************************************************
\file			AudioManager.h
\project
\author         Lian Khai Kiat

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations Audio Manager that does the following:
	1. Add sounds into system
	2. Play Sounds
	3. Stop Sounds
	4. Pause Musics
	5. Set settings for each channels

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#include <FMOD/fmod.hpp>
#include <Core/Events.h>
#include <AssetManager/AssetTypes.h>
#include "Scene/Components.h" // @kk or @bean does this slow down compile time? i need AudioSource component tho

#define AUDIOMANAGER AudioManager::Instance()
//using SoundMap = std::map<std::string, FMOD::Sound*>; //@kk delete once done
using SoundMap = std::unordered_map<Engine::GUID, FMOD::Sound*>;

SINGLETON(AudioManager) {
private:
	//static AudioManager* audioManager;
	enum Category { CATEGORY_SFX, CATEGORY_MUSIC, CATEGORY_LOOPFX, CATEGORY_COUNT };
	enum FadeState { FADE_NONE, FADE_IN, FADE_OUT };
public:
	// initialize Audio Manager
	void InitAudioManager();

	// Clear all channels and other settings
	void DestroyAudioManager();

	// update music settings accordingly
	void Update(float dt);		

	// add music into music channel
	//void AddMusic(const std::string& soundGUID, const std::string& name); // @kk delete once done
	void AddMusic(const std::string& path, const Engine::GUID& soundGUID);

	// add looping SFX onto LoopFX channel
	//void AddLoopFX(const std::string& soundGUID, const std::string& name);
	void AddLoopFX(const std::string& path, const Engine::GUID soundGUID);

	// add SFX into SFX channel
	//void AddSFX(const std::string& soundGUID, const std::string& name);
	void AddSFX(const std::string& path, const Engine::GUID soundGUID);

	// Play / Unpause music with the filename
	//void PlayMusic(const std::string soundGUID);
	void PlayMusic(Engine::GUID soundGUID);

	// Unpause Music
	void PlayMusic();
	
	// Play SFX with the filename on loop
	//void PlayLoopFX(const std::string soundGUID, float pan = 0.f, float vol = 1.f);
	void PlayLoopFX(Engine::GUID soundGUID, float pan = 0.f, float vol = 1.f,
		float minPitch = -1, float maxPitch = 3);

	// Play SFX once
	//void PlaySFX(const std::string& name, float pan = 0,
	void PlaySFX(Engine::GUID soundGUID, float pan = 0,
		float minVolume = 1, float maxVolume = 1,
		float minPitch = -1, float maxPitch = 3);

	// play from component
	void PlayComponent(AudioSource Source);

	// stop SFX from playing
	void StopFX();

	// stop current music
	void StopMusic();

	// pause current music
	void PauseMusic();

	//pause loopinf sfx
	void PauseLoopFX();

	// Functions for volume settings
	void SetMasterVolume(float volume);
	float GetMasterVolume();
	float GetSFXVolume();
	float GetMusicVolume();
	void SetSFXVolume(float volume);
	void SetMusicVolume(float volume);
	// Functions for volume settings end

	// Generate random float number
	float RandFloat(float min, float max);

	//Handle audio adding here
	void CallbackAudioAssetLoaded(AssetLoadedEvent<AudioAsset>* pEvent);

	//Handle audio removal here
	void CallbackAudioAssetUnloaded(AssetUnloadedEvent<AudioAsset>* pEvent);
private:

	FMOD::System* system{};
	FMOD::ChannelGroup* master{};
	FMOD::ChannelGroup* groups[CATEGORY_COUNT]{};
	FMOD_MODE modes[CATEGORY_COUNT]{};
	FMOD::Channel* currentMusic{};
	FMOD::Channel* currentFX{};
	FMOD::Channel* currentFootSteps{};
	SoundMap sounds[CATEGORY_COUNT];
	std::vector<SoundMap> soundvec;
	Engine::GUID currentMusicPath{ DEFAULT_ASSETS["None.wav"] };
	Engine::GUID currentFXPath{ DEFAULT_ASSETS["None.wav"] };
	Engine::GUID nextMusicPath{ DEFAULT_ASSETS["None.wav"] };


	FadeState fade{ FADE_NONE };

	float musicVolume{ 1.f };
	float loopfxVolume{ 1.f };
	float fixStepTime{ .5f };
	float stepTime{ .0f };

	bool enableStep{ false };
};