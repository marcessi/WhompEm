#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() : soundSystem(nullptr), backgroundMusic(nullptr), musicChannel(nullptr) {}

AudioManager::~AudioManager() {
    release();
}

void AudioManager::init() {
    FMOD::System_Create(&soundSystem);
    soundSystem->init(512, FMOD_INIT_NORMAL, nullptr);
}

void AudioManager::update() {
    if (soundSystem) {
        // Comprobar qu� canales han terminado para limpiarlos
        auto it = soundChannels.begin();
        while (it != soundChannels.end()) {
            bool isPlaying = false;
            it->second->isPlaying(&isPlaying);
            if (!isPlaying) {
                it = soundChannels.erase(it);
            }
            else {
                ++it;
            }
        }

        // Actualizar el sistema FMOD
        soundSystem->update();
    }
}

void AudioManager::release() {
    for (auto& sound : soundEffects) {
        sound.second->release();
    }
    soundEffects.clear();
    if (backgroundMusic) {
        backgroundMusic->release();
        backgroundMusic = nullptr;
    }
    if (soundSystem) {
        soundSystem->close();
        soundSystem->release();
        soundSystem = nullptr;
    }
}

void AudioManager::playMusic(const std::string& filePath, bool loop, float volume) {
    if (backgroundMusic) {
        backgroundMusic->release();
    }
    soundSystem->createStream(filePath.c_str(), loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF, nullptr, &backgroundMusic);
    soundSystem->playSound(backgroundMusic, nullptr, false, &musicChannel);
    musicChannel->setVolume(volume);
}

void AudioManager::stopMusic() {
    if (musicChannel) {
        musicChannel->stop();
    }
}

void AudioManager::pauseMusic() {
    if (musicChannel) {
        musicChannel->setPaused(true);
    }
}

void AudioManager::resumeMusic() {
    if (musicChannel) {
        musicChannel->setPaused(false);
    }
}

void AudioManager::setMusicVolume(float volume) {
    if (musicChannel) {
        musicChannel->setVolume(volume);
    }
}

void AudioManager::loadSound(const std::string& name, const std::string& filePath) {
    FMOD::Sound* sound;
    soundSystem->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &sound);
    soundEffects[name] = sound;
}

void AudioManager::playSound(const std::string& name, float volume) {
    auto it = soundEffects.find(name);
    if (it != soundEffects.end()) {
        // Crear un nombre �nico para el canal usando el tiempo actual
        static int counter = 0;
        std::string uniqueChannelName = name + "_" + std::to_string(counter++);

        // Crear un nuevo canal
        FMOD::Channel* channel = nullptr;
        soundSystem->playSound(it->second, nullptr, false, &channel);
        channel->setVolume(volume);
        soundChannels[uniqueChannelName] = channel;
    }
}


void AudioManager::stopAllSounds() {
    for (auto& channel : soundChannels) {
        if (channel.second) {
            channel.second->stop();
        }
    }
    soundChannels.clear();
}
