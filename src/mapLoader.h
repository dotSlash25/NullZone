#pragma once

#include "Tilemap.h"

//todo: add map info like steps per turn etc

typedef struct mapData {
    short numEnemySpawnPositions;
    Vector2 enemySpawnPositions[32];
    short enemyTypes[32];
    short numCollectibleSpawnPositions;
    Vector2 collectiblePositions[32];
    short collectibleTypes[32];
    short collectibleData[32];
    Vector2 playerPosition;
} mapData;

#include "proceduralGenerator.h"

typedef struct CollisionInfo {
    Rectangle finalCollider;
    Vector2 repulsiveDirection;
} CollisionInfo;

bool colourEquals(Color c1, Color c2) {
    return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a);
}

class mapLoader
{
private:
    Tilemap tiles = Tilemap(0, Vector2{16.0f, 16.0f});
public:
    mapLoader();
    ~mapLoader();
    mapData loadMap(int level);
    mapData generateLevel(int seed);
    void draw();

    Rectangle checkCollisions(Rectangle rec, Vector2 velo);
    CollisionInfo checkCollisionsInfo(Rectangle rec, Vector2 velo);
    bool checkCollisions(Rectangle rec);
    bool rayCast(Vector2 position, Vector2 target);
};

mapLoader::mapLoader()
{
}

mapLoader::~mapLoader()
{
}

inline mapData mapLoader::loadMap(int level)
{    
    std::string fileName = "levels/lvl_" + std::to_string(level) + ".png";
    Image img = LoadImage(fileName.c_str());

    mapData data = { 0 };
    
    for (int i = 0; i < img.height; i++)
    {
        for (int j = 0; j < img.width; j++)
        {
            Color c = GetImageColor(img, j, i);
            if (colourEquals(c, Color{0,0,0,255})) {
                tiles.setTile(Vector2{(float)j, (float)i}, 0);
            } else {
                tiles.setTile(Vector2{(float)j, (float)i}, 1);
            }
            if (c.r == 255) {
                data.enemyTypes[data.numEnemySpawnPositions] = c.g;
                data.enemySpawnPositions[data.numEnemySpawnPositions] = Vector2{j*1.0f*tileDrawSize + tileDrawSize/2, i*1.0f*tileDrawSize + tileDrawSize/2};
                data.numEnemySpawnPositions++;
            }
            if (c.b == 255) {
                data.collectibleTypes[data.numCollectibleSpawnPositions] = c.g;
                data.collectibleData[data.numCollectibleSpawnPositions] = c.r;
                data.collectiblePositions[data.numCollectibleSpawnPositions] = Vector2{j*1.0f*tileDrawSize + tileDrawSize/2, i*1.0f*tileDrawSize + tileDrawSize/2};
                data.numCollectibleSpawnPositions++;
            }
            if (c.g == 255) {
                data.playerPosition = Vector2{j*1.0f*tileDrawSize + tileDrawSize/2, i*1.0f*tileDrawSize + tileDrawSize/2};
            }
        }
        
    }
    UnloadImage(img);
    return data;
}

inline void mapLoader::draw()
{
    tiles.draw();
}

inline Rectangle mapLoader::checkCollisions(Rectangle rec, Vector2 velo)
{
    velo = Vector2Scale(velo, delta);
    int x = (int)rec.x/tileDrawSize;
    int y = (int)rec.y/tileDrawSize;
    rec.x += velo.x;
    for (int i = x-1; i < x+2; i++)
    {
        for (int j = y-1; j < y+2; j++)
        {
            if (tiles.getTile(Vector2{(float)i, (float)j}) <= 3)
            {
                Rectangle tile = Rectangle{i*tileDrawSize, j*tileDrawSize, tileDrawSize, tileDrawSize};
                if (CheckCollisionRecs(rec, tile))
                {
                    if(velo.x > 0) rec.x = tile.x - rec.width;
                    else rec.x = tile.x + tile.width;
                }                
            }
            
        }
        
    }
    rec.y += velo.y;
    for (int i = x-1; i < x+2; i++)
    {
        for (int j = y-1; j < y+2; j++)
        {
            if (tiles.getTile(Vector2{(float)i, (float)j}) <= 3)
            {
                Rectangle tile = Rectangle{i*tileDrawSize, j*tileDrawSize, tileDrawSize, tileDrawSize};
                if (CheckCollisionRecs(rec, tile))
                {
                    if(velo.y > 0) rec.y = tile.y - rec.height;
                    else rec.y = tile.y + tile.height;
                }                
            }
            
        }
        
    }
    return rec;
}

inline bool mapLoader::checkCollisions(Rectangle rec)
{
    bool colliding = false;
    int x = (int)rec.x/tileDrawSize;
    int y = (int)rec.y/tileDrawSize;
    for (int i = x-1; i < x+2; i++)
    {
        for (int j = y-1; j < y+2; j++)
        {
            if (tiles.getTile(Vector2{(float)i, (float)j}) <= 3)
            {
                Rectangle tile = Rectangle{i*tileDrawSize, j*tileDrawSize, tileDrawSize, tileDrawSize};
                if (CheckCollisionRecs(rec, tile))
                {
                    colliding = true;
                }                
            }
            
        }
        
    }
    return colliding;
}

inline bool mapLoader::rayCast(Vector2 position, Vector2 target) {
    float distanceTravelled = 0;
    float stepDistance = 5;
    bool found = false;
    Vector2 scanPos = position;
    if (Vector2DistanceSqr(scanPos, target) < 25) {
        found = true;
    }
    while (!found && distanceTravelled < 800) {
        scanPos = Vector2MoveTowards(scanPos, target, stepDistance);
        if (checkCollisions({scanPos.x, scanPos.y, 5, 5})) break;
        distanceTravelled += stepDistance;
        if (Vector2DistanceSqr(scanPos, target) < 25) {
            found = true;
        }
    }
    return found;
}

inline CollisionInfo mapLoader::checkCollisionsInfo(Rectangle rec, Vector2 velo) {
    velo = Vector2Scale(velo, delta);
    int x = (int)rec.x/tileDrawSize;
    int y = (int)rec.y/tileDrawSize;
    rec.x += velo.x;
    Vector2 repl = { 0 };
    for (int i = x-1; i < x+2; i++)
    {
        for (int j = y-1; j < y+2; j++)
        {
            if (tiles.getTile(Vector2{(float)i, (float)j}) <= 3)
            {
                Rectangle tile = Rectangle{i*tileDrawSize, j*tileDrawSize, tileDrawSize, tileDrawSize};
                if (CheckCollisionRecs(rec, tile))
                {
                    repl.x = (rec.x + rec.width)/2 - (tile.x + tile.width)/2;
                    repl.y = (rec.y + rec.height)/2 - (tile.y + tile.height)/2;
                    
                    if(velo.x > 0) rec.x = tile.x - rec.width;
                    else rec.x = tile.x + tile.width;
                }                
            }
            
        }
        
    }
    rec.y += velo.y;
    for (int i = x-1; i < x+2; i++)
    {
        for (int j = y-1; j < y+2; j++)
        {
            if (tiles.getTile(Vector2{(float)i, (float)j}) <= 3)
            {
                Rectangle tile = Rectangle{i*tileDrawSize, j*tileDrawSize, tileDrawSize, tileDrawSize};
                if (CheckCollisionRecs(rec, tile))
                {
                    repl.x = (rec.x + rec.width)/2 - (tile.x + tile.width)/2;
                    repl.y = (rec.y + rec.height)/2 - (tile.y + tile.height)/2;
                    
                    if(velo.y > 0) rec.y = tile.y - rec.height;
                    else rec.y = tile.y + tile.height;
                }                
            }
            
        }
        
    }
    return {rec, repl};
}

inline mapData mapLoader::generateLevel(int seed) {
    return Generator::generateLevel(seed, &tiles);
}