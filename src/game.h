#pragma once

enum gunType {NONE, PISTOL, SMG, RIFLE, SHOTGUN, SNIPER, RPG};

void throwGun(Vector2 position, gunType type);
void applyAreaDamage(Vector2 position, float damage, float radius);
void slowMo(float time, float factor);
void drawMinimap();
void drawGameOverScreen();
void toggleFullscreen();

Shader flashShader;
Shader vignetteShader;
Shader brightFilter;
Shader blurShader;

#include "camera.h"
#include "SpritesManager.h"
#include "Particles.h"
#include "SoundsManager.h"
#include "AnimatedSprite.h"
#include "mapLoader.h"
#include "Explosives.h"
#include "Gun.h"
#include "HUD.h"
#include "Player.h"
#include "Enemy.h"

std::vector<std::unique_ptr<Enemy>> enemies;

#include "Ally.h"
#include "Collectible.h"
#include "Menu.h"
#include "LevelMaker.h"

SpritesManager spriteManager;
SoundsManager soundManager;
mapLoader MapLoader = mapLoader();

std::vector<Bullet> bullets;
std::vector<Collectible> collectibles;

Ally ally = Ally({0, 0}, SMG);
AllySpawner allySpawner = AllySpawner();

Explosives explosives;

Player player;

enum screens {LOADING, MAIN, LEVELSELECT, LEVELMAKER, GAME};

screens currentScene = LOADING;

OverworldParticles overworldParticles({0, 0, 100*tileDrawSize, 100*tileDrawSize});
LevelMaker lvlMaker;

float slowMoTimer = 0;
float slowMoFactor = 0.5;
float percentLoaded = 0.0f;
bool gameWon = false;
bool gameOver = false;

int resolutionLocation = 0;
int blurResolutionLocation = 0;
int blurLocation = 0;

float blurAmount = 0;

RenderTexture2D minimap;
RenderTexture2D mainDrawPass;
RenderTexture2D brightDraws;
RenderTexture2D blurredBrights;

float val = 4;

class Game {
public:

    void init();
    void loadLevel(int level);
    void update();
    void draw();

    void unload();

    Rectangle drawRect = { 0 };
};

inline void Game::init() {
    spriteManager.loadSprites();
    soundManager.loadSounds();
    flashShader = LoadShader(0, "shaders/flash.glsl");
    vignetteShader = LoadShader(0, "shaders/vignette.glsl");
    blurShader = LoadShader(0, "shaders/blur.glsl");
    brightFilter = LoadShader(0, "shaders/brightExtractor.glsl");
    blurLocation = GetShaderLocation(blurShader, "strength");
    int resLoc = GetShaderLocation(blurShader, "resolution");
    Vector2 res = { (float)SCREENWIDTH, (float)SCREENHEIGHT };
    SetShaderValue(blurShader, resLoc, &res, SHADER_UNIFORM_VEC2);
    res = { (float)SCREENWIDTH, (float)SCREENHEIGHT };
    resolutionLocation = GetShaderLocation(vignetteShader, "resolution");
    blurResolutionLocation = GetShaderLocation(blurShader, "resolution");
    //toggleFullscreen();
    mainDrawPass = LoadRenderTexture(SCREENWIDTH, SCREENHEIGHT);
    brightDraws = LoadRenderTexture(SCREENWIDTH, SCREENHEIGHT);
    blurredBrights = LoadRenderTexture(SCREENWIDTH, SCREENHEIGHT);
    SetTextureFilter(blurredBrights.texture, TEXTURE_FILTER_ANISOTROPIC_4X);
}

inline void Game::loadLevel(int level) {
    mapData dat = MapLoader.generateLevel(level);
    printf("Loading seed: %d\n", level);
    player = Player();
    player.init();
    player.position = dat.playerPosition;
    printf("Loading Player at %.2f %.2f\n", dat.playerPosition.x, dat.playerPosition.y);
    player.cam.cam.target = dat.playerPosition;
    player.cam.cam.offset = { SCREENWIDTH/2.0f, SCREENHEIGHT/2.0f };
    player.cam.cam.zoom   = 1.0f;
    drawRect = {0, 0, 0, 0};
    ally = Ally(player.position, (gunType)GetRandomValue(1, 5));
    printf("Placing enemies\n");
    for (int i = 0; i < dat.numEnemySpawnPositions; i++) {
        if (dat.enemyTypes[i] == 1) enemies.push_back(std::make_unique<ShooterEnemy>(dat.enemySpawnPositions[i], (gunType)GetRandomValue(1, 6)));
        else if (dat.enemyTypes[i] == 2) enemies.push_back(std::make_unique<BoomEnemy>(dat.enemySpawnPositions[i]));
        else enemies.push_back(std::make_unique<WalkerEnemy>(dat.enemySpawnPositions[i]));
        if (allySpawner.position == Vector2Zero() && GetRandomValue(0, 100) < 5) allySpawner.position = dat.enemySpawnPositions[i];
    }
    printf("Placing Collectibles\n");
    for (int i = 0; i < dat.numCollectibleSpawnPositions; i++) {
        collectibles.push_back(Collectible((CollectibleType)dat.collectibleTypes[i], (int)dat.collectibleData[i], dat.collectiblePositions[i]));
    }
    minimap = dat.minimap;
    allySpawner.init();
}

inline void Game::update()
{
    if (IsKeyPressed(KEY_F11)) toggleFullscreen();
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
        if (player.dead) {
            gameOver = true;
            ShowCursor();
        }
        player.update();
        blurAmount = Clamp(player.cam.getTrauma() * 5 + player.visionBlur, 0, 4.0);
        allySpawner.update();
        ally.update();
        for (auto &&enemy : enemies) {
            enemy->update();
        }
        gameWon = enemies.size() == 0;
        if (gameWon) {
            gameOver = true;
            ShowCursor();
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
        drawRect = player.cam.getViewRect();
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
    switch (currentScene) {
    case LOADING:
        percentLoaded = static_cast<float>(spriteManager.unitLoaded)*50 + static_cast<float>(soundManager.unitLoaded)*50;
        DrawRectangle(0, 0, percentLoaded*SCREENWIDTH/100, SCREENHEIGHT, Fade(foregroundColour, 0.1));
        drawMenuBackground("LOADING");
        DrawText(TextFormat("%.0f", percentLoaded), SCREENWIDTH/2 - MeasureText(TextFormat("%.0f", percentLoaded), 30)/2, SCREENHEIGHT/2 - 15, 30, foregroundColour);
        break;
    case MAIN:
        drawMenuBackground("NULLZONE");
        if(drawButton({SCREENWIDTH - 120.0f, SCREENHEIGHT - 120.0f, 100, 100}, "START", 20, foregroundColour)) currentScene = LEVELSELECT;
        break;
    case LEVELSELECT: {
            drawMenuBackground("SELECT LEVEL");
            int width = MeasureText("Press ENTER to play random seed", 20);
            DrawText("Press ENTER to play random seed", SCREENWIDTH / 2 - width / 2, SCREENHEIGHT - 30, 20, WHITE);
            int lvl = drawLevelButtons(Rectangle{450, SCREENHEIGHT / 2.0f - 110.0f, 800, 400}, 10);
            if (IsKeyPressed(KEY_ENTER)) lvl = GetRandomValue(100, 1000);
            if (lvl != -1) {
                collectibles.clear();
                enemies.clear();
                loadLevel(lvl);
                gameOver = false;
                gameWon = false;
                currentScene = GAME;
                HideCursor();
            }
            break;
        }
    case GAME:
        BeginTextureMode(mainDrawPass);
        BeginMode2D(player.cam.cam);
        MapLoader.draw(drawRect);
        for (auto &&bullet : bullets)
        {
            bullet.draw();
        }
        player.draw();
        ally.draw();
        allySpawner.draw();
        for (auto &&enemy : enemies) {
            if (!CheckCollisionPointRec(enemy->position, drawRect)) continue;
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
        EndTextureMode();
        BeginTextureMode(brightDraws);
        ClearBackground(BLANK);
        BeginShaderMode(brightFilter);
        DrawTexturePro(mainDrawPass.texture, {0, 0, SCREENWIDTH, SCREENHEIGHT}, {0, 0, (float)SCREENWIDTH, (float)SCREENHEIGHT}, {0, 0}, 0, WHITE);
        EndShaderMode();
        EndTextureMode();
        SetShaderValue(blurShader, blurLocation, &val, SHADER_UNIFORM_FLOAT);
        BeginTextureMode(blurredBrights);
        ClearBackground(BLANK);
        BeginShaderMode(blurShader);
        DrawTexturePro(brightDraws.texture, {0, 0, SCREENWIDTH, SCREENHEIGHT}, {0, 0, (float)SCREENWIDTH, (float)SCREENHEIGHT}, {0, 0}, 0, WHITE);
        EndShaderMode();
        EndTextureMode();
        SetShaderValue(blurShader, blurLocation, &blurAmount, SHADER_UNIFORM_FLOAT);
        BeginShaderMode(blurShader);
        DrawTexturePro(mainDrawPass.texture, {0, 0, SCREENWIDTH, -SCREENHEIGHT}, {0, 0, (float)SCREENWIDTH, (float)SCREENHEIGHT}, {0, 0}, 0, WHITE);
        EndShaderMode();
        BeginBlendMode(BLEND_ADDITIVE);
        DrawTexturePro(blurredBrights.texture, {0, 0, blurredBrights.texture.width, -blurredBrights.texture.height}, {0, 0, (float)SCREENWIDTH, (float)SCREENHEIGHT}, {0, 0}, 0, Fade(WHITE, 0.9));
        EndBlendMode();
        drawMinimap();
        player.drawHUD();
        DrawFPS(10, 10);
        BeginShaderMode(vignetteShader);
        DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, WHITE);
        EndShaderMode();
        if (gameOver) {
            drawGameOverScreen();
        }
        break;
    case LEVELMAKER:
        lvlMaker.draw();
        break;
    }
}

inline void Game::unload()
{
    ShowCursor();
    if (IsRenderTextureValid(minimap)) UnloadRenderTexture(minimap);
    UnloadRenderTexture(mainDrawPass);
    UnloadRenderTexture(brightDraws);
    UnloadRenderTexture(blurredBrights);
    spriteManager.unload();
    soundManager.unload();
    UnloadShader(flashShader);
    UnloadShader(vignetteShader);
    UnloadShader(blurShader);
    UnloadShader(brightFilter);
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
        player.cam.shakeExplosion(Vector2Distance(player.position, position));
        float distance = Vector2DistanceSqr(player.position, position);
        distance = Clamp(distance / 22500, 0, 0.8);
        SetSoundVolume(soundManager.getSound(6), 1 - distance);
        soundManager.playSoundWithVarPitch(6);
    }
    MapLoader.tileExplosion(position);
}

void slowMo(float time, float factor) {
    slowMoTimer = time;
    slowMoFactor = factor;
}

void drawMinimap() {
    DrawRectangleLines(10, SCREENHEIGHT - 110, 100, 100, WHITE);
    float startX = std::max(0.0f, player.position.x / tileDrawSize - 8);
    float startY = std::max(0.0f, player.position.y / tileDrawSize - 8);
    float endX = std::min(100.0f, player.position.x / tileDrawSize + 8);
    float endY = std::min(100.0f, player.position.y / tileDrawSize + 8);
    Rectangle srcRec = {startX, minimap.texture.height - endY, endX - startX,  - (endY - startY)};
    Rectangle dstRec = {15, SCREENHEIGHT - 105.0f, 90, 90};
    DrawTexturePro(minimap.texture, srcRec, dstRec, Vector2Zero(), 0, WHITE);
    DrawCircle(60, SCREENHEIGHT - 60, 2, foregroundColour);
}

void drawGameOverScreen() {
    DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, Fade(BLACK, 0.7));
    if (gameWon) {
        DrawText("LEVEL CLEARED", SCREENWIDTH/2 - MeasureText("LEVEL CLEARED", 60)/2, SCREENHEIGHT/2 - 70, 60, foregroundColour);
        DrawText("Press E to return", SCREENWIDTH/2 - MeasureText("Press E to return", 30)/2, SCREENHEIGHT/2, 30, WHITE);
    } else {
        DrawText("GAME OVER", SCREENWIDTH/2 - MeasureText("GAME OVER", 60)/2, SCREENHEIGHT/2 - 70, 60, foregroundColour);
        DrawText("Press E to retry", SCREENWIDTH/2 - MeasureText("Press E to retry", 30)/2, SCREENHEIGHT/2, 30, WHITE);
    }
    if (IsKeyPressed(KEY_E)) {
        currentScene = LEVELSELECT;
    }
}

inline void toggleFullscreen() {
    if (IsWindowFullscreen()) {
        SCREENWIDTH = 1200;
        SCREENHEIGHT = 800;
        ToggleFullscreen();
        SetWindowSize(SCREENWIDTH, SCREENHEIGHT);
        Vector2 res = {SCREENWIDTH, SCREENHEIGHT};
        SetShaderValue(vignetteShader, resolutionLocation, &res, SHADER_UNIFORM_VEC2);
        SetShaderValue(blurShader, blurResolutionLocation, &res, SHADER_UNIFORM_VEC2);
    } else {
        SCREENWIDTH = GetMonitorWidth(0);
        SCREENHEIGHT = GetMonitorHeight(0);
        SetWindowSize(SCREENWIDTH, SCREENHEIGHT);
        ToggleFullscreen();
        Vector2 res = {SCREENWIDTH, SCREENHEIGHT};
        SetShaderValue(vignetteShader, resolutionLocation, &res, SHADER_UNIFORM_VEC2);
        SetShaderValue(blurShader, blurResolutionLocation, &res, SHADER_UNIFORM_VEC2);
    }
}
