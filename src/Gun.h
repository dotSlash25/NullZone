#pragma once
#include "math.h"

#include "src/Bullet.h"

extern std::vector<Bullet> bullets;
//extern SoundsManager soundManager;

class Gun {
private:
    float firerate = 0;
    float reloadTime = 0;

    float damage = 0;

    float inaccuracy = 0;
    int spreadNumber = 1;
    float spreadAngle = 15*DEG2RAD;
    int burstNumber = 1;
    float burstDelay = 0;

    float knockbackAngle = 0;
    Vector2 offset = {0, 0};


    void loadData(gunType t);

    float lastFired = 0;


    void shoot(bool isEnemy);

    int spriteFlip = 1;
    bool justShot = false;

    
    Particles shootParticles = Particles();
public:
    Gun(gunType GunType);
    Gun();
    gunType type;
    ~Gun();
    
    Vector2 position;
    bool active;
    Rectangle srcTexture;
    
    int numbullets;
    int bulletsIn;
    int clips;

    bool automatic = false;
    float kickback = 2;
    float rotation = 0;
    float range = 40;
    float scope = 1.0f;

    bool reloading = false;
    float reloadingTime = 0;

    bool fire(bool isEnemy);
    void update(Vector2 aim, Vector2 pos);
    void draw();
    void reload();
    void ammoPickup();

};

inline void Gun::loadData(gunType t)
{
    if (t == NONE) return;
    shootParticles.oneShot = true;
    shootParticles.initialVelocity = 100;
    shootParticles.spread = 1.5f;
    shootParticles.lifetime = 0.2f;
    
    active = true;
    srcTexture = gunSizes[t-1];
    gunData dat = GunData[t-1];
    numbullets = dat.numbullets;
    bulletsIn = numbullets;
    clips = dat.clips;
    damage = dat.damage;
    firerate = dat.firerate;
    reloadTime = dat.reloadTime;
    automatic = dat.automatic;
    inaccuracy = dat.inaccuracy;
    kickback = dat.kickback;
    spreadNumber = dat.spreadNumber;
    spreadAngle = dat.spreadAngle;
    scope = dat.scope;
    range = dat.range;
    shootParticles.spread = 15*DEG2RAD;
}

inline void Gun::shoot(bool isEnemy)
{
    for (int i = 0; i < spreadNumber; i++)
    {
        float angle = rotation + (i*spreadAngle) - ((spreadNumber - 1) * (spreadAngle/2));
        angle += GetRandomValue(-100, 100)/100.0f * inaccuracy;
        Bullet b = Bullet(type, angle);
        b.shotByEnemy = isEnemy;
        b.range = range;
        b.damage = damage;
        b.position = Vector2Add(position, Vector2Rotate(Vector2{srcTexture.width*5.4f, 0}, angle));
        bullets.push_back(b);
        lastFired = 0;
    }

    Vector2 del = Vector2{cos(rotation), sin(rotation)};
    del = Vector2Scale(del, kickback);
    offset = Vector2Subtract(offset, del);
    offset = Vector2Scale(offset, 2);
    offset = Vector2ClampValue(offset, -15, 15);
    knockbackAngle -= 2*kickback *DEG2RAD* del.x;

    shootParticles.position = Vector2Add(position, Vector2Rotate(Vector2{srcTexture.width*5.4f, 0}, rotation));
    shootParticles.direction = del;
    shootParticles.start();
    soundManager.playSound((int)type - 1);
    
    justShot = true;
}

Gun::Gun(gunType GunType)
{
    type = GunType;
    loadData(type);
}

inline Gun::Gun()
{
    active = false;
}

Gun::~Gun()
{
}

inline bool Gun::fire(bool isEnemy)
{
    if (type == NONE) return false;
    if (lastFired > firerate and bulletsIn > 0 and !reloading)
    {
        bulletsIn--;
        shoot(isEnemy);
        return true;
    }
    return false;
}

inline void Gun::update(Vector2 aim, Vector2 pos)
{
    if(!active or type == NONE) return;
    position = pos;
    Vector2 del = Vector2Subtract(aim, position);
    float mouseAim = atan2(del.y, del.x);
    rotation = lerpAngle(rotation, mouseAim, 1);
    offset = lerp(offset, Vector2Zero(), 1);
    position = Vector2Add(position, offset);
    rotation += knockbackAngle;

    knockbackAngle = lerp(knockbackAngle, 0, 12);
    lastFired += delta;

    if (position.x > aim.x) spriteFlip = -1;
    else spriteFlip = 1;

    if (reloading) {
        reloadingTime += delta;
        if (reloadingTime >= reloadTime) {
            reloadingTime = 0;
            reloading = false;
        }
    }

    
    shootParticles.update();
}

inline void Gun::draw()
{
    if(!active || type == NONE) return;
    srcTexture.height = spriteFlip*abs(srcTexture.height);
    float scale = 5.4f;
    Vector2 org = {1*scale, 2*scale};
    Rectangle dst = {position.x, position.y, srcTexture.width*scale, abs(srcTexture.height)*scale};
    DrawTexturePro(spriteManager.sprite(1), srcTexture, dst, org, rotation*RAD2DEG, WHITE);
    if (reloading) {
        DrawRing(Vector2Add(position, {0, 30}), 8, 10, 0, reloadingTime/reloadTime * 360.0f, 64, WHITE);
    }
    /*if (justShot)
    {
        Texture tex = spriteManager.sprite(4);
        Vector2 p = Vector2Add(position, Vector2Rotate(Vector2{srcTexture.width*3, 0}, rotation));
        Rectangle src = Rectangle{0,0,13,9};
        Rectangle dst = {p.x, p.y-src.height/2*scale, src.width*scale, src.height*scale};
        Vector2 org = {0, 0};
        DrawTexturePro(tex, src, dst, org, rotation*RAD2DEG, WHITE);
        justShot = false;
    }*/ 
    
    shootParticles.draw();
}

inline void Gun::reload() {
    reloading = true;
    int del = numbullets - bulletsIn;
    if (clips >= del) {
        clips -= del;
        bulletsIn =  numbullets;
    } else {
        bulletsIn = clips;
        clips = 0;
    }
}

inline void Gun::ammoPickup() {
    clips = GunData[type - 1].clips;
}