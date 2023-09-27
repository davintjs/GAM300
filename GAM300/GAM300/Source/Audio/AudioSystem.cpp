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
		if (!audio.play) {
			continue;
		}
		audio.play = false;
		if (audio.channel == AudioSource::Channel::MUSIC) {
			// music should auto loop
			AUDIOMANAGER.PlayMusic(audio.currentSound);
			continue;
		}
		if (audio.channel == AudioSource::Channel::SFX) {
			//no loop
			AUDIOMANAGER.PlaySFX(audio.currentSound);
			continue;
		}

	}
	AUDIOMANAGER.Update(dt);
}
void AudioSystem::Exit() {

}
