#pragma once

class camera
{
private:
    Vector2 reqTarget;
    float reqZoom = 1;
    Vector2 shakeOffset;
    float rotTimer = 0;
    Vector2 shakeBias = Vector2Zero();
    float shakeValue = 0;
public:
    Camera2D cam;
    camera();
    void update(Vector2 pos);
    void zoomBy(float value);
    void setZoom(float value);
    void shake(Vector2 init);
    void shakeExplosion(float dis);
    ~camera();
};

camera::camera()
{
    reqTarget = {0,0};
    cam.offset = Vector2{SCREENWIDTH/2, SCREENHEIGHT/2};
    cam.zoom = 1.0f;
    reqZoom = 1.0f;
}

inline void camera::update(Vector2 pos)
{
    cam.zoom = lerp(cam.zoom, reqZoom, 2);
    cam.target = lerp(cam.target, pos, 25);
    cam.target = Vector2Add(cam.target, shakeOffset);
    shakeValue = lerp(shakeValue, 0, 5);

    float randAngle = GetRandomValue(0, 360) * DEG2RAD;
    shakeOffset = Vector2Scale(Vector2Add(Vector2Rotate({1, 0}, randAngle), shakeBias), shakeValue);

    if (rotTimer >= 0) {
        rotTimer -= delta;
        cam.rotation = GetRandomValue(-30, 30) * rotTimer * 50.0f;
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

inline void camera::shake(Vector2 init)
{
    shakeBias = init;
    shakeValue += 0.05;
}

inline void camera::shakeExplosion(float dis) {
    shakeBias = Vector2Zero();
    shakeValue += 1;
    shakeValue = Clamp(shakeValue, 0, 5);
    rotTimer += dis/20.0f;
    rotTimer = Clamp(rotTimer, 0, 0.08f);
}

camera::~camera()
{
}
