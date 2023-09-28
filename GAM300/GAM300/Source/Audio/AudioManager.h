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

#define AUDIOMANAGER AudioManager::Instance()
using SoundMap = std::map<std::string, FMOD::Sound*>;

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
	void AddMusic(const std::string& path, const std::string& name);

	// add looping SFX onto LoopFX channel
	void AddLoopFX(const std::string& path, const std::string& name);

	// add SFX into SFX channel
	void AddSFX(const std::string& path, const std::string& name);	

	// Play / Unpause music with the filename
	void PlayMusic(const std::string path);

	// Unpause Music
	void PlayMusic();
	
	// Play SFX with the filename on loop
	void PlayLoopFX(const std::string path, float pan = 0.f, float vol = 1.f);

	// Play SFX once
	void PlaySFX(const std::string& name, float pan = 0,
		float minVolume = 1, float maxVolume = 1,
		float minPitch = -1, float maxPitch = 3);

	// stop SFX from playing
	void StopFX();

	// stop current music
	void StopMusic();

	// pause current music
	void PauseMusic();

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
private:

	FMOD::System* system;
	FMOD::ChannelGroup* master;
	FMOD::ChannelGroup* groups[CATEGORY_COUNT];
	FMOD_MODE modes[CATEGORY_COUNT];
	FMOD::Channel* currentMusic;
	FMOD::Channel* currentFX;
	FMOD::Channel* currentFootSteps;
	SoundMap sounds[CATEGORY_COUNT];

	std::string currentMusicPath;
	std::string currentFXPath;
	std::string stepPath;

	std::string nextMusicPath;
	std::string nextStepPath;


	FadeState fade;

	float musicVolume{ 1.f };
	float loopfxVolume{ 1.f };
	float fixStepTime{ .5f };
	float stepTime{ .0f };

	bool enableStep{ false };
};