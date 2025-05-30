class WalkerEnemy: public Enemy {
    private:
        AnimatedSprite sprite = AnimatedSprite(6, 3, (int[]){4, 4, 4}, 10);
    public:
        WalkerEnemy(Vector2 pos) : Enemy(5, {20, 30}, 1) {
            position = pos;
            speed = 280;
            sprite.scale = 5;
            sprite.refresh();
            sprite.play(0);
            state = IDLE;
        }
    
    void update() {
        if (!dead) {
            if (state == CHASING) {
                velocity = {player.position.x - position.x, player.position.y - position.y};    
                if (velocity.x > 0) sprite.flipH = false;
                else sprite.flipH = true;
        
                if (Vector2DistanceSqr(position, player.position) < 100) {
                    Vector2 del = Vector2Subtract(player.position, position);
                    player.applyDamage(5, del);
                    knockback = Vector2Scale(del, -speed/4);
                }

                if (Vector2DistanceSqr(position, player.position) > 225000 && !MapLoader.rayCast(position, player.position)) {
                    state = IDLE;
                    sprite.play(0);
                }
            } else {
                velocity = lerp(velocity, Vector2Zero(), 10);
                if (Vector2DistanceSqr(position, player.position) < 81000 && MapLoader.rayCast(position, player.position)) {
                    state = CHASING;
                    sprite.play(1);
                }
            }
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