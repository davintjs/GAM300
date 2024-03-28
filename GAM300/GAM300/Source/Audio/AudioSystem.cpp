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
	glm::vec3 campos(0.f);
	bool hasListener = false;
	for (AudioListener& listener : currentScene.GetArray<AudioListener>()) {

		Transform& transform = currentScene.Get<Transform>(listener);
		campos = transform.GetGlobalTranslation();
		FMOD_VECTOR pos = { campos.x , campos.y, campos.z };
		FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
		FMOD_VECTOR up = { 0.f, 1.f, 0.f };
		glm::vec3 gl_forward = transform.GetGlobalRotation() * glm::vec3{ 0.f, 0.f, -1.f };
		FMOD_VECTOR forward = { gl_forward.x, gl_forward.y, gl_forward.z };

		if (currentScene.Has<Camera>(currentScene.Get<Entity>(listener))) {
			hasListener = true;
			Camera& cam = currentScene.Get<Camera>(listener);

			forward = { cam.GetForwardVec().x, cam.GetForwardVec().y, cam.GetForwardVec().z };
			up = { cam.GetUpVec().x, cam.GetUpVec().y, cam.GetUpVec().z };
			AUDIOMANAGER.system->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
			break;
		}
	}

	for (AudioSource& audio : currentScene.GetArray<AudioSource>()) {
		if (audio.state == DELETED) continue;
		// if audio is not playing, skip this loop unless it is music
		if (!audio.play) {
			if (audio.current_channel == (int)AudioSource::Channel::SFX) {
				if (audio.channel != nullptr) {
					// Update position of the playing SFX
					Transform& pos = currentScene.Get<Transform>(audio);
					FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f }; // Assuming no velocity for now
					FMOD_VECTOR position = { pos.GetGlobalTranslation().x, pos.GetGlobalTranslation().y, pos.GetGlobalTranslation().z };
					audio.channel->set3DAttributes(&position, &velocity);
					bool play;
					audio.channel->isPlaying(&play);
					audio.channel = play ? audio.channel : nullptr;
				}
				continue;
			}
			if (audio.current_channel == (int)AudioSource::Channel::MUSIC) {
				AUDIOMANAGER.PauseMusic();
				continue;
			}
		}
		// update music settings
		if (audio.current_channel == (int)AudioSource::Channel::MUSIC) {
			// music should auto loop
			AUDIOMANAGER.SetMusicVolume(audio.volume);
			AUDIOMANAGER.PlayMusic(audio.currentSound, audio.fadeOutTime, audio.fadeInTime);
		}

		// update SFX settings
		if (audio.current_channel == (int)AudioSource::Channel::SFX && AUDIOMANAGER.SFXEnabled()) {

			Transform& pos = currentScene.Get<Transform>(audio);
			glm::vec3 glmPos = pos.GetGlobalTranslation();
			FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f }; // Assuming no velocity for now
			FMOD_VECTOR position = { glmPos.x, glmPos.y, glmPos.z };
			float newVol = audio.volume;
			if (glm::length2(glmPos - campos) >= audio.maxDistance * audio.maxDistance) {
				newVol = 0;
			}
			//AUDIOMANAGER.SetSFXVolume(newVol);
			if (audio.loop) {
				bool isPlaying;
				audio.channel->isPlaying(&isPlaying);
				if (!isPlaying) {
					audio.channel = AUDIOMANAGER.PlaySFX(audio.currentSound, position, audio.channel, audio.minDistance, audio.maxDistance, newVol, audio.minPitch, audio.maxPitch);
				}
			}
			else {
				audio.channel = AUDIOMANAGER.PlaySFX(audio.currentSound, position, audio.channel, audio.minDistance, audio.maxDistance, newVol, audio.minPitch, audio.maxPitch);
				audio.play = false;
			}
			audio.channel->set3DAttributes(&position, &velocity);
			continue;
		}
	}
	
	AUDIOMANAGER.Update(dt); // this is for loops and other fancy stuff
}
void AudioSystem::Exit() {

}
