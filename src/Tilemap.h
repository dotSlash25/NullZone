#pragma once
#include <iostream>

class Tilemap
{
private:
    int tileSetId;
    Vector2 tileSize;
    int data[100][100];
public:
    Tilemap(int TileSetID, Vector2 TileSize);
    ~Tilemap();
    void setTile(Vector2 position, int tile);
    int getTile(Vector2 position);
    void draw(Rectangle camRect);
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

inline void Tilemap::draw() {
    draw({0, 0, 100*tileDrawSize, 100*tileDrawSize});
}

inline void Tilemap::draw(Rectangle camRect)
{
    int startX = std::max(0.0f, camRect.x / tileDrawSize);
    int startY = std::max(0.0f, camRect.y / tileDrawSize);
    int endX = std::min(99.0f, (camRect.x + camRect.width) / tileDrawSize);
    int endY = std::min(99.0f, (camRect.y + camRect.height) / tileDrawSize);

    for (int i = startX; i <= endX; i++)
    {
        for (int j = startY; j <= endY; j++)
        {
            float delX = data[i][j]%4;
            float delY = (float)((int)data[i][j]/4);
            DrawTexturePro(spriteManager.sprite(tileSetId), Rectangle{delX*tileSize.x, delY*tileSize.y, tileSize.x, tileSize.y}, Rectangle{(float)i*tileDrawSize, (float)j*tileDrawSize, tileDrawSize, tileDrawSize}, Vector2Zero(), 0.0f, WHITE);
        }

    }
}

inline void Tilemap::clear() {
    for (short i = 0; i < 100; i++) {
        for (short j = 0; j < 100; j++) {
            data[i][j] = GetRandomValue(0, 3);
        }
    }
}
