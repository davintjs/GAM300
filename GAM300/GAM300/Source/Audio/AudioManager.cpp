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
#include "AppEntry/Application.h"

void AudioManager::InitAudioManager() {
	EVENTS.Subscribe(this, &AudioManager::CallbackAudioAssetLoaded);
	EVENTS.Subscribe(this, &AudioManager::CallbackAudioAssetUnloaded);
	EVENTS.Subscribe(this, &AudioManager::CallbackSceneStop);
	FMOD::System_Create(&system);
	system->init(32, FMOD_INIT_3D_RIGHTHANDED, 0);
	system->getMasterChannelGroup(&master);
	for (int i = 0; i < CATEGORY_COUNT; ++i) {
		system->createChannelGroup(0, &groups[i]);
		groups[i]->setVolume(1.f);
		master->addGroup(groups[i]);
	}
	master->setVolume(1.f);
	// Set up modes for each category
	modes[CATEGORY_SFX] = FMOD_3D;
	modes[CATEGORY_MUSIC] = FMOD_DEFAULT | FMOD_LOOP_NORMAL;
	modes[CATEGORY_MUSIC2] = FMOD_DEFAULT | FMOD_LOOP_NORMAL;
	//modes[CATEGORY_LOOPFX] = FMOD_DEFAULT | FMOD_LOOP_NORMAL;
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
	//const float fadeTime = 2.f; // in seconds
	static float masterVolume = GetMasterVolume();
	if (!Application::Instance().IsWindowFocused())
	{
		masterVolume = (GetMasterVolume() != 0.01f) ? GetMasterVolume() : masterVolume;
		master->setVolume(0.01f);
	}
	else
		master->setVolume(masterVolume);

	for (MusicBuffer& musicBuffer : musics) {
		if (musicBuffer.currentMusic != 0 && musicBuffer.fade == FADE_IN) {
			float volume;
			musicBuffer.currentMusic->getVolume(&volume);
			float nextVolume = volume + dt / musicBuffer.fadetime;
			if (nextVolume >= musicVolume || nextVolume <= -musicVolume) {
				musicBuffer.currentMusic->setVolume(musicVolume);
				musicBuffer.fade = FADE_NONE;
			}
			else {
				musicBuffer.currentMusic->setVolume(nextVolume);
			}
		}
		else if (musicBuffer.currentMusic != 0 && musicBuffer.fade == FADE_OUT) {
			float volume;
			musicBuffer.currentMusic->getVolume(&volume);
			float nextVolume = volume - dt / musicBuffer.fadetime;
			musicBuffer.currentMusic->setVolume(nextVolume);

			if (nextVolume <= 0.0f) {
				musicBuffer.currentMusic->stop();
				musicBuffer.currentMusic = 0;
				musicBuffer.currentMusicPath = 0;
				musicBuffer.fade = FADE_NONE;

				// want to completly stop because fade
				if (musicBuffer.nextMusicPath != 0) {
					PlayMusic(musicBuffer.nextMusicPath);
					musicBuffer.fade = FADE_IN;
					musicBuffer.nextMusicPath = 0;
				}/**/
			}
		}
		else if (musicBuffer.currentMusic == 0 && musicBuffer.nextMusicPath != 0) {
			PlayMusic(musicBuffer.nextMusicPath);
		}
	}
	
	currentFX->setVolume(GetSFXVolume() * loopfxVolume);
	/* int listener, vec* pos, vec* vel, vec* forward, vec* up */
	/*FMOD_VECTOR pos = { 0.f, 0.f, 1.f};
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR forward = { 0.0f, 0.0f, -1.0f };
	FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
	system->set3DListenerAttributes(0, &pos, &vel, &forward, &up);*/
	system->update();
}


void AudioManager::AddMusic(const std::string& path, const Engine::GUID<AudioAsset>& name) {

	FMOD::Sound* sound;
	FMOD_RESULT r = system->createSound(path.c_str(), modes[CATEGORY_MUSIC], 0, &sound);
	E_ASSERT(r == FMOD_OK, path, " doesnt exist!\n");
	sounds[CATEGORY_MUSIC].emplace(name, sound);

	FMOD::Sound* sound2;
	r = system->createSound(path.c_str(), modes[CATEGORY_MUSIC2], 0, &sound2);
	E_ASSERT(r == FMOD_OK, path, " doesnt exist!\n");
	sounds[CATEGORY_MUSIC2].emplace(name, sound2);
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
void AudioManager::PlayMusic(const Engine::GUID<AudioAsset> name, float componentFadeOut, float componentFadeIn) {

	// if on same music, just unpause
	if (musics[currentMusicIdx].currentMusicPath == name) {

		groups[CATEGORY_MUSIC]->setPaused(false);
		groups[CATEGORY_MUSIC2]->setPaused(false);
		musics[currentMusicIdx].currentMusic->setPaused(false);
		musics[currentMusicIdx].fadetime = componentFadeIn;
		return;
	}
	// if != name
	// If a Music is playing stop them and set this as the next Music
	if (musics[currentMusicIdx].currentMusic != 0) {

		if (musics[currentMusicIdx].fade == FADE_OUT && musics[currentMusicIdx].nextMusicPath != name) {

			currentMusicIdx ^= 1;
			// Find the Music in the corresponding sound map
			SoundMap::iterator sound = sounds[CATEGORY_MUSIC + currentMusicIdx].find(name);
			if (sound == sounds[CATEGORY_MUSIC + currentMusicIdx].end()) {
				return;
			}
			// Start playing Music with volume set to 0 and fade in
			musics[currentMusicIdx].currentMusicPath = name;
			system->playSound(sound->second, 0, true, &musics[currentMusicIdx].currentMusic);
			musics[currentMusicIdx].currentMusic->setChannelGroup(groups[CATEGORY_MUSIC + currentMusicIdx]);
			musics[currentMusicIdx].currentMusic->setVolume(0.f);
			musics[currentMusicIdx].currentMusic->setPaused(false);
			groups[CATEGORY_MUSIC + currentMusicIdx]->setPaused(false);
			musics[currentMusicIdx].fade = FADE_IN;
			musics[currentMusicIdx].fadetime = componentFadeIn;

			return;
		}

		musics[currentMusicIdx].fade = FADE_OUT;
		musics[currentMusicIdx].fadetime = componentFadeOut;
		musics[currentMusicIdx].nextMusicPath = name;
		return;
	}

	// Find the Music in the corresponding sound map
	SoundMap::iterator sound = sounds[CATEGORY_MUSIC + currentMusicIdx].find(name);
	if (sound == sounds[CATEGORY_MUSIC + currentMusicIdx].end()) {
		return;
	}
	// Start playing Music with volume set to 0 and fade in
	musics[currentMusicIdx].currentMusicPath = name;
	system->playSound(sound->second, 0, true, &musics[currentMusicIdx].currentMusic);
	musics[currentMusicIdx].currentMusic->setChannelGroup(groups[CATEGORY_MUSIC + currentMusicIdx]);
	musics[currentMusicIdx].currentMusic->setVolume(0.f);
	musics[currentMusicIdx].currentMusic->setPaused(false);
	groups[CATEGORY_MUSIC + currentMusicIdx]->setPaused(false);
	musics[currentMusicIdx].fade = FADE_IN;
	musics[currentMusicIdx].fadetime = componentFadeIn;
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
	if (musics[currentMusicIdx].currentMusic) {
		groups[CATEGORY_MUSIC]->setPaused(false);
		musics[currentMusicIdx].currentMusic->setPaused(false);
		return;
	}
	if (musics[currentMusicIdx].nextMusicPath != 0)
	{
		musics[currentMusicIdx].currentMusicPath = musics[currentMusicIdx].nextMusicPath;
	}
	//StopMusic();
}

void AudioManager::PlayComponent(AudioSource& Source) {
	switch (Source.current_channel)
	{
	case 0: // Music
		Source.play = true;
		//musics[currentMusicIdx].currentMusic->setVolume(Source.volume);
		groups[CATEGORY_MUSIC]->setVolume(Source.volume);
		groups[CATEGORY_MUSIC2]->setVolume(Source.volume);
		PlayMusic(Source.currentSound, Source.fadeOutTime, Source.fadeInTime);
		break;
	case 1: // SFX
		/*Source.play = true;*/
		if (SFXEnabled()) {
			Source.play = true;
			//PlaySFX(Source.currentSound);
			//Source.channel = AUDIOMANAGER.PlaySFX(Source.currentSound, position, Source.channel, Source.maxDistance, Source.volume, Source.volume, Source.minPitch, Source.maxPitch);
		}
		break;
	default:
		break;
	}
}


void AudioManager::PauseMusic() {
	groups[CATEGORY_MUSIC]->setPaused(true);
	musics[0].currentMusic->setPaused(true);
	musics[1].currentMusic->setPaused(true);
}

void AudioManager::PauseLoopFX() {
	groups[CATEGORY_LOOPFX]->setPaused(true);
	currentFX->setPaused(true);
}

void AudioManager::PauseComponent(AudioSource& source) {
	source.play = false;
}

FMOD::Channel* AudioManager::PlaySFX(const Engine::GUID<AudioAsset> name,
	FMOD_VECTOR pos, FMOD::Channel* channel,
	float minDistance,
	float maxDistance,
	float volume,
	float minPitch, float maxPitch)
{
	SoundMap::iterator sound = sounds[CATEGORY_SFX].find(name);
	if (sound == sounds[CATEGORY_SFX].end()) {
		return nullptr;
	}
	if (volume == 0.f)
	{
		return nullptr;
	}
	//FMOD::Channel* channel;
	system->playSound(sound->second, 0, false, &channel);
	float frequency;
	static float semitone_ratio = pow(2.0f, 1.0f / 12.0f);
	float pitch;
	pitch = RandFloat(minPitch, maxPitch);
	channel->setChannelGroup(groups[CATEGORY_SFX]);
	channel->setVolume(volume);
	channel->getFrequency(&frequency);

	frequency = frequency * pow(semitone_ratio, pitch);
	channel->setFrequency(frequency);
	channel->setPaused(false);/**/
	FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };
	channel->set3DAttributes(&pos, &velocity);
	channel->set3DMinMaxDistance(minDistance, maxDistance);
	return channel;
}

void AudioManager::UpdateSFXPosition(FMOD_VECTOR pos) {
	FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };
	/*for (FMOD::Channel& channel : playingSFXChannels) {
		channel.set3DAttributes(&pos, &velocity);
	}*/
}

void AudioManager::StopMusic(float fadetime) {
	if (musics[currentMusicIdx].currentMusic != 0) {
		musics[currentMusicIdx].fade = FADE_OUT;
		musics[currentMusicIdx].nextMusicPath = 0;
		musics[currentMusicIdx].fadetime = fadetime;
	}
}

void AudioManager::SetMusicFade(AudioSource& source, float fadeOut, float fadeIn) {
	source.fadeOutTime = fadeOut;
	source.fadeInTime = fadeIn;
	/*musics[currentMusicIdx].fadetime = fadeOut;
	musics[currentMusicIdx ^ 1].fadetime = fadeIn;*/
}

void AudioManager::StopFX() {
	currentFX->stop();
	currentFX = 0;
	currentFXPath = 0;
}

void AudioManager::EnableSFX(bool toggle) {
	enableSFX = toggle;
}
void AudioManager::StopAllAudio() {

	currentFXPath = 0;
	musics[0].currentMusicPath = 0;
	musics[0].nextMusicPath = 0;
	musics[0].currentMusic = 0;
	musics[0].currentMusic->stop();
	musics[0].fade = FADE_OUT;
	
	musics[1].currentMusicPath = 0;
	musics[1].nextMusicPath = 0;
	musics[1].currentMusic = 0;
	musics[1].currentMusic->stop();
	musics[1].fade = FADE_OUT;

	groups[CATEGORY_SFX]->stop();
	groups[CATEGORY_MUSIC]->stop();
	groups[CATEGORY_MUSIC2]->stop();
	groups[CATEGORY_LOOPFX]->stop();
	currentFX->stop();
}

void AudioManager::StopAudioComponent(AudioSource& Source) {
	groups[Source.current_channel]->stop();
	Source.play = 0;

	switch (static_cast<Category>(Source.current_channel))
	{
	case CATEGORY_SFX:
		currentFXPath = 0;
		groups[CATEGORY_SFX]->stop();
		currentFX->stop();
		break;
	case CATEGORY_MUSIC:
		currentFXPath = 0;
		//groups[CATEGORY_MUSIC]->stop();
		//musics[currentMusicIdx].currentMusic->stop();
		musics[currentMusicIdx].fade = FADE_OUT;
		break;
	/*case CATEGORY_LOOPFX:
		currentFXPath = 0;
		groups[CATEGORY_LOOPFX]->stop();
		currentFX->stop();
		break;*/
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
	groups[CATEGORY_MUSIC2]->setVolume(volume);
	//musics[currentMusicIdx].currentMusic->setVolume(volume);
	musicVolume = volume;
}
void AudioManager::SetComponentVolume(AudioSource& source, float volume) {
	source.volume = volume;
}

float AudioManager::GetComponentVolume(AudioSource& source) {
	return source.volume;
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
	AUDIOMANAGER.AddMusic(pEvent->asset.mFilePath.string(), pEvent->asset.importer->guid);
	AUDIOMANAGER.AddSFX(pEvent->asset.mFilePath.string(), pEvent->asset.importer->guid);
	//AUDIOMANAGER.AddLoopFX(pEvent->asset.mFilePath.string(), pEvent->asset.importer->guid);
}

//Handle audio removal here
void AudioManager::CallbackAudioAssetUnloaded(AssetUnloadedEvent<AudioAsset>* pEvent)
{

}


void AudioManager::CallbackSceneStop(SceneStopEvent* pEvent)
{
	StopAllAudio();
}