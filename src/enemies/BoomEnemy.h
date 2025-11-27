class BoomEnemy: public Enemy {
    private:
        AnimatedSprite sprite = AnimatedSprite(8, 3, (int[]){4, 4, 1}, 10);
        bool isAlarmed = false;
    public:
    BoomEnemy(Vector2 pos) : Enemy(3, {20, 30}, 4) {
        position = pos;
        speed = 250;
        sprite.scale = 5;
        sprite.refresh();
        dead = false;
        justHit = false;
    }
    
    void update() {
        if (!dead) {
            if (isAlarmed) {
                sprite.play(1);
                velocity = {player.position.x - position.x, player.position.y - position.y};    
                if (velocity.x > 0) sprite.flipH = false;
                else sprite.flipH = true;
                if (Vector2DistanceSqr(position, player.position) < 400) death();
            } else {
                sprite.play(0);
                if (Vector2DistanceSqr(player.position, position) < 80000) isAlarmed = true;
            }
        } else {
            speed = lerp(speed, 0, 4);
        }
        velocity = Vector2Normalize(velocity);
        velocity = Vector2Scale(velocity, speed);
            
        velocity = Vector2Add(velocity, knockback);
            
        collider = Rectangle{position.x - 10, position.y - 5, size.x, size.y};
        collider = MapLoader.checkCollisions(collider, velocity);
        position = Vector2{collider.x + 10, collider.y + 5};
    
        baseUpdate();
        sprite.update();
    }
    
    void draw() {
        if (justHit) {
            sprite.draw(position, WHITE);
        } else {
            sprite.draw(position);
        }
    }
    
    void death() {
        explosives.addExplosive(position, Vector2Zero(), 50, 0);
        dead = true;
        sprite.playOnce(2);
    }
};