#pragma once

enum CollectibleType {GUN, AMMO, MEDIKIT, CHARGE};

class Collectible
{
private:
    int spriteID;
    bool hudActive = false;
    float hudAnimTimer = 0;
    float hudTimePassed = 0;
    float animTimer = GetRandomValue(0, 100) / 100.0f;
public:
    bool active = true;
    Rectangle spriteRect;
    gunType gType = PISTOL;
    int ammoCount = 0;
    float healAmount = 0;

    float drawScale = 3;

    Vector2 position;

    CollectibleType type = GUN;
    Collectible(CollectibleType t, int dat, Vector2 pos);
    ~Collectible();

    void loadData(CollectibleType t, int k);

    void draw();
    void drawHUD();
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
        if (k == 0) active = false;
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
        spriteID = 4;
        spriteRect = {32, 0, 16, 16};
        break;
    }
}

inline void Collectible::draw()
{
    if (!CheckCollisionPointRec(position, player.cam.getViewRect())) return;
    animTimer += delta;
    if (animTimer > 2) animTimer = 0;
    Rectangle dstRect = {position.x, position.y, spriteRect.width*drawScale, spriteRect.height*drawScale};
    Vector2 origin = {spriteRect.width*drawScale/2, spriteRect.height*drawScale/2};
    DrawTexturePro(spriteManager.sprite(spriteID), spriteRect, dstRect, origin, 0, Fade(BLACK, 0.5));
    dstRect.y += -10.0f + 5.0f*sin(2 * animTimer/2.0f * PI);
    DrawTexturePro(spriteManager.sprite(spriteID), spriteRect, dstRect, origin, 0, WHITE);
    if (Vector2DistanceSqr(position, player.position) < 400*(drawScale*drawScale))
    {
        if (type == GUN) {
            hudActive = true;
        }
        int w = 7;
        DrawRectangleLines(position.x - w, position.y + spriteRect.height + 10, 2*w, 2*w, WHITE);
        DrawText("F", position.x - MeasureText("F", 10) / 2, position.y + spriteRect.height + 10 + (2*w - 10) / 2, 10, WHITE);
        if (IsKeyPressed(KEY_F)) {
            active = false;
            switch (type) {
            case GUN: {
                player.gun = Gun(gType);
                float scope = GunData[(int)gType - 1].scope;
                player.cam.setZoom(scope);
                break;
            }
            case AMMO:
                player.gun.ammoPickup();
                break;
            case MEDIKIT:
                player.health += healAmount;
                if (player.health > 100) player.health = 100;
                break;
            case CHARGE:
                player.charges++;
                break;
            default:
                break;
            }
        }
    } else {
        hudActive = false;
    }
}

inline void Collectible::drawHUD() {
    if (!hudActive) return;
    int txtLen = MeasureText(gunNames[(int)gType].c_str(), 20);
    int x = SCREENWIDTH - txtLen - 30;
    DrawRectangle(x, 10, txtLen + 20, 30, foregroundColour);
    DrawText(gunNames[(int)gType].c_str(), x + 10, 15, 20, backgroundColour);
}
