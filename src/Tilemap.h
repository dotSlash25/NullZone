#pragma once
#include <iostream>

class Tilemap
{
private:
    int tileSetId;
    Vector2 tileSize;
    int data[50][50];
public:
    Tilemap(int TileSetID, Vector2 TileSize);
    ~Tilemap();
    void setTile(Vector2 position, int tile);
    int getTile(Vector2 position);
    void draw();
    void clear();
};

Tilemap::Tilemap(int TileSetID, Vector2 TileSize)
{
    tileSetId = TileSetID;
    tileSize = TileSize;
}

Tilemap::~Tilemap()
{
}

inline void Tilemap::setTile(Vector2 position, int tile)
{
    data[(int)position.x][(int)position.y] = tile;
}

inline int Tilemap::getTile(Vector2 position)
{
    return data[(int)position.x][(int)position.y];
}

inline void Tilemap::draw()
{
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            float delX = data[i][j]%4;
            float delY = (float)((int)data[i][j]/4);
            DrawTexturePro(spriteManager.sprite(tileSetId), Rectangle{delX*tileSize.x, delY*tileSize.y, tileSize.x, tileSize.y}, Rectangle{(float)i*tileDrawSize, (float)j*tileDrawSize, tileDrawSize, tileDrawSize}, Vector2Zero(), 0.0f, WHITE);
            //DrawTextureRec(spriteManager.sprite(tileSetId), Rectangle{data[i][j]*tileSize.x, 0, tileSize.x, tileSize.y}, Vector2{i*tileSize.x, j*tileSize.y}, WHITE);
        }
        
    }
}

inline void Tilemap::clear() {
    for (short i = 0; i < 50; i++) {
        for (short j = 0; j < 50; j++) {
            data[i][j] = GetRandomValue(0, 3);
        }
    }
}
