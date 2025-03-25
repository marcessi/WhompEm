#include "AudioManager.h"
#include <fmod_errors.h>
#include <iostream>

AudioManager::AudioManager()
    : soundSystem(nullptr), backgroundMusic(nullptr), musicChannel(nullptr)
{
}

AudioManager::~AudioManager()
{
    release();
}

void AudioManager::init()
{
    FMOD::System_Create(&soundSystem);
    soundSystem->init(512, FMOD_INIT_NORMAL, nullptr);
}

void AudioManager::release()
{
    if (backgroundMusic) {
        backgroundMusic->release();
        backgroundMusic = nullptr;
    }

    for (auto& sound : soundEffects) {
        if (sound.second) {
            sound.second->release();
        }
    }
    soundEffects.clear();

    if (soundSystem) {
        soundSystem->close();
        soundSystem->release();
        soundSystem = nullptr;
    }
}

void AudioManager::playMusic(const std::string& filePath, bool loop, float volume)
{
    stopMusic();

    soundSystem->createStream(filePath.c_str(), loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF, nullptr, &backgroundMusic);
    soundSystem->playSound(backgroundMusic, nullptr, false, &musicChannel);
    musicChannel->setVolume(volume);
}

void AudioManager::stopMusic()
{
    if (musicChannel) {
        musicChannel->stop();
        backgroundMusic->release();
        backgroundMusic = nullptr;
        musicChannel = nullptr;
    }
}

void AudioManager::pauseMusic()
{
    if (musicChannel) {
        bool paused;
        musicChannel->getPaused(&paused);
        musicChannel->setPaused(!paused);
    }
}

void AudioManager::resumeMusic()
{
    if (musicChannel) {
        musicChannel->setPaused(false);
    }
}

void AudioManager::setMusicVolume(float volume)
{
    if (musicChannel) {
        musicChannel->setVolume(volume);
    }
}

void AudioManager::loadSound(const std::string& name, const std::string& filePath)
{
    FMOD::Sound* sound;
    soundSystem->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &sound);
    soundEffects[name] = sound;
}

void AudioManager::playSound(const std::string& name, float volume)
{
    auto it = soundEffects.find(name);
    if (it != soundEffects.end()) {
        FMOD::Channel* channel = nullptr;
        soundSystem->playSound(it->second, nullptr, false, &channel);
        if (channel) {
            channel->setVolume(volume);
        }
    }
}

void AudioManager::stopAllSounds()
{
    // Obtener el grupo de canales maestro
    FMOD::ChannelGroup* masterGroup;
    soundSystem->getMasterChannelGroup(&masterGroup);

    // Detener todos los canales del grupo maestro
    if (masterGroup) {
        masterGroup->stop();
    }

    // También detenemos la música explícitamente
    stopMusic();
}

