#pragma once

class Ally {
    private:
        Vector2 velocity = Vector2Zero();
        Vector2 size = Vector2Zero();
        Vector2 knockback = Vector2Zero();
        bool active = false;
        bool dead = false;
        float removeTimer = 5;
        int health = 0;
        float speed = 0;
        bool justHit = false;
        float hitTimer = 0.3f;
        float weight = 0;
        Rectangle collider = { 0 };

        AnimatedSprite sprite = AnimatedSprite(13, 3, (int[]){4, 4, 5}, 10);
        Vector2 patrolPoint = { 0 };
        bool dirtyPatrolPoint = true;
        float patrolWaitTime = 0;
        float reactionTime = GetRandomValue(0, 100) * 1.2f * 0.01f + 0.8f;
        float range = GetRandomValue(200, 400);
        float timeSinceInSight = 0;
    
        Vector2 wallRepulsion = { 0 };

        void baseUpdate() {
            if(!dead) {
                for (auto &&bullet : bullets) {
                    if (CheckCollisionRecs(collider, bullet.collider) && bullet.shotByEnemy) {
                        applyDamage(bullet.damage, bullet.velocity, 1);
                        bullet.disable();
                    }
                }
            } else {
                removeTimer -= delta;
                if (removeTimer <= 0) active = false;
            }
            knockback = lerp(knockback, Vector2Zero(), 6);
            if (justHit) {
                hitTimer -= delta;
                if (hitTimer <= 0) {
                    hitTimer = 0.3f;
                    justHit = false;
                }
            }
            sprite.update();
        }

    public:
        Vector2 position = Vector2Zero();
        Vector2 target = Vector2Zero();
        Vector2 moveTarget = Vector2Zero();
        bool enemyInSight = false;
        Gun gun = Gun(PISTOL);
        bool foundBreadCrumb = false;
        Enemy* targetEnemy = NULL;
        
        Ally(Vector2 pos, gunType gType) {
            position = pos;
            speed = 200;
            gun = Gun(gType);
            sprite.refresh();
            sprite.scale = 5;
            sprite.play(0);
            weight = 2;
            health = 40;
            size = {40, 80}; 
            gun.update({position.x + GetRandomValue(-100, 100), position.y + GetRandomValue(-100, 100)}, position);
        }
    
        void update() {
            if (!active) return;
            if (health < 0 && !dead) {
                sprite.playOnce(2);
                return;
            }
            baseUpdate();
            if (dead) return;
            if (gun.bulletsIn == 0) gun.reload();

            if (targetEnemy == NULL || targetEnemy->dead) {
                for (auto &&enemy : enemies) {
                    if (enemy->dead) continue;
                    if (Vector2DistanceSqr(position, enemy->position) < 64000) {
                        if (MapLoader.rayCast(position, enemy->position)) {
                            targetEnemy = enemy.get();
                            break;
                        }
                    }
                }
                enemyInSight = false;
            } else {
                if (MapLoader.rayCast(position, targetEnemy->position)) {
                    target = targetEnemy->position;
                    enemyInSight = true;
                }
                else enemyInSight = false;
            }

            if (Vector2DistanceSqr(position, player.position) > 4000) {
                moveTarget = player.position;
                sprite.play(1);
            } else {
                sprite.play(0);
                moveTarget = position;
            }
            
            velocity = {moveTarget.x - position.x, moveTarget.y - position.y};
            velocity = Vector2Normalize(velocity);
            velocity = Vector2Scale(velocity, speed);
    
            if (velocity.x > 0) sprite.flipH = false;
            else if (velocity.x < 0) sprite.flipH = true;
            else if (target.x > position.x) sprite.flipH = false;
            else if (target.x < position.x) sprite.flipH = true;
    
            velocity = Vector2Add(velocity, knockback);
    
            velocity = Vector2Add(velocity, Vector2Scale(wallRepulsion, 10));
            collider = Rectangle{position.x - 20, position.y - 40, size.x, size.y};
            CollisionInfo info = MapLoader.checkCollisionsInfo(collider, velocity);
            collider = info.finalCollider;
            wallRepulsion = info.repulsiveDirection;
    
            position = Vector2{collider.x + 20, collider.y + 40};
            gun.update(target, Vector2{position.x, position.y + 20});
            if (enemyInSight) {
                if (timeSinceInSight > reactionTime && Vector2DistanceSqr(position, targetEnemy->position) <= range*range) {
                    gun.fire(false);
                } else {
                    timeSinceInSight += delta;
                }
            } else {
                timeSinceInSight = std::max(timeSinceInSight - delta, 0.0f);
            }
        }
    
        void draw() {
            if (!active) return;
            sprite.draw(position);
            if (justHit) {
                sprite.draw(position, WHITE);
            }
            gun.draw();
            if (DEBUG) {
                DrawCircleV(patrolPoint, 5, RED);
                DrawCircleV(moveTarget, 10, GREEN);
                DrawLineV(position, Vector2Add(position, wallRepulsion), WHITE);
                DrawText(TextFormat("%.2f %.2f\n%f", timeSinceInSight, reactionTime, Vector2Distance(position, player.position)), position.x, position.y, 10, WHITE);
            }
        }
        
        void death() {
            sprite.playOnce(2);
            throwGun(Vector2Add(position, Vector2Scale(Vector2Rotate(Vector2{1, 0}, GetRandomValue(0, 628) / 100.0), 30)), gun.type);
            gun = Gun(NONE);
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

        void spawn(Vector2 pos) {
            active = true;
            position = pos;
        }
};

extern Ally ally;

class AllySpawner {
    private:
    AnimatedSprite sprite = AnimatedSprite(14, 1, (int[]){10}, 10);
    bool active = false;
    
    public:
    Vector2 position = { 0 };

    void init() {
        sprite.refresh();
        sprite.scale = 3;
        active = true;
    }

    void update() {
        if (!active) return;
        sprite.update();
    }

    void draw() {
        if (!active) return;
        sprite.draw(position);
        if (Vector2DistanceSqr(position, player.position) < 900) {
            int w = 7;
            DrawRectangleLines(position.x - w, position.y + 10, 2*w, 2*w, WHITE);
            DrawText("E", position.x - MeasureText("E", 10) / 2, position.y + 10 + (2*w - 10) / 2, 10, WHITE);

            if (IsKeyPressed(KEY_E)) {
                active = false;
                ally.spawn(position);
            }
        }
    }
};