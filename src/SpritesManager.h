#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <string>

class SpritesManager
{
private:
    static const int numSprites = 15;
    std::string paths[numSprites];
    Image images[numSprites];
    Texture sprites[numSprites];
    std::thread loadingThread;
    void loadData();
public:
    void setup();
    std::atomic<bool> isLoading = false;
    std::atomic<float> unitLoaded = 0;
    SpritesManager();
    ~SpritesManager();
    void loadSprites();
    Texture sprite(int n);
    void unload();
};

SpritesManager::SpritesManager() {
    paths[0] = ("img/tilesetNew.png");
    paths[1] = ("img/gunsAlt.png");
    paths[2] = ("img/bullet.png");
    paths[3] = ("img/charactah.png");
    paths[4] = ("img/collectibles.png");
    paths[5] = ("img/particles.png");
    paths[6] = ("img/enemy1.png");
    paths[7] = ("img/enemy2.png");
    paths[8] = ("img/enemy3.png");
    paths[9] = ("img/grenade.png");
    paths[10] = ("img/crosshair.png");
    paths[11] = ("img/explosionParticles.png");
    paths[12] = ("img/explosion.png");
    paths[13] = ("img/ally.png");
    paths[14] = ("img/allySpawner.png");
}

inline void SpritesManager::loadSprites() {
    if (!isLoading) {
        isLoading = true;
        loadingThread = std::thread(&SpritesManager::loadData, this);
    }
}

SpritesManager::~SpritesManager()
{
}

inline void SpritesManager::setup() {
    for (int i = 0; i < numSprites; i++) {
        sprites[i] = LoadTextureFromImage(images[i]);
        UnloadImage(images[i]);
    }
    isLoading = false;
}

inline Texture SpritesManager::sprite(int n)
{
    return sprites[n];
}

inline void SpritesManager::unload()
{
    loadingThread.join();
    for (int i = 0; i < numSprites; i++) {
        UnloadTexture(sprites[i]);
    }
    
}

inline void SpritesManager::loadData() {
    for (int i = 0; i < numSprites; i++) {
        images[i] = LoadImage(paths[i].c_str());
        unitLoaded = (float)(i+1)/numSprites;
    }
    isLoading = false;
}
