#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <fmod.hpp>
#include <unordered_map>
#include <string>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void init();
    void release();

    void playMusic(const std::string& filePath, bool loop, float volume);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(float volume);

    void loadSound(const std::string& name, const std::string& filePath);
    void playSound(const std::string& name, float volume);
    void stopAllSounds();

private:
    FMOD::System* soundSystem;
    FMOD::Sound* backgroundMusic;
    FMOD::Channel* musicChannel;
    std::unordered_map<std::string, FMOD::Sound*> soundEffects;
};

#endif // AUDIOMANAGER_H
