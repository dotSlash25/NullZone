class ShooterEnemy: public Enemy {
    private:
        AnimatedSprite sprite = AnimatedSprite(7, 3, (int[]){5, 8, 6}, 10);
        Vector2 patrolPoint = { 0 };
        bool dirtyPatrolPoint = true;
        float patrolWaitTime = 0;
        float reactionTime = GetRandomValue(0, 100) * 1.2f * 0.01f + 0.8f;
        float range = GetRandomValue(200, 400);
        float timeSinceInSight = 0;
    
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
            range = gun.range;
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
                    }
                    if (GetRandomValue(1, 100) < 5) {
                        state = PATROLLING;
                        dirtyPatrolPoint = true;
                    }
                    moveTarget = position;
                    break;
                case CHASING: {
                    playerInSight = MapLoader.rayCast(position, player.position);
                    foundBreadCrumb = false;
                    if (playerInSight) {
                        if (Vector2DistanceSqr(position, player.position) <= range*range) {
                            moveTarget = position;
                        } else {
                            moveTarget = player.position;
                        }
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
                        }
                    }
                    break;
                }
                case PATROLLING:
                    if (dirtyPatrolPoint && patrolWaitTime <= 0) {
                        Vector2 newPoint = {position.x + (float)GetRandomValue(-100, 100)/100.0 * 500, position.y + (float)GetRandomValue(-100, 100)/100.0 * 500};
                        int count = 0;
                        float size = 500;
                        while (!MapLoader.rayCast(position, newPoint)) {
                            size /= 2;
                            newPoint = {position.x + (float)GetRandomValue(-100, 100)/100.0 * size, position.y + (float)GetRandomValue(-100, 100)/100.0 * size};
                            count++;
                            if (count >= 5) {
                                state = IDLE;
                                break;
                            }
                        }
                        if (GetRandomValue(0, 100) < 5) {
                            state = IDLE;
                        }
                        patrolPoint = newPoint;
                        dirtyPatrolPoint = false;
                    } else {
                        if (Vector2DistanceSqr(patrolPoint, position) < 1600) {
                            dirtyPatrolPoint = true;
                            patrolPoint = position;
                            if (patrolWaitTime <= 0) {
                                patrolWaitTime = (float)GetRandomValue(0, 100)/100.0 * 3.0;
                            }
                            else patrolWaitTime -= delta;
                        }
                        moveTarget = patrolPoint;
    
                        if (Vector2DistanceSqr(position, player.position) < 640000 && MapLoader.rayCast(position, player.position)) {
                            state = CHASING;
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
            else if (target.x > position.x) sprite.flipH = false;
            else if (target.x < position.x) sprite.flipH = true;

            if (!dead){
                if (Vector2LengthSqr(velocity)) sprite.play(1);
                else sprite.play(0);
            }
    
            velocity = Vector2Add(velocity, knockback);
    
            velocity = Vector2Add(velocity, Vector2Scale(wallRepulsion, 10));
            collider = Rectangle{position.x - 20, position.y - 40, size.x, size.y};
            CollisionInfo info = MapLoader.checkCollisionsInfo(collider, velocity);
            collider = info.finalCollider;
            wallRepulsion = info.repulsiveDirection;
    
            position = Vector2{collider.x + 20, collider.y + 40};
            gun.update((playerInSight) ? target : moveTarget, Vector2{position.x, position.y});
            if (playerInSight) {
                if (timeSinceInSight > reactionTime && Vector2DistanceSqr(position, player.position) <= range*range) {
                    gun.fire(true);
                } else {
                    timeSinceInSight += delta;
                }
            } else {
                timeSinceInSight = std::max(timeSinceInSight - delta, 0.0f);
            }
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
                DrawText(TextFormat("%.2f %.2f\n%f", timeSinceInSight, reactionTime, Vector2Distance(position, player.position)), position.x, position.y, 10, WHITE);
            }
        }
        
        void death() {
            state = DEAD;
            sprite.playOnce(2);
            throwGun(Vector2Add(position, Vector2Scale(Vector2Rotate(Vector2{1, 0}, GetRandomValue(0, 628) / 100.0), 30)), gun.type);
            gun = Gun(NONE);
        }
};
    