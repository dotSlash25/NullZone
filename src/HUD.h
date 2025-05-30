#pragma once

class HUD {
private:
    float gunScale = 0;
    int selectedGunType = 0;
    float health = 0;
    int bullets = 0;
    int clips = 0;
    float bulletScale = 1.0f;
public:
    void update(gunType _selectedGunType, float health, int bullets, int clips);
    void draw();
};

inline void HUD::update(gunType _selectedGunType, float healthRatio, int _bullets, int _clips) {
    selectedGunType = (int)_selectedGunType;
    health = healthRatio;
    if (bullets != _bullets) bulletScale = 2;
    bullets = _bullets;
    clips = _clips;
    bulletScale = lerp(bulletScale, 1, 10);
}

inline void HUD::draw() {
    DrawRectangleLinesEx({45.0f + 10.0f, SCREENHEIGHT - 50, 40, 40}, 2, foregroundColour2);
    Rectangle srcTexture = gunSizes[selectedGunType - 1];
    float scale = 5.4f;
    Vector2 org = {srcTexture.width * scale / 2, srcTexture.height * scale / 2};
    Rectangle dst = {45.0f + 10.0f + 20.0f, SCREENHEIGHT - 50.0f + 20.0f, srcTexture.width*scale, srcTexture.height*scale};
    DrawTexturePro(spriteManager.sprite(1), srcTexture, dst, org, -45, WHITE);
    Vector2 mousePos = GetMousePosition();
    mousePos = Vector2Subtract(mousePos, {7, 7});
    DrawTextureEx(spriteManager.sprite(10), mousePos, 0, 2, WHITE);
    DrawText(TextFormat("%i/%i", bullets, clips), SCREENWIDTH - MeasureText(TextFormat("%i/%i", bullets, clips), 20 * bulletScale) - 10, SCREENHEIGHT - 10 - 20*bulletScale, 20*bulletScale, WHITE);
}