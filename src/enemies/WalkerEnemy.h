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
    
            if (Vector2DistanceSqr(position, player.position) < 100) player.applyDamage(10, Vector2Subtract(player.position, position));
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