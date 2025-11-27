#pragma once

class camera
{
private:
    Vector2 reqTarget = { 0 };
    float reqZoom = 1;
    Vector2 shakeOffset = { 0 };
    float rotTimer = 0;
    Vector2 shakeBias = Vector2Zero();
    float shakeValue = 0;
public:
    Camera2D cam;
    camera();
    void update(Vector2 pos);
    void zoomBy(float value);
    void setZoom(float value);
    void shake(Vector2 init, float amount);
    void shakeExplosion(float dis);
    void init();
    Rectangle getViewRect();
    float getTrauma();
    ~camera();
};

inline camera::camera() {
    reqTarget = {0,0};
    cam.offset = Vector2{(float)SCREENWIDTH/2, (float)SCREENHEIGHT/2};
    cam.zoom = 1.0f;
    reqZoom = 1.0f;
    shakeOffset = { 0 };
    rotTimer = 0;
    shakeBias = Vector2Zero();
    shakeValue = 0;
}

inline void camera::init() {
    reqTarget = {0,0};
    cam.offset = Vector2{(float)SCREENWIDTH/2, (float)SCREENHEIGHT/2};
    cam.zoom = 1.0f;
    reqZoom = 1.0f;
    shakeOffset = { 0 };
    rotTimer = 0;
    shakeBias = Vector2Zero();
    shakeValue = 0;
}

inline void camera::update(Vector2 pos)
{
    cam.zoom = lerp(cam.zoom, reqZoom, 2);
    //cam.target = pos;//lerp(cam.target, pos, 25);
    cam.target = Vector2Add(pos, shakeOffset);
    shakeValue = lerp(shakeValue, 0, 2.0f);

    float randAngle = GetRandomValue(0, 360) * DEG2RAD;
    shakeOffset = Vector2Scale(Vector2Add(Vector2Rotate({1, 0}, randAngle), shakeBias), shakeValue*shakeValue);
    shakeOffset = Vector2ClampValue(shakeOffset, -200, 200);

    shakeBias = lerp(shakeBias, Vector2Zero(), 3.0f);

    if (rotTimer >= 0) {
        rotTimer -= delta;
        cam.rotation = GetRandomValue(-30, 30) * rotTimer * 25.0f;
        cam.rotation = Clamp(cam.rotation, -30, 30);
    }

    cam.rotation = lerp(cam.rotation, 0, 15);
}

inline void camera::zoomBy(float value)
{
    reqZoom += value;
}

inline void camera::setZoom(float value)
{
    reqZoom = value;
}

inline void camera::shake(Vector2 init, float amount)
{
    shakeBias = init;
    shakeValue += amount;
    // rotTimer += 0.1f;
}

inline void camera::shakeExplosion(float dis) {
    // shakeBias = Vector2Zero();
    float range = 500;
    if (dis > range) return;
    shakeValue += 10*(1 - dis/range);
    shakeValue = Clamp(shakeValue, 0, 15);
    rotTimer += (1 - dis/range);
    rotTimer = Clamp(rotTimer, 0, 0.04f);
}

inline camera::~camera()
{
}

inline Rectangle camera::getViewRect() {
    Vector2 zero = GetScreenToWorld2D({0, 0}, this->cam);
    Vector2 corner = GetScreenToWorld2D({(float)SCREENWIDTH, (float)SCREENHEIGHT}, this->cam);
    return {zero.x, zero.y, corner.x - zero.x, corner.y - zero.y};
}

inline float camera::getTrauma() {
    return shakeValue * shakeValue;
}
