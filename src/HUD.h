#pragma once

class HUD {
private:
    float gunScale = 1;
    int selectedGunType = 0;
    float health = 0;
    int bullets = 0;
    int clips = 0;
    float bulletScale = 1.0f;
    float chargeScale = 0.0f;
    float healthScale = 1.0f;
    int charges = 0;
public:
    void update(gunType _selectedGunType, float health, int bullets, int clips, int _charges);
    void draw();
};

inline void HUD::update(gunType _selectedGunType, float healthRatio, int _bullets, int _clips, int _charges) {
    if (selectedGunType != _selectedGunType) gunScale = 2;
    selectedGunType = (int)_selectedGunType;
    if (health != healthRatio) healthScale = 1.35f;
    health = healthRatio;
    if (bullets != _bullets) bulletScale = 2;
    bullets = _bullets;
    clips = _clips;
    bulletScale = lerp(bulletScale, 1, 10);
    gunScale = lerp(gunScale, 1, 10);
    chargeScale = lerp(chargeScale, 0, 5);
    healthScale = lerp(healthScale, 1.0f, 5);
    if (charges < _charges) chargeScale = 3.0f;
    charges = _charges;
}

inline void HUD::draw() {
    DrawRectangle(10, SCREENHEIGHT - 90, 80, 80, Fade(WHITE, 0.2));
    DrawRectangleLinesEx({10.0f, SCREENHEIGHT - 90, 80, 80}, 2, WHITE);
    Rectangle srcTexture = gunSizes[selectedGunType - 1];
    float scale = 5.4f * 2 * gunScale;
    Vector2 org = {srcTexture.width * scale / 2, srcTexture.height * scale / 2};
    Rectangle dst = {10.0f + 40.0f, SCREENHEIGHT - 90.0f + 40.0f, srcTexture.width*scale, srcTexture.height*scale};
    DrawTexturePro(spriteManager.sprite(1), srcTexture, dst, org, -45, WHITE);
    Vector2 mousePos = GetMousePosition();
    mousePos = Vector2Subtract(mousePos, {7, 7});
    DrawTextureEx(spriteManager.sprite(10), mousePos, 0, 2, WHITE);
    int width = MeasureText(TextFormat("%d", clips), 20);
    DrawText(TextFormat("%d", clips), SCREENWIDTH - width - 10, SCREENHEIGHT - 30, 20, WHITE);
    DrawText(TextFormat("%d", bullets), SCREENWIDTH - width - 15 - MeasureText(TextFormat("%d", bullets), 40*bulletScale), SCREENHEIGHT - 5 - 40*bulletScale, 40*bulletScale, WHITE);

    width = 300*healthScale;
    int height = 30;
    DrawRectangle(SCREENWIDTH / 2 - width / 2, SCREENHEIGHT - height - 10, width, height, Fade(WHITE, 0.3));
    DrawRectangleLinesEx({SCREENWIDTH / 2 - width / 2, SCREENHEIGHT - height - 10, width, height}, 2, WHITE);
    DrawRectangle(SCREENWIDTH / 2 - width / 2 + 5, SCREENHEIGHT - height - 10 + 5, (width - 10) * health, height - 10, Fade(foregroundColour, 0.8));
    
    Rectangle rect2 = {SCREENWIDTH/2 - 20, SCREENHEIGHT - height - 55, 40, 40};
    DrawRectangleLinesEx(rect2, 1, foregroundColour);
    DrawRectangleRec(rect2, Fade(foregroundColour, 0.2));
    float textDrawX = rect2.x + rect2.width/2 - MeasureText(TextFormat("%d", charges), 20)/2;
    float textDrawY = rect2.y + rect2.height/2 - 20/2;
    DrawText(TextFormat("%d", charges), textDrawX, textDrawY, 20, foregroundColour);

    DrawRectangleGradientV(0, SCREENHEIGHT - 40 * chargeScale, SCREENWIDTH, 40 * chargeScale, (Color){0, 0, 0, 0}, Fade(foregroundColour, 0.5));
}