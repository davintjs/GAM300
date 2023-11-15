/*!***************************************************************************************
\file			AudioManager.cpp
\project
\author         Lian Khai Kiat

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definations of Audio Manager that does the following:
	1. Add sounds into system
	2. Play Sounds
	3. Stop Sounds
	4. Pause Musics
	5. Set settings for each channels

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include <Precompiled.h>
#include "AudioManager.h"
#include "Core/EventsManager.h"

void AudioManager::InitAudioManager() {
	EVENTS.Subscribe(this, &AudioManager::CallbackAudioAssetLoaded);
	EVENTS.Subscribe(this, &AudioManager::CallbackAudioAssetUnloaded);
	EVENTS.Subscribe(this, &AudioManager::CallbackSceneStop);
	FMOD::System_Create(&system);
	system->init(32, FMOD_INIT_NORMAL, 0);
	system->getMasterChannelGroup(&master);
	for (int i = 0; i < CATEGORY_COUNT; ++i) {
		system->createChannelGroup(0, &groups[i]);
		groups[i]->setVolume(1.f);
		master->addGroup(groups[i]);
	}
	master->setVolume(1.f);
	// Set up modes for each category
	modes[CATEGORY_SFX] = FMOD_DEFAULT;
	modes[CATEGORY_MUSIC] = FMOD_DEFAULT | FMOD_LOOP_NORMAL;
	modes[CATEGORY_LOOPFX] = FMOD_DEFAULT | FMOD_LOOP_NORMAL;
	// Seed random number generator for SFXs
	srand((unsigned int)time(0));

	soundvec.resize(static_cast<size_t>(CATEGORY_COUNT));
}

void AudioManager::DestroyAudioManager() {
	SoundMap::iterator iter;
	for (int i = 0; i < CATEGORY_COUNT; ++i) {
		for (iter = sounds[i].begin(); iter != sounds[i].end(); ++iter) {
			iter->second->release();
		}
		sounds[i].clear();
	}
	// Release system
	system->release();
}

void AudioManager::Update(float dt) {
	const float fadeTime = .2f; // in seconds
	if (currentMusic != 0 && fade == FADE_IN) {
		float volume;
		currentMusic->getVolume(&volume);
		float nextVolume = volume + dt / fadeTime;
		if (nextVolume >= musicVolume || nextVolume <= -musicVolume) {
			currentMusic->setVolume(musicVolume);
			fade = FADE_NONE;
		}
		else {
			currentMusic->setVolume(nextVolume);
		}
	}
	else if (currentMusic != 0 && fade == FADE_OUT) {
		float volume;
		currentMusic->getVolume(&volume);
		float nextVolume = volume - dt / fadeTime;
		if (nextVolume <= 0.0f) {
			currentMusic->stop();
			currentMusic = 0;
			currentMusicPath = 0;
			fade = FADE_NONE;
			if (nextMusicPath != 0) {
				PlayMusic(nextMusicPath);
				fade = FADE_IN;
				nextMusicPath = 0;
			}
		}
		else {
			currentMusic->setVolume(nextVolume);
		}
	}
	else if (currentMusic == 0 && nextMusicPath != 0) {
		PlayMusic(nextMusicPath);
	}
	currentFX->setVolume(GetSFXVolume() * loopfxVolume);
	system->update();
}


void AudioManager::AddMusic(const std::string& path, const Engine::GUID<AudioAsset>& name) {

	FMOD::Sound* sound;
	FMOD_RESULT r = system->createSound(path.c_str(), modes[CATEGORY_MUSIC], 0, &sound);
	E_ASSERT(r == FMOD_OK, path, " doesnt exist!\n");
	sounds[CATEGORY_MUSIC].emplace(name, sound);
}

void AudioManager::AddLoopFX(const std::string& path, const Engine::GUID<AudioAsset> name) {

	FMOD::Sound* sound;
	FMOD_RESULT r = system->createSound(path.c_str(), modes[CATEGORY_LOOPFX], 0, &sound);
	E_ASSERT(r == FMOD_OK, path, " doesnt exist!\n");

	sounds[CATEGORY_LOOPFX].emplace(name, sound);
}

void AudioManager::AddSFX(const std::string& path, const Engine::GUID<AudioAsset> name) {
	FMOD::Sound* sound;
	FMOD_RESULT r = system->createSound(path.c_str(), modes[CATEGORY_SFX], 0, &sound);
	E_ASSERT(r == FMOD_OK, path, " doesnt exist!\n");
	sounds[CATEGORY_SFX].emplace(name, sound);/**/
}

//@kk change var name
void AudioManager::PlayMusic(const Engine::GUID<AudioAsset> name) {

	if (currentMusicPath == name) {
		groups[CATEGORY_MUSIC]->setPaused(false);
		currentMusic->setPaused(false);
		return;
	}
	// If a Music is playing stop them and set this as the next Music
	if (currentMusic != 0) {
		AudioManager::StopMusic();
		nextMusicPath = name;
		return;
	}
	// Find the Music in the corresponding sound map
	SoundMap::iterator sound = sounds[CATEGORY_MUSIC].find(name);
	//E_ASSERT(sound != sounds[CATEGORY_MUSIC].end(), name.ToHexString(), " not found!\n");
	if (sound == sounds[CATEGORY_MUSIC].end()) {
		return;
	}
	// Start playing Music with volume set to 0 and fade in
	currentMusicPath = name;
	system->playSound(sound->second, 0, true, &currentMusic);
	currentMusic->setChannelGroup(groups[CATEGORY_MUSIC]);
	currentMusic->setVolume(1.f);
	currentMusic->setPaused(false);
	groups[CATEGORY_MUSIC]->setPaused(false);
	fade = FADE_IN;

}

void AudioManager::PlayLoopFX(const Engine::GUID<AudioAsset> name, float pan, float vol,
	float minPitch, float maxPitch) {

	//std::string name = "../Sandbox/Assets/All/Sounds/" + filename + ".mp3";
	if (currentFXPath == name) {
		groups[CATEGORY_LOOPFX]->setPaused(false);
		currentFX->setPaused(false);
		currentFX->setPan(pan);
		loopfxVolume = vol;
		int count{ 1 };
		currentFX->getLoopCount(&count);
		if (count == 1 || count == -1) {
			count = 1;
			currentFX->setFrequency(44100.f);
		}
		currentFX->setLoopCount(count);
		if (count == 0) {
			static float semitone_ratio = pow(2.0f, 1.0f / 12.0f);
			float frequency{1};
			float pitch = RandFloat(minPitch, maxPitch);
			currentFX->getFrequency(&frequency);
			frequency = frequency * pow(semitone_ratio, pitch);
			currentFX->setFrequency(frequency);
			currentFX->setLoopCount(2);

		}
		return;
	}
	// If a Music is playing stop them and set this as the next Music
	if (currentFX != 0) {
		//AudioManager::StopFX();
		
		return;
	}
	// Find the Music in the corresponding sound map
	SoundMap::iterator sound = sounds[CATEGORY_LOOPFX].find(name);
	//E_ASSERT(sound != sounds[CATEGORY_LOOPFX].end(), name.ToHexString(), " not found!\n");
	if (sound == sounds[CATEGORY_LOOPFX].end()) {
		return;
	}
	// Start playing Music with volume set to 0 and fade in
	currentFXPath = name;
	system->playSound(sound->second, 0, true, &currentFX);
	currentFX->setChannelGroup(groups[CATEGORY_LOOPFX]);
	//volume = RandFloat(minVolume, maxVolume);
	
	currentFX->setVolume(1.0f);
	currentFX->setPan(pan);
	loopfxVolume = vol;
	currentFX->setPaused(false);
	groups[CATEGORY_LOOPFX]->setPaused(false);

}

void AudioManager::PlayMusic() {

	if (currentMusic) {
		groups[CATEGORY_MUSIC]->setPaused(false);
		currentMusic->setPaused(false);
		return;
	}
	if (nextMusicPath != 0)
	{
		currentMusicPath = nextMusicPath;
	}
	//StopMusic();
}

void AudioManager::PlayComponent(AudioSource& Source) {
	switch (Source.current_channel)
	{
	case 0: // Music
		Source.play = true;
		currentMusic->setVolume(Source.volume);
		PlayMusic(Source.currentSound);
		break;
	case 1: // SFX
		currentMusic->setVolume(Source.volume);
		PlaySFX(Source.currentSound);
		break;
	default:
		break;
	}
}


void AudioManager::PauseMusic() {
	groups[CATEGORY_MUSIC]->setPaused(true);
	currentMusic->setPaused(true);
}

void AudioManager::PauseLoopFX() {
	groups[CATEGORY_LOOPFX]->setPaused(true);
	currentFX->setPaused(true);
}

void AudioManager::PlaySFX(const Engine::GUID<AudioAsset> name,
	float pan,
	float minVolume, float maxVolume,
	float minPitch, float maxPitch)
{
	SoundMap::iterator sound = sounds[CATEGORY_SFX].find(name);
	if (sound == sounds[CATEGORY_SFX].end()) {
		return;
	}
	FMOD::Channel* channel;
	system->playSound(sound->second, 0, false, &channel);
	float frequency;
	static float semitone_ratio = pow(2.0f, 1.0f / 12.0f);
	float volume, pitch;
	volume = RandFloat(minVolume, maxVolume);
	pitch = RandFloat(minPitch, maxPitch);
	channel->setChannelGroup(groups[CATEGORY_SFX]);
	channel->setVolume(volume);
	channel->getFrequency(&frequency);

	frequency = frequency * pow(semitone_ratio, pitch);
	channel->setFrequency(frequency);
	channel->setPan(pan);
	channel->setPaused(false);/**/
}

void AudioManager::StopMusic() {
	if (currentMusic != 0) {
		fade = FADE_OUT;
	}
}
void AudioManager::StopFX() {
	currentFX->stop();
	currentFX = 0;
	currentFXPath = 0;
}

void AudioManager::StopAllAudio() {
	currentMusicPath = 0;
	currentFXPath = 0;
	nextMusicPath = 0;
	groups[CATEGORY_SFX]->stop();
	groups[CATEGORY_MUSIC]->stop();
	groups[CATEGORY_LOOPFX]->stop();
	currentMusic = 0;
	currentMusic->stop();
	currentFX->stop();
	fade = FADE_OUT;
}

void AudioManager::StopAudioComponent(AudioSource& Source) {
	groups[Source.current_channel]->stop();
	Source.play = 0;
	//sounds[Source.current_channel][Source.current_channel]
	switch (static_cast<Category>(Source.current_channel))
	{
	case CATEGORY_SFX:
		currentFXPath = 0;
		groups[CATEGORY_SFX]->stop();
		currentFX->stop();
		break;
	case CATEGORY_MUSIC:
		currentFXPath = 0;
		groups[CATEGORY_MUSIC]->stop();
		currentMusic->stop();
		fade = FADE_OUT;
		break;
	case CATEGORY_LOOPFX:
		currentFXPath = 0;
		groups[CATEGORY_LOOPFX]->stop();
		currentFX->stop();
		break;
	default:
		break;
	}
}

void AudioManager::SetMasterVolume(float volume) {
	master->setVolume(volume);
}

void AudioManager::SetSFXVolume(float volume) {
	groups[CATEGORY_SFX]->setVolume(volume);
}

void AudioManager::SetMusicVolume(float volume) {
	groups[CATEGORY_MUSIC]->setVolume(volume);
	currentMusic->setVolume(volume);
	musicVolume = volume;
}
float AudioManager::GetMasterVolume() {
	float vol;
	master->getVolume(&vol);
	return vol;
}
float AudioManager::GetSFXVolume() {
	float vol;
	groups[CATEGORY_SFX]->getVolume(&vol);
	return vol;
}

float AudioManager::GetMusicVolume() {

	return musicVolume;
}

float AudioManager::RandFloat(float min, float max) {
	if (min == max) return min;
	float n = (float)rand() / (float)RAND_MAX;
	return min + n * (max - min);
}

//Handle audio adding here
void AudioManager::CallbackAudioAssetLoaded(AssetLoadedEvent<AudioAsset>* pEvent)
{
	// @kk rmb to delete when done
	/*AUDIOMANAGER.AddMusic(pEvent->assetPath.string(), pEvent->assetPath.stem().string());
	AUDIOMANAGER.AddSFX(pEvent->assetPath.string(), pEvent->assetPath.stem().string());*/
	std::cout << "guid: " << pEvent->guid.ToHexString() << "\t\t sound: " << pEvent->assetPath.stem().string() << std::endl;
	AUDIOMANAGER.AddMusic(pEvent->assetPath.string(), pEvent->guid);
	AUDIOMANAGER.AddSFX(pEvent->assetPath.string(), pEvent->guid);
	AUDIOMANAGER.AddLoopFX(pEvent->assetPath.string(), pEvent->guid);
}

//Handle audio removal here
void AudioManager::CallbackAudioAssetUnloaded(AssetUnloadedEvent<AudioAsset>* pEvent)
{

}


void AudioManager::CallbackSceneStop(SceneStopEvent* pEvent)
{
	StopAllAudio();
}