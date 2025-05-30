#pragma once

enum gunType {NONE, PISTOL, SMG, RIFLE, SHOTGUN, SNIPER, RPG};

void throwGun(Vector2 position, gunType type);
void applyAreaDamage(Vector2 position, float damage, float radius);
void slowMo(float time, float factor);
void drawMinimap();

Shader flashShader;
Shader vignetteShader;

#include "src/camera.h"
#include "src/SpritesManager.h"
#include "src/Particles.h"
#include "src/SoundsManager.h"
#include "src/AnimatedSprite.h"
#include "src/mapLoader.h"
#include "src/Gun.h"
#include "src/HUD.h"
#include "src/Explosives.h"
#include "src/Player.h"
#include "src/Enemy.h"

std::vector<std::unique_ptr<Enemy>> enemies;

#include "src/Ally.h"
#include "src/Collectible.h"
#include "src/Menu.h"
#include "src/LevelMaker.h"

SpritesManager spriteManager;
SoundsManager soundManager;
mapLoader MapLoader = mapLoader();

std::vector<Bullet> bullets;
std::vector<Collectible> collectibles;

Ally ally = Ally({0, 0}, SMG);

Explosives explosives;

Player player = Player();

enum screens {LOADING, MAIN, LEVELSELECT, LEVELMAKER, GAME};

screens currentScene = LOADING;

OverworldParticles overworldParticles({0, 0, 4800, 4800});
LevelMaker lvlMaker;

float slowMoTimer = 0;
float slowMoFactor = 0.5;
float percentLoaded = 0.0f;

RenderTexture2D minimap;

class Game
{
public:

    Game();
    ~Game();
    
    void init();
    void loadLevel(int level);
    void update();
    void draw();

    void unload();
};

Game::Game()
{
}

Game::~Game()
{
}

inline void Game::init() {
    spriteManager.loadSprites();
    soundManager.loadSounds();
    flashShader = LoadShader(0, "shaders/flash.glsl");
    vignetteShader = LoadShader(0, "shaders/vignette.glsl");
}

inline void Game::loadLevel(int level) {
    mapData dat = MapLoader.generateLevel(level);
    printf("Loading seed: %d\n", level);
    player.position = dat.playerPosition;
    ally = Ally(player.position, SMG);
    for (int i = 0; i < dat.numEnemySpawnPositions; i++) {
        if (dat.enemyTypes[i] == 1) enemies.push_back(std::make_unique<ShooterEnemy>(dat.enemySpawnPositions[i], (gunType)GetRandomValue(0, 5)));
        else if (dat.enemyTypes[i] == 2) enemies.push_back(std::make_unique<BoomEnemy>(dat.enemySpawnPositions[i]));
        else enemies.push_back(std::make_unique<WalkerEnemy>(dat.enemySpawnPositions[i]));
    }
    for (int i = 0; i < dat.numCollectibleSpawnPositions; i++) {
        collectibles.push_back(Collectible((CollectibleType)dat.collectibleTypes[i], (int)dat.collectibleData[i], dat.collectiblePositions[i]));
    }
    minimap = dat.minimap;
}

inline void Game::update()
{
    float reqDelta = 0;
    if (slowMoTimer > 0) {
        reqDelta = slowMoFactor * GetFrameTime();
        slowMoTimer -= GetFrameTime();
    } else {
        reqDelta = GetFrameTime();
    }
    delta = reqDelta;

    switch (currentScene)
    {
    case LOADING:
        if (!spriteManager.isLoading && !soundManager.isLoading) {
            spriteManager.setup();
            currentScene = MAIN;
            player.init();
        }
        break;
    case MAIN:
        if (IsKeyPressed(KEY_ENTER)) {
            lvlMaker.newLevel();
            currentScene = LEVELMAKER;
        }
        break;
    case LEVELSELECT:
        break;
    case GAME:
        if (IsKeyPressed(KEY_ENTER)) loadLevel(GetRandomValue(0, 100));
        player.update();
        ally.update();
        for (auto &&enemy : enemies) {
            enemy->update();
        }
        for (auto it = enemies.begin(); it != enemies.end();) {
            if (!(*it)->active) {
                it = enemies.erase(it);
            } else {
                ++ it;
            }
        }
        for (auto &&bullet : bullets) {
            bullet.update();
        }
        for (auto itx = bullets.begin(); itx != bullets.end();) {
            if (!itx->active) {
                itx = bullets.erase(itx);
            } else {
                ++ itx;
            }
        }
        explosives.update();
        overworldParticles.update();

        if (IsKeyPressed(KEY_Z)) Generator::stepChangeFrequency++;
        if (IsKeyPressed(KEY_X)) Generator::stepChangeFrequency--;
        if (IsKeyPressed(KEY_C)) Generator::maxStepsPerTurn++;
        if (IsKeyPressed(KEY_V)) Generator::maxStepsPerTurn--;
        break;
    case LEVELMAKER:
        lvlMaker.update();
        if (IsKeyPressed(KEY_X)) {
            currentScene = MAIN;
        }
        break;
    }
}

inline void Game::draw()
{
    switch (currentScene)
    {
    case LOADING:
        percentLoaded = static_cast<float>(spriteManager.unitLoaded)*50 + static_cast<float>(soundManager.unitLoaded)*50;
        DrawRectangle(0, 0, percentLoaded*SCREENWIDTH, SCREENHEIGHT, Fade(BLACK, 0.3));
        drawMenuBackground("LOADING");
        DrawText(TextFormat("%0f", percentLoaded), 400, 400, 30, MAROON);
        break;
    case MAIN:
        drawMenuBackground("NETHERIUM");
        if(drawButton({SCREENWIDTH - 120, SCREENHEIGHT - 120, 100, 100}, "START", 20, foregroundColour)) currentScene = LEVELSELECT;
        break;
    case LEVELSELECT: {
            drawMenuBackground("SELECT LEVEL");
            int lvl = drawLevelButtons(Rectangle{100, 100, 300, 300}, 10);
            if (lvl != -1) {
                loadLevel(lvl);
                currentScene = GAME;
                HideCursor();
            }
            break;
        }
    case GAME:
        BeginMode2D(player.cam.cam);
        MapLoader.draw();
        for (auto &&bullet : bullets)
        {
            bullet.draw();
        }
        player.draw();
        ally.draw();
        for (auto &&enemy : enemies) {
            enemy->draw();
        }
        explosives.draw();
        collectibles.erase(
            std::remove_if(collectibles.begin(), collectibles.end(), [](auto& collectible) {
                    if (!collectible.active) return true; 
                    collectible.draw();
                    return false;   
                }),
                collectibles.end()
        );
        overworldParticles.draw(Vector2Zero());
        EndMode2D();
        for (auto &&collectible : collectibles) {
            collectible.drawHUD();
        }
        drawMinimap();
        player.drawHUD();
        DrawFPS(10, 10);
        BeginShaderMode(vignetteShader);
        DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, WHITE);
        EndShaderMode();
        //DrawText(TextFormat("RoomFr: %d\nStepsPT: %d\nStepF:%d", Generator::roomFrequency, Generator::maxStepsPerTurn, Generator::stepChangeFrequency), 10, 30, 10, WHITE);
        break;
    case LEVELMAKER:
        lvlMaker.draw();
    }
}

inline void Game::unload()
{
    ShowCursor();
    if (IsRenderTextureValid(minimap)) UnloadRenderTexture(minimap);
    spriteManager.unload();
    soundManager.unload();
    UnloadShader(flashShader);
    UnloadShader(vignetteShader);
}

void throwGun(Vector2 position, gunType type) {
    collectibles.push_back(Collectible(GUN, (int)type, position));
}

// FIX
void applyAreaDamage(Vector2 position, float damage, float radius) {
    slowMo(0.3f, 0.3f);
    for (auto &&enemy : enemies) {
        if (Vector2DistanceSqr(enemy->position, position) < radius*radius) {
            float unitDmg = (1.0f - Vector2Distance(enemy->position, position) / radius);
            Vector2 dir = Vector2Subtract(enemy->position, position);
            dir = Vector2Normalize(dir);
            enemy->applyDamage(unitDmg*damage, dir, unitDmg);
        }
    }
    if (Vector2DistanceSqr(player.position, position) < radius*radius) {
        float unitDmg = (1.0f - Vector2Distance(player.position, position) / radius);
        Vector2 dir = Vector2Subtract(player.position, position);
        dir = Vector2Normalize(dir);
        player.applyDamage(unitDmg * damage, dir);
    }
    if (Vector2DistanceSqr(player.position, position) < 25*radius*radius) {
        player.cam.shakeExplosion((1.0f - Vector2Distance(player.position, position) / radius));
    }
    MapLoader.tileExplosion(position);
}

void slowMo(float time, float factor) {
    slowMoTimer = time;
    slowMoFactor = factor;
}

void drawMinimap() {
    DrawRectangleLines(10, 30, 100, 100, WHITE);
    float startX = std::max({0.0f, player.position.x / tileDrawSize - 8});
    float startY = std::max({0.0f, player.position.y / tileDrawSize - 8});
    float endX = std::min({100.0f, player.position.x / tileDrawSize + 8});
    float endY = std::min({100.0f, player.position.y / tileDrawSize + 8});
    Rectangle srcRec = {startX, minimap.texture.height - endY, endX - startX,  - (endY - startY)};
    Rectangle dstRec = {15, 35, 90, 90};
    DrawTexturePro(minimap.texture, srcRec, dstRec, Vector2Zero(), 0, WHITE);
    DrawCircle(60, 80, 2, foregroundColour);
}