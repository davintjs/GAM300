/*!***************************************************************************************
\file			AudioSystem.h
\project
\author         Lian Khai Kiat

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definations of Audio System

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

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
			if (audio.current_channel == (int)AudioSource::Channel::MUSIC) {
				AUDIOMANAGER.PauseMusic();
			}
			continue;
		}

		// update music settings
		if (audio.current_channel == (int)AudioSource::Channel::MUSIC) {
			// music should auto loop
			AUDIOMANAGER.SetMusicVolume(audio.volume);
			AUDIOMANAGER.PlayMusic(audio.currentSound);
		}

		// update SFX settings
		if (audio.current_channel == (int)AudioSource::Channel::SFX) {
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
