#pragma once


extern SpritesManager spriteManager;
//chhatu phuti jaichi
class Particle
{
public:
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float rotation = 0;
    float angularVelocity = 0;
    float scale = 1;
    bool active = false;
    float lifeTime = 0;
    float damping = 0;
    float size = 5;
    int textureID = -1;
    int textureFrame = 0;
    Particle();
    ~Particle();
    void update();
    void draw();
};

Particle::Particle()
{
}

Particle::~Particle()
{
}

inline void Particle::update()
{
    if (active) {
        velocity = Vector2Add(Vector2Scale(acceleration, delta), velocity);
        velocity = Vector2Scale(velocity, 1 - damping);
        position = Vector2Add(position, Vector2Scale(velocity, delta));
        lifeTime -= delta;
        scale = lerp(scale, 0, 3);
        if (lifeTime <= 0) active = false;
    }
}

inline void Particle::draw()
{
    if(active) {
        float drawSize = size * scale;
        if (textureID == -1) {
            DrawRectanglePro(Rectangle{position.x - size/2, position.y - size/2, size, size}, {size/2, size/2}, rotation, RAYWHITE);
        } else {
            DrawTexturePro(spriteManager.sprite(textureID), Rectangle{textureFrame*16, 16, 16, 16}, Rectangle{position.x, position.y, drawSize, drawSize}, {drawSize/2, drawSize/2}, rotation, WHITE);
        }
    }
}



class Particles {
    private:
        float timePassed = 0;
        float lastSpawn = 0;
    public:
        int amount = 20;
        bool emitting = false;
        Particles();
        ~Particles();
        Vector2 position = Vector2Zero();
        Vector2 direction = Vector2{0, 1};
        float initialVelocity = 10.0f;
        Vector2 particleAcceleration = Vector2{0, 0};
        Particle particles[100];
        float lifetime = 1;
        bool oneShot = true;
        float spread = 0.0f;
        float damping = 0;
        int textureID = -1;
        int maxTextureFrame = 0;
        float scale = 1;
        void update();
        void draw();
        void start();
};

inline Particles::Particles(void)
{
}

inline Particles::~Particles(void)
{
}

inline void Particles::update(void)
{
    if (emitting) {
        emitting = false;
        int spawnAmount = 0;
        for (auto &&p : particles)
        {
            if (!p.active)
            {
                p.active = true;
                spawnAmount++;
                p.position = position;
                Vector2 v = Vector2Scale(direction, initialVelocity * (float)GetRandomValue(50, 150)/100.0f);
                p.velocity = Vector2Rotate(v, (float)GetRandomValue(-100, 100)/100.0f * spread);
                p.rotation = GetRandomValue(-100, 100)/100.0f * 180.0f;
                p.acceleration = particleAcceleration;
                p.lifeTime = lifetime;
                p.damping = damping;
                p.textureID = textureID;
                p.textureFrame = GetRandomValue(0, maxTextureFrame);
                p.scale = scale;
                if (spawnAmount == amount)
                    break;
            }    
        }        
    }
    for (auto &&i : particles)
    {
        i.update();
    }
}

inline void Particles::draw(void) {
    for (auto &&i : particles)
    {
        i.draw();
    }
}

inline void Particles::start()
{
    emitting = true;
    timePassed = 0;
}


typedef struct overworldParticle {
    Vector2 position;
    short spriteIndex;
    Vector2 velocity;
    bool active;
};

class OverworldParticles {
public:
    Rectangle bounds = { 0 };
    Vector2 windDirection = { 0 };
    static const int numParticles = 300;
    overworldParticle particles[numParticles];
    float lastWindChanged = 0;

    OverworldParticles(Rectangle _bounds);
    void update();
    void draw(Vector2 referencePosition);
};

OverworldParticles::OverworldParticles(Rectangle _bounds) {
    bounds = _bounds;
}

inline void OverworldParticles::update() {
    for (short i = 0; i < numParticles; i++) {
        if (!particles[i].active) {
            particles[i].position = {GetRandomValue(bounds.x, bounds.width), GetRandomValue(bounds.y, bounds.height)};
            particles[i].velocity = {GetRandomValue(-300, 300), GetRandomValue(-300, 300)};
            particles[i].spriteIndex = GetRandomValue(0, 15);
            particles[i].active = true;
        } else {
            particles[i].velocity = Vector2Add(particles[i].velocity, Vector2Scale(windDirection, delta));
            particles[i].position = Vector2Add(particles[i].position, Vector2Scale(particles[i].velocity, delta));
            if (!CheckCollisionPointRec(particles[i].position, bounds)) particles[i].active = false;
        }
    }
    lastWindChanged -= delta;
    if (lastWindChanged <= 0) {
        windDirection = Vector2{GetRandomValue(-30,30), GetRandomValue(-30,30)};
    }
}

inline void OverworldParticles::draw(Vector2 referencePoint) {
    for (short i = 0; i < numParticles; i++) {
        int delX = particles[i].spriteIndex%4;
        int delY = (int)(particles[i].spriteIndex/4);
        float scale = 2;
        DrawTexturePro(spriteManager.sprite(5), Rectangle{delX*16, delY*16, 16, 16}, Rectangle{particles[i].position.x, particles[i].position.y, 16*scale, 16*scale}, Vector2Zero(), 0.0f, WHITE);
    }
}