#pragma once

extern Player player;
extern std::vector<Bullet> bullets;

enum enemyState {IDLE, PATROLLING, CHASING, DEAD};

class Enemy {
public:
    Vector2 position = Vector2Zero();
    Vector2 velocity = Vector2Zero();
    Vector2 size = Vector2Zero();
    Vector2 knockback = Vector2Zero();
    bool active = true;
    bool dead = false;
    float removeTimer = 5;
    int health = 0;
    float speed = 0;
    bool justHit = false;
    float hitTimer = 0.3f;
    float weight = 0;
    Rectangle collider = { 0 };
    enemyState state;

    Enemy(int _h, Vector2 _s, float _w) {
        health = _h;
        size = _s;
        weight = _w;
        collider.width = size.x;
        collider.height = size.y;
    }

    void move() {
        collider = Rectangle{position.x, position.y, 40, 40};
        collider = MapLoader.checkCollisions(collider, velocity);
        position = Vector2{collider.x, collider.y};
    }

    virtual void update() {};
    virtual void draw() {};
    virtual void death() {};

    void damageCheck() {
        for (auto &&bullet : bullets) {
            if (CheckCollisionRecs(collider, bullet.collider) && !bullet.shotByEnemy) {
                applyDamage(bullet.damage, bullet.velocity, 1);
                bullet.disable();
            }
        }
    }

    void hitManager() {
        if (justHit) {
            hitTimer -= delta;
            if (hitTimer <= 0) {
                hitTimer = 0.3f;
                justHit = false;
            }
        }
    }

    void baseUpdate() {
        if(!dead) {
            damageCheck();
        } else {
            removeTimer -= delta;
            if (removeTimer <= 0) active = false;
        }
        knockback = lerp(knockback, Vector2Zero(), 6);
        hitManager();
    }

    void applyDamage(float damage, Vector2 direction, float impactMultiplier) {
        health -= damage;
        justHit = true;
        knockback = Vector2Add(knockback, Vector2Scale(direction, 200*impactMultiplier/weight));
        if (health <= 0) {
            if (!dead) {
                dead = true;
                death();
            }
        }
    }
};

class WalkerEnemy: public Enemy {
private:
    AnimatedSprite sprite = AnimatedSprite(6, 3, (int[]){4, 4, 4}, 10);
public:
    
    WalkerEnemy(Vector2 pos) : Enemy(20, {20, 30}, 1) {
        position = pos;
        speed = 250;
        sprite.scale = 5;
        sprite.refresh();
        sprite.play(1);
    }

    void update() {
        if (!dead) {
            velocity = {player.position.x - position.x, player.position.y - position.y};    
            if (velocity.x > 0) sprite.flipH = false;
            else sprite.flipH = true;
        } else {
            speed = lerp(speed, 0, 4);
        }
        velocity = Vector2Normalize(velocity);
        velocity = Vector2Scale(velocity, speed);
        
        velocity = Vector2Add(velocity, knockback);
        
        collider = Rectangle{position.x - 10, position.y - 25, size.x, size.y};
        collider = MapLoader.checkCollisions(collider, velocity);
        position = Vector2{collider.x + 10, collider.y + 25};

        baseUpdate();
        sprite.update();
    }

    void draw() {
        sprite.draw(position);
        if (justHit) {
            sprite.draw(position, WHITE);
        }
    }

    void death() {
        sprite.playOnce(2);
    }
};

class ShooterEnemy: public Enemy {
private:
    AnimatedSprite sprite = AnimatedSprite(7, 3, (int[]){5, 8, 6}, 10);
    Vector2 patrolPoint = { 0 };
    bool dirtyPatrolPoint = true;
    float patrolWaitTime = 0;

    Vector2 wallRepulsion = { 0 };
public:
    Vector2 target = Vector2Zero();
    Vector2 moveTarget = Vector2Zero();
    bool playerInSight = false;
    Gun gun = Gun(PISTOL);
    bool foundBreadCrumb = false;
    
    ShooterEnemy(Vector2 pos, gunType gType): Enemy(40, {40, 55}, 2) {
        active = true;
        position = pos;
        speed = 200;
        gun = Gun(gType);
        state = IDLE;
        sprite.refresh();
        sprite.scale = 5;
        gun.update({position.x + GetRandomValue(-100, 100), position.y + GetRandomValue(-100, 100)}, position);
    }

    void update() {
        if (health < 0 && !dead) {
            sprite.playOnce(2);
            return;
        }
        if (gun.bulletsIn == 0) gun.reload();
        switch (state) {
            case IDLE:
                if (Vector2DistanceSqr(position, player.position) < 640000 && MapLoader.rayCast(position, player.position)) {
                    state = CHASING;
                    sprite.play(1);
                }
                if (GetRandomValue(1, 100) < 5) {
                    state = PATROLLING;
                    dirtyPatrolPoint = true;
                    sprite.play(1);
                }
                moveTarget = position;
                break;
            case CHASING: {
                playerInSight = MapLoader.rayCast(position, player.position);
                foundBreadCrumb = false;
                if (playerInSight) {
                    moveTarget = player.position;
                    target = player.position;
                } else {
                    float minCost = 0;
                    int bestID = 0;
                    for(int i = 0; i < (int)player.breadCrumbs.size(); i++) {
                        if (!(MapLoader.rayCast(position, player.breadCrumbs[i].position))) continue;
                        float dis = Vector2DistanceSqr(position, player.breadCrumbs[i].position);
                        float plrDis = Vector2DistanceSqr(player.breadCrumbs[i].position, player.position);
                        float cost = dis + plrDis;
                        if (minCost == 0) minCost = cost; 
                        else if (cost > minCost) continue;
                        foundBreadCrumb = true;
                        bestID = i;
                    }
                    moveTarget = player.breadCrumbs[bestID].position;
                    if (!foundBreadCrumb) {
                        state = IDLE;
                        sprite.play(0);
                    }
                }
                break;
            }
            case PATROLLING:
                if (dirtyPatrolPoint && patrolWaitTime <= 0) {
                    sprite.play(1);
                    Vector2 newPoint = {position.x + (float)GetRandomValue(-100, 100)/100.0 * 500, position.y + (float)GetRandomValue(-100, 100)/100.0 * 500};
                    int count = 0;
                    float size = 500;
                    while (!MapLoader.rayCast(position, newPoint)) {
                        size /= 2;
                        newPoint = {position.x + (float)GetRandomValue(-100, 100)/100.0 * size, position.y + (float)GetRandomValue(-100, 100)/100.0 * size};
                        count++;
                        if (count >= 5) {
                            state = IDLE;
                            sprite.play(0);
                            break;
                        }
                    }
                    if (GetRandomValue(0, 100) < 5) {
                        state = IDLE;
                        sprite.play(0);
                    }
                    patrolPoint = newPoint;
                    dirtyPatrolPoint = false;
                } else {
                    if (Vector2DistanceSqr(patrolPoint, position) < 1600) {
                        dirtyPatrolPoint = true;
                        patrolPoint = position;
                        if (patrolWaitTime <= 0) {
                            patrolWaitTime = (float)GetRandomValue(0, 100)/100.0 * 3.0;
                            sprite.play(0);
                        }
                        else patrolWaitTime -= delta;
                    }
                    moveTarget = patrolPoint;

                    if (Vector2DistanceSqr(position, player.position) < 640000 && MapLoader.rayCast(position, player.position)) {
                        state = CHASING;
                        sprite.play(1);
                    }

                }
                break;

            case DEAD:
                moveTarget = position;
                break;
        }
        velocity = {moveTarget.x - position.x, moveTarget.y - position.y};
        velocity = Vector2Normalize(velocity);
        velocity = Vector2Scale(velocity, speed);

        if (velocity.x > 0) sprite.flipH = false;
        else if (velocity.x < 0) sprite.flipH = true;

        velocity = Vector2Add(velocity, knockback);

        velocity = Vector2Add(velocity, Vector2Scale(wallRepulsion, 10));
        collider = Rectangle{position.x - 20, position.y - 40, size.x, size.y};
        CollisionInfo info = MapLoader.checkCollisionsInfo(collider, velocity);
        collider = info.finalCollider;
        wallRepulsion = info.repulsiveDirection;

        position = Vector2{collider.x + 20, collider.y + 40};
        gun.update(target, Vector2{position.x, position.y});
        if (playerInSight) gun.fire(true);
        sprite.update();
        baseUpdate();
    }

    void draw() {
        sprite.draw(position);
        if (justHit) {
            sprite.draw(position, WHITE);
        }
        gun.draw();
        if (DEBUG) {
            DrawCircleV(patrolPoint, 5, RED);
            DrawCircleV(moveTarget, 10, GREEN);
            DrawLineV(position, Vector2Add(position, wallRepulsion), WHITE);
        }
    }
    
    void death() {
        state = DEAD;
        sprite.playOnce(2);
        throwGun(Vector2Add(position, Vector2Scale(Vector2Rotate(Vector2{1, 0}, GetRandomValue(0, 628) / 100.0), 30)), gun.type);
        gun = Gun(NONE);
    }
};
