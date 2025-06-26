#pragma once

extern mapLoader MapLoader;

class Bullet
{
private:
    float speed = 900;
    float angle = 0.0f;
    int spriteID = 2;
    Rectangle srcRect = {0, 0, 8, 4};
    void loadData(int x);

    Rectangle sprRect = { 0 };
    float scale = 1.0f;
    float anglePhi = 0;

public:
    bool active = true;
    float range;
    bool shotByEnemy = false;
    Vector2 position = { 0 };
    Vector2 velocity = { 0 };
    bool explosive = false;
    Bullet(int type, float shootAngle);
    ~Bullet();

    Rectangle collider = { 0 };
    
    float damage = 0.0f;

    void disable();

    void update();
    void draw();
    void setExplosive();
};

Bullet::Bullet(int type, float shootAngle)
{
    loadData(type);
    angle = shootAngle;
    active = true;
}

Bullet::~Bullet()
{
    
}

inline void Bullet::disable()
{
    active = false;
    if (this->explosive) explosives.addExplosive(position, { 0 }, 0);
}

inline void Bullet::update()
{
    if (!active) return;
    collider = {position.x -2 , position.y -2, 4, 4};
    bool colliding = MapLoader.checkCollisions(collider);
    if (colliding) {
        disable();
        return;
    }
    velocity = Vector2Scale(Vector2{cos(angle), sin(angle)}, speed*delta);
    position = Vector2Add(velocity, position);
    range -= speed*delta;
    if (range <= 0) disable();
}

inline void Bullet::draw()
{
    if (!active) return;
    Texture tex = spriteManager.sprite(this->spriteID);
    Rectangle src = this->srcRect;
    Rectangle dst = {position.x, position.y-src.height/2*scale, src.width*scale*2, src.height*scale};
    if (explosive) dst.width /= 2;
    Vector2 org = {sprRect.width*scale/2, sprRect.height*scale/2};
    DrawTexturePro(tex, src, dst, org, angle*RAD2DEG - this->anglePhi, WHITE);
}

inline void Bullet::loadData(int x) {
    
}

inline void Bullet::setExplosive() {
    this->explosive = true;
    this->speed = 450;
    this->spriteID = 9;
    this->srcRect = {0, 0, 6, 6};
    this->scale = 6.0f;
    this->anglePhi = 45.0f;
}
