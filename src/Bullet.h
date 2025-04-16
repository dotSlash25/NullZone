#pragma once

extern mapLoader MapLoader;

class Bullet
{
private:
    float speed = 900;
    float angle = 0.0f;
    void loadData(int x);
    float firetime = 0;

    Rectangle sprRect = { 0 };
    float scale = 1.0f;

public:
    bool active = true;
    bool shotByEnemy = false;
    Vector2 position = { 0 };
    Vector2 velocity = { 0 };
    Bullet(int type, float shootAngle);
    ~Bullet();

    Rectangle collider = { 0 };
    
    float damage = 0.0f;

    void disable();

    void update();
    void draw();
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
}

inline void Bullet::update()
{
    if (!active) return;
    collider = {position.x -2 , position.y -2, 4, 4};
    active = !MapLoader.checkCollisions(collider);
    if (!active) return;
    velocity = Vector2Scale(Vector2{cos(angle), sin(angle)}, speed*delta);
    position = Vector2Add(velocity, position);
    firetime += delta;
    if (firetime > 2) active = false;
}

inline void Bullet::draw()
{
    if (!active) return;
    Texture tex = spriteManager.sprite(2);
    Rectangle src = Rectangle{0,0,8,4};
    Rectangle dst = {position.x, position.y-src.height/2*scale, src.width*scale*2, src.height*scale};
    Vector2 org = {sprRect.width*scale/2, sprRect.height*scale/2};
    DrawTexturePro(tex, src, dst, org, angle*RAD2DEG, WHITE);
}

inline void Bullet::loadData(int x) {
    
}
