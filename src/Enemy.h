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

#include "src/enemies/WalkerEnemy.h"
#include "src/enemies/ShooterEnemy.h"
#include "src/enemies/BoomEnemy.h"