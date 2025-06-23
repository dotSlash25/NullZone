#pragma once

struct Explosive {
    Vector2 position;
    Vector2 velocity;
    float detonationTime;
    float timePassed;
    bool active;
    bool detonating;
    float timeAfterDetonation;
    int frame;
    float scale;
    int rotation;
    bool damageApplied;
};

Particles explosionParticles[MAX_EXPLOSIVES];

class Explosives {
private:
    struct Explosive explosivesArr[MAX_EXPLOSIVES] = { 0 };
    void detonate(struct Explosive* e) {
        e->detonating = true;
        e->timeAfterDetonation = 0;
        e->frame = 0;
        e->rotation = GetRandomValue(0, 360);
        e->velocity = Vector2Zero();
        e->scale = 10 * GetRandomValue(80, 120) / 100.0f;
        e->damageApplied = false;
    }

public:

    Explosives() {
        for (int i = 0; i < MAX_EXPLOSIVES; i++) {
            explosionParticles[i].textureID = 11;
            explosionParticles[i].initialVelocity = 200.0f;
            explosionParticles[i].damping = 0.01;
            explosionParticles[i].spread = 6.28;
            explosionParticles[i].maxTextureFrame = 1;
            explosionParticles[i].scale = 80;
            explosionParticles[i].lifetime = 3;
        }
    }

    void addExplosive(Vector2 position, Vector2 velocity, float _detonationTime) {
        for (int i = 0; i < MAX_EXPLOSIVES; i++) {
            if (!explosivesArr[i].active) {
                explosivesArr[i].position = position;
                explosivesArr[i].velocity = velocity;
                explosivesArr[i].detonationTime = _detonationTime;
                explosivesArr[i].active = true;
                explosivesArr[i].frame = 0;
                explosivesArr[i].timePassed = 0;
                break;
            }
        }
    }

    void update() {
        for (int i = 0; i < MAX_EXPLOSIVES; i++) {
            explosionParticles[i].update();
            if (!explosivesArr[i].active) continue;
            if (MapLoader.checkCollisions({explosivesArr[i].position.x + explosivesArr[i].velocity.x * delta, explosivesArr[i].position.y, 3, 3})) explosivesArr[i].velocity.x *= -1;
            if (MapLoader.checkCollisions({explosivesArr[i].position.x ,explosivesArr[i].position.y + explosivesArr[i].velocity.y * delta, 3, 3})) explosivesArr[i].velocity.y *= -1;
            explosivesArr[i].velocity = lerp(explosivesArr[i].velocity, Vector2Zero(), 2);
            explosivesArr[i].position = Vector2Add(explosivesArr[i].position, Vector2Scale(explosivesArr[i].velocity, delta));
            explosivesArr[i].timePassed += delta;
            if (!explosivesArr[i].detonating && explosivesArr[i].timePassed >= explosivesArr[i].detonationTime) {
                detonate(&explosivesArr[i]);
                explosionParticles[i].position = explosivesArr[i].position;
                explosionParticles[i].start();
            } else if (explosivesArr[i].detonating) {
                explosivesArr[i].timeAfterDetonation += delta;
                explosivesArr[i].frame = (int)(explosivesArr[i].timeAfterDetonation / 0.015);
                if (explosivesArr[i].frame == 3 && !explosivesArr[i].damageApplied) {
                    applyAreaDamage(explosivesArr[i].position, 100, 20*explosivesArr[i].scale);
                    explosivesArr[i].damageApplied = true;
                }
                if (explosivesArr[i].frame >= 12) {
                    explosivesArr[i].active = false;
                    explosivesArr[i].detonating = false;
                }
            }
        }
    }

    void draw() {
        for (int i = 0; i < MAX_EXPLOSIVES; i++) {
            explosionParticles[i].draw();
            if (!explosivesArr[i].active) continue;
            if (explosivesArr[i].detonating) {
                int swidth = 32;
                int sheight = 32;
                Rectangle sourceRec = { (float)explosivesArr[i].frame*swidth, 0, (float)swidth, (float)sheight};
                Rectangle dstRec = { explosivesArr[i].position.x, explosivesArr[i].position.y, swidth*explosivesArr[i].scale, sheight*explosivesArr[i].scale};
                DrawCircleV(explosivesArr[i].position, explosivesArr[i].scale*20, Fade(BLACK, 0.2));
                DrawTexturePro(spriteManager.sprite(12), sourceRec, dstRec, {swidth / 2 * explosivesArr[i].scale, sheight / 2 * explosivesArr[i].scale}, explosivesArr[i].rotation, WHITE);
            } else {
                int swidth = 6;
                int sheight = 6;
                float scale = 4 + sinf(powf(explosivesArr[i].timePassed, 3)*8.0f);
                Rectangle sourceRec = { 0, 0, (float)swidth, (float)sheight};
                Rectangle dstRec = {explosivesArr[i].position.x, explosivesArr[i].position.y, swidth*scale, sheight*scale};
                DrawTexturePro(spriteManager.sprite(9), sourceRec, dstRec, {swidth / 2 * scale, sheight / 2 * scale}, explosivesArr[i].rotation, WHITE);
            }
        }
    }
};

extern Explosives explosives;