#pragma once

class HUD {
private:
    float gunScales[2] = { 0 };
    int selectedGunID = 0;
    int selectedGunTypes[2] = { 0 };
    float health = 0;
    int bullets = 0;
    int clips = 0;
public:
    gunType guns[2];
    HUD();
    void update(int selectedGun, gunType selectedGunType, float health, int bullets, int clips);
    void draw();
};

HUD::HUD() {
    guns[0] = NONE;
    guns[1] = NONE;
}

inline void HUD::update(int selectedGun, gunType selectedGunType, float healthRatio, int _bullets, int _clips) {
    selectedGunID = selectedGun;
    selectedGunTypes[selectedGun] = (int)selectedGunType;
    health = healthRatio;
    bullets = _bullets;
    clips = _clips;
}

inline void HUD::draw() {
    for (short i = 0; i < 2; i++) {
        DrawRectangleLinesEx({i*45.0f + 10.0f, SCREENHEIGHT - 50, 40, 40}, 2, foregroundColour2);
        Rectangle srcTexture = gunSizes[selectedGunTypes[selectedGunID] - 1];
        float scale = 5.4f;
        Vector2 org = {srcTexture.width * scale / 2, srcTexture.height * scale / 2};
        Rectangle dst = {i*45.0f + 10.0f + 20.0f, SCREENHEIGHT - 50.0f + 20.0f, srcTexture.width*scale, srcTexture.height*scale};
        DrawTexturePro(spriteManager.sprite(1), srcTexture, dst, org, -45, WHITE);
    }
    Vector2 mousePos = GetMousePosition();
    mousePos = Vector2Subtract(mousePos, {7, 7});
    DrawTextureEx(spriteManager.sprite(10), mousePos, 0, 2, WHITE);
    DrawText(TextFormat("%i/%i", bullets, clips), SCREENWIDTH - MeasureText(TextFormat("%i/%i", bullets, clips), 20) - 10, SCREENHEIGHT - 30, 20, WHITE);
}