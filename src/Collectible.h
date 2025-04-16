#pragma once

enum CollectibleType {GUN, AMMO, MEDIKIT, COIN};

class Collectible
{
private:
    int spriteID;

    float animTimer = GetRandomValue(0, 100) / 100.0f;
public:
    bool active = true;
    Rectangle spriteRect;
    gunType gType = PISTOL;
    int ammoCount = 0;
    float healAmount = 0;
    int coinValue = 0;

    float drawScale = 3;

    Vector2 position;

    CollectibleType type = GUN;
    Collectible(CollectibleType t, int dat, Vector2 pos);
    ~Collectible();

    void loadData(CollectibleType t, int k);

    void draw();
};

Collectible::Collectible(CollectibleType t, int dat, Vector2 pos)
{
    loadData(t, dat);
    type = t;
    position = pos;
}

Collectible::~Collectible()
{
}

inline void Collectible::loadData(CollectibleType t, int k)
{
    switch (t)
    {
    case GUN:
        gType = (gunType)(k);
        spriteID = 1;
        spriteRect = gunSizes[k-1];
        break;
    case AMMO:
        ammoCount = k;
        spriteID = 4;
        spriteRect = {16, 0, 16, 16};
        break;
    case MEDIKIT:
        healAmount = k;
        spriteID = 4;
        spriteRect = {0, 0, 16, 16};
        break;
    default:
        coinValue = k;
        spriteID = 4;
        spriteRect = {32, 0, 16, 16};
        break;
    }
}

inline void Collectible::draw()
{
    animTimer += delta;
    if (animTimer > 2) animTimer = 0;
    Rectangle dstRect = {position.x, position.y, spriteRect.width*drawScale, spriteRect.height*drawScale};
    Vector2 origin = {spriteRect.height*drawScale/2, spriteRect.height*drawScale/2};
    DrawTexturePro(spriteManager.sprite(spriteID), spriteRect, dstRect, origin, 0, Fade(BLACK, 0.5));
    dstRect.y += -10.0f + 5.0f*sin(2 * animTimer/2.0f * PI);
    DrawTexturePro(spriteManager.sprite(spriteID), spriteRect, dstRect, origin, 0, WHITE);
    if (Vector2DistanceSqr(position, player.position) < 400*(drawScale*drawScale))
    {
        DrawText("Press E", position.x + spriteRect.width/2 - MeasureText("Press E", 10)/2, position.y + 2 + spriteRect.height, 10, BLACK);
        if (IsKeyPressed(KEY_E)) {
            active = false;
            switch (type)
            {
            case GUN:
                player.gun = Gun(gType);
                break;
            case AMMO:
                player.gun.clips += ammoCount;
                break;
            case MEDIKIT:
                player.health += healAmount;
                if (player.health > 100) player.health = 100;
            default:
                break;
            }
        }
    }
    
}
