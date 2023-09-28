#include <Precompiled.h>
#include <Audio/AudioSystem.h>
#include <Audio/AudioManager.h>
#include "Scene/SceneManager.h"
#include "Core/EventsManager.h"


void AudioSystem::Init() {
	//AUDIOMANAGER.InitAudioManager();
}

void AudioSystem::Update(float dt) {
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();
	for (AudioSource& audio : currentScene.GetArray<AudioSource>()) {
		// if audio is not playing, skip this loop unless it is music
		if (!audio.play) {
			if (audio.channel == AudioSource::Channel::MUSIC) {
				AUDIOMANAGER.PauseMusic();
			}
			continue;
		}

		// update music settings
		if (audio.channel == AudioSource::Channel::MUSIC) {
			// music should auto loop
			AUDIOMANAGER.SetMusicVolume(audio.volume);
			AUDIOMANAGER.PlayMusic(audio.currentSound);
		}

		// update SFX settings
		if (audio.channel == AudioSource::Channel::SFX) {
			//no loop
			audio.play = false;
			AUDIOMANAGER.SetSFXVolume(audio.volume);
			AUDIOMANAGER.PlaySFX(audio.currentSound);
			continue;
		}
	}
	AUDIOMANAGER.Update(dt); // this is for loops and other fancy stuff
}
void AudioSystem::Exit() {

}
