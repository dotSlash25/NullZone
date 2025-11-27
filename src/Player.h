#pragma once

extern mapLoader MapLoader;
extern Explosives explosives;

typedef struct breadCrumb {
    Vector2 position;
    bool active;
    float timePassed;
} breadCrumb;

class Player {
private:
    int width = 0;
    int height = 0;

    Vector2 aimDel = { 0 };

    Rectangle collider = Rectangle{0, 0, 40, 40};
    float maxSpeed = 300;
    bool dashing = 0;
    Vector2 dashVelocity;

    AnimatedSprite sprite = AnimatedSprite(3, 3, (int[]){8, 8, 8}, 10);

    float lastBreadCrumbTime = 0;
    bool justHit = false;
    float hitTimer = 0;
    HUD hud;

    float explosivePrimeTimer = 0;
    bool explosivePrimed = false;

    Vector2 spriteSize = { 0 };

    Vector2 getGlobalMousePosition();
    void updateBreadcrumbs();
    void updateGun();
    void handleInput();
    void handleMovement();
public:
    Player();
    ~Player();
    Vector2 position = { 0 };
    Vector2 velocity = { 0 };
    Vector2 knockback = { 0 };
    camera cam = camera();
    Gun gun = Gun(PISTOL);

    int charges = 3;
    float health = 100;
    bool dead = false;
    float visionBlur = 0;

    void update();
    void draw();
    void init();
    void updateHUD();
    void drawHUD();
    void fire();
    void reset();
    void applyDamage(float damage, Vector2 direction);

    std::vector<breadCrumb> breadCrumbs;
};

inline Vector2 Player::getGlobalMousePosition() {
    return GetScreenToWorld2D(GetMousePosition(), cam.cam);
}

Player::Player() {
    charges = 3;
}

Player::~Player() {
}

inline void Player::init() {
    cam.init();
    sprite.refresh();
    sprite.scale = 5;
    spriteSize = sprite.getSize();
}

inline void Player::update() {
    if (dead) {
        sprite.playOnce(2);
        sprite.update();
        velocity = lerp(velocity, Vector2Zero(), 2);
    } else {
        handleInput();
        updateBreadcrumbs();
        updateGun();
        handleMovement();
        visionBlur = lerp(visionBlur, 0, 1.5f);
    }
    if (justHit) {
        hitTimer -= delta;
        if (hitTimer <= 0) {
            hitTimer = 0.3f;
            justHit = false;
        }
    }
    sprite.update();
    Vector2 camTarget = Vector2Add(position, Vector2{width*3/2.0f + aimDel.x, height*3/2.0f + aimDel.y});
    cam.update(camTarget);
    updateHUD();
}

inline void Player::draw() {
    if (DEBUG)
        for (auto &&crumb : breadCrumbs) {
            DrawCircleV(crumb.position, 5, YELLOW);
        }
    if (justHit) BeginShaderMode(flashShader);
    sprite.draw(position);
    if (justHit) EndShaderMode();
    if (!dead) gun.draw();
    if (explosivePrimeTimer > 0) {
        Vector2 mpos = getGlobalMousePosition();
        drawBar({mpos.x - 30, mpos.y - 30, 60, 10}, 1 - explosivePrimeTimer / 3.0f);
    }
}

inline void Player::updateHUD() {
    hud.update(gun.type, health/100.0f, gun.bulletsIn, gun.clips, charges);
}

inline void Player::fire() {
    if (gun.fire(false)) {
        Vector2 del = Vector2{cos(gun.rotation), sin(gun.rotation)};
        del = Vector2Scale(del, gun.kickback);
        if (gun.type == SNIPER or gun.type == SHOTGUN) visionBlur = 2;
        cam.shake(del, gun.kickback / 5);
        knockback = Vector2Normalize(Vector2Subtract(position, getGlobalMousePosition()));
        knockback = Vector2Scale(knockback, gun.kickback*50);
    }
}

inline void Player::drawHUD() {
    hud.draw();
    if (gun.reloading) {
        hud.drawReloadingEffect(gun.reloadingTime / gun.reloadTime);
    }
}

inline void Player::updateBreadcrumbs() {
    lastBreadCrumbTime -= delta;
    if (lastBreadCrumbTime <= 0 && Vector2LengthSqr(velocity) != 0) {
        lastBreadCrumbTime = 0.2;
        breadCrumbs.push_back(breadCrumb{{position.x, position.y}, true, 0});
    }
    for (auto &&crumb : breadCrumbs)
    {
        crumb.timePassed += delta;
        if (crumb.timePassed >= 6) crumb.active = false;
    }
    for (auto it = breadCrumbs.begin(); it != breadCrumbs.end();) {
        if (!(it)->active) {
            it = breadCrumbs.erase(it);
        } else {
            ++ it;
        }
    }
}

inline void Player::updateGun() {

    bool shoot = gun.automatic ? IsMouseButtonDown(0) : IsMouseButtonPressed(0);
    if (shoot) {
        fire();
    }

    gun.update(getGlobalMousePosition(), Vector2{position.x, position.y});

    for (auto &&bullet : bullets) {
        if (CheckCollisionRecs(collider, bullet.collider) && bullet.shotByEnemy)
        {
            applyDamage(bullet.damage, bullet.velocity);
            bullet.disable();
        }
    }
}

inline void Player::handleInput() {
    velocity = Vector2{0, 0};
    if (IsKeyDown(KEY_A))
    {
        velocity.x = -1;
    } else if (IsKeyDown(KEY_D)) {
        velocity.x = 1;
    }
    if (IsKeyDown(KEY_W))
    {
        velocity.y = -1;
    } else if (IsKeyDown(KEY_S)) {
        velocity.y = 1;
    }
    if (IsKeyPressed(KEY_SPACE) && dashing == 0 && charges > 0) {
        dashVelocity = Vector2Scale(Vector2Normalize(velocity), 10.0f*maxSpeed);
        dashing = true;
        charges--;
    }
    if(IsKeyPressed(KEY_G)) {
        throwGun(position, gun.type);
        gun = Gun(NONE);
    }
    if (IsKeyPressed(KEY_R)) {
        gun.reload();
    }

    if (IsMouseButtonDown(1) && charges > 0) {
        explosivePrimeTimer += delta;
        explosivePrimed = true;
    } else if (IsMouseButtonReleased(1) && explosivePrimed) {
        explosives.addExplosive(position, Vector2Scale(Vector2Normalize(Vector2Subtract(getGlobalMousePosition(), position)), 1400.0f*explosivePrimeTimer + 100), 100, 3 - explosivePrimeTimer);
        explosivePrimeTimer = 0;
        charges--;
        explosivePrimed = false;
    }
}

inline void Player::handleMovement() {
    velocity = Vector2Normalize(velocity);
    velocity = Vector2Scale(velocity, maxSpeed);

    if (dashing) {
        knockback = Vector2Zero();
        velocity = dashVelocity;
        dashVelocity = lerp(dashVelocity, Vector2Zero(), 5);
        if (Vector2LengthSqr(dashVelocity) < maxSpeed * maxSpeed) {
            dashing = false;
        }
    }

    if (Vector2LengthSqr(velocity) > 10) {
        sprite.play(1);
        //if ((sprite.frame == 3 || sprite.frame == 7) && !IsSoundPlaying(soundManager.getSound(5))) soundManager.playSoundWithVarPitch(5);
    } else {
        sprite.play(0);
    }

    velocity = Vector2Add(velocity, knockback);
    knockback = lerp(knockback, Vector2Zero(), 8);

    if (getGlobalMousePosition().x - position.x > 0) sprite.flipH = false;
    else sprite.flipH = true;

    collider = Rectangle{position.x - 22.5f, position.y - 32.5f, 45, 65};
    collider = MapLoader.checkCollisions(collider, velocity);
    position = Vector2{collider.x + 22.5f, collider.y + 32.5f};

    aimDel = Vector2Add(aimDel, Vector2Scale(Vector2Subtract(getGlobalMousePosition(), cam.cam.target), 0.5 * delta));
    aimDel = Vector2ClampValue(aimDel, -30/gun.scope, 30/gun.scope);

    float zoom = gun.scope * (1 - Vector2LengthSqr(velocity) / (maxSpeed * maxSpeed) * 0.02f);
    cam.setZoom(zoom);
}

inline void Player::applyDamage(float damage, Vector2 direction) {
    health -= damage;

    if (health <= 0) {
        dead = true;
        slowMo(1.0f, 0.1f);
    } else {
        justHit = true;
        slowMo(0.2f, 0.6f);
    }
    knockback = Vector2Add(knockback, Vector2Scale(direction, 200));
    knockback = Vector2ClampValue(knockback, -2*maxSpeed, 2*maxSpeed);
}

inline void Player::reset() {
    dead = false;
    health = 100;
    Gun gun = Gun(PISTOL);
    sprite.playingOnce = false;
}
