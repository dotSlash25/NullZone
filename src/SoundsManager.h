#pragma once

class SoundsManager {
private:
    static const int numAliases = 5;
    static const int soundAliasCount = 5;
    static const int numSounds = 6;
    Sound sounds[numSounds];
    Sound soundAliases[soundAliasCount][numAliases];
    int currentPlayingAlias[soundAliasCount];
    std::string paths[numSounds];
    std::thread loadingThread;
    void loadData();
public:
    std::atomic<float> unitLoaded = 0;
    std::atomic<float> isLoading = false;
    SoundsManager();
    void loadSounds();
    Sound getSound(int id);
    void playSound(int id);
    void playSoundWithVarPitch(int id);
    void unload();
};

SoundsManager::SoundsManager() {
    paths[0] = ("audio/pistol.wav");
    paths[1] = ("audio/smg.wav");
    paths[2] = ("audio/rifle.wav");
    paths[3] = ("audio/shotgun.wav");
    paths[4] = ("audio/sniper.wav");
    paths[5] = "audio/walking.wav";
}

inline void SoundsManager::loadSounds() {
    if (!isLoading) {
        isLoading = true;
        unitLoaded = 0;
        loadingThread = std::thread(&SoundsManager::loadData, this);
    }
}

inline Sound SoundsManager::getSound(int id) {
    return sounds[id];
}

inline void SoundsManager::playSound(int id) {
    if (id < soundAliasCount)
        PlaySound(soundAliases[id][(currentPlayingAlias[id]++)%numAliases]);
    else
        PlaySound(sounds[id]);
}

inline void SoundsManager::playSoundWithVarPitch(int id) {
    float pitch = 0.9f + (float)GetRandomValue(0, 20) / 100.0f; 
    if (id < soundAliasCount) {
        SetSoundPitch(soundAliases[id][(currentPlayingAlias[id]++)%numAliases], pitch);
        PlaySound(soundAliases[id][(currentPlayingAlias[id]++)%numAliases]);
    } else {
        SetSoundPitch(sounds[id], pitch);
        PlaySound(sounds[id]);
    }
}

inline void SoundsManager::unload() {
    loadingThread.join();
    printf("Sounds are unloading here\n");
    for (int i = 0; i < soundAliasCount; i++) {
        for (int j = 0; j < numAliases; j++) {
            UnloadSoundAlias(soundAliases[i][j]);
        }
    }
    for (int i = 0; i < numSounds; i++)
    {
        UnloadSound(sounds[i]);
    }
}

inline void SoundsManager::loadData() {
    for (int i = 0; i < numSounds; i++) {
        sounds[i] = LoadSound(paths[i].c_str());
        unitLoaded = (float)(i+1)/numSounds;
    }

    for (int i = 0; i < soundAliasCount; i++) {
        for (int j = 0; j < numAliases; j++) {
            soundAliases[i][j] = LoadSoundAlias(sounds[i]);
        }
    }
    isLoading = false;
}

extern SoundsManager soundManager;