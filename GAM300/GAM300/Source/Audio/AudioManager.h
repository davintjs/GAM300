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
	void InitAudioManager();
	void DestroyAudioManager();
	void Update(float dt);

	void AddMusic(const std::string& path, const std::string& name);
	void AddLoopFX(const std::string& path, const std::string& name);
	void AddSFX(const std::string& path, const std::string& name);

	void PlayMusic(const std::string path);
	void PlayLoopFX(const std::string path, float pan = 0.f, float vol = 1.f);
	void PlayMusic();
	void PlaySFX(const std::string& name, float pan = 0,
		float minVolume = 1, float maxVolume = 1,
		float minPitch = -1, float maxPitch = 3);

	void StopFX();
	void StopMusic();
	void PauseMusic();

	void SetMasterVolume(float volume);
	float GetMasterVolume();
	float GetSFXVolume();
	float GetMusicVolume();
	void SetSFXVolume(float volume);
	void SetMusicVolume(float volume);

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