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
    Rectangle GunRect = { 0 };
    float t = 0;
public:
    void update(gunType _selectedGunType, float health, int bullets, int clips, int _charges);
    void draw();
    void drawReloadingEffect(float progress);
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
    t += GetFrameTime();
    if (t >= 2 * PI) t -= 2 * PI;

    Rectangle srcTexture = gunSizes[selectedGunType - 1];
    float scale = 5.4 * 2 * gunScale + 0.5 * sin(1.2f * PI * t);
    Vector2 org = Vector2Zero();//{srcTexture.width * scale / 2, srcTexture.height * scale / 2};
    float yOffset = 10;
    float drawWidth = srcTexture.width * scale;
    float drawHeight = srcTexture.height * scale;
    int clipsWidth = MeasureText(TextFormat("%d", clips), 20);
    int bulletsWidth = MeasureText(TextFormat("%d", bullets), 40*bulletScale);
    GunRect.x = SCREENWIDTH - clipsWidth - 20 - bulletsWidth - drawWidth - 20;
    GunRect.width = clipsWidth + bulletsWidth + drawWidth + 30;
    GunRect.height = drawHeight + 20;
    GunRect.y = SCREENHEIGHT - GunRect.height - 10;
    DrawRectangle(GunRect.x, GunRect.y, GunRect.width, GunRect.height, Fade(WHITE, 0.2));
    DrawRectangleLinesEx({GunRect.x, GunRect.y, GunRect.width, GunRect.height}, 1, WHITE);
    Rectangle dst = {SCREENWIDTH - clipsWidth - bulletsWidth - 30 - drawWidth, SCREENHEIGHT - 10 - drawHeight - yOffset, drawWidth, drawHeight};//{SCREENWIDTH - 15.0f, SCREENHEIGHT - 55.0f, srcTexture.width*scale, srcTexture.height*scale};
    BeginShaderMode(flashShader);
    DrawTexturePro(spriteManager.sprite(1), srcTexture, dst, org, 0, WHITE);
    EndShaderMode();
    Vector2 mousePos = GetMousePosition();
    mousePos = Vector2Subtract(mousePos, {7, 7});
    DrawTextureEx(spriteManager.sprite(10), mousePos, 0, 2, WHITE);
    DrawText(TextFormat("%d", clips), SCREENWIDTH - clipsWidth - 15, SCREENHEIGHT - 48, 20, WHITE);
    DrawText(TextFormat("%d", bullets), SCREENWIDTH - clipsWidth - 20 - bulletsWidth, SCREENHEIGHT - 10 - 40*bulletScale, 40*bulletScale, WHITE);

    float delta = 100 - GunRect.height;
    if (selectedGunType > 0) drawBar({GunRect.x, GunRect.y - delta, GunRect.width, delta - 5}, (float)bullets / (float)GunData[selectedGunType - 1].numbullets);

    clipsWidth = 300*healthScale;
    int height = 30;
    DrawRectangle(SCREENWIDTH / 2 - clipsWidth / 2, SCREENHEIGHT - height - 10, clipsWidth, height, Fade(WHITE, 0.3));
    DrawRectangleLinesEx({SCREENWIDTH / 2 - clipsWidth / 2, SCREENHEIGHT - height - 10, clipsWidth, height}, 2, WHITE);
    DrawRectangle(SCREENWIDTH / 2 - clipsWidth / 2 + 5, SCREENHEIGHT - height - 10 + 5, (clipsWidth - 10) * health, height - 10, Fade(foregroundColour, 0.8));
    
    Rectangle rect2 = {SCREENWIDTH/2 - 20, SCREENHEIGHT - height - 55, 40, 40};
    DrawRectangleLinesEx(rect2, 1, foregroundColour);
    DrawRectangleRec(rect2, Fade(foregroundColour, 0.2));
    float textDrawX = rect2.x + rect2.width/2 - MeasureText(TextFormat("%d", charges), 20)/2;
    float textDrawY = rect2.y + rect2.height/2 - 20/2;
    DrawText(TextFormat("%d", charges), textDrawX, textDrawY, 20, foregroundColour);

    DrawRectangleGradientV(0, SCREENHEIGHT - 40 * chargeScale, SCREENWIDTH, 40 * chargeScale, (Color){0, 0, 0, 0}, Fade(foregroundColour, 0.5));
}

inline void HUD::drawReloadingEffect(float progress) {
    //DrawRectangle(GunRect.x + 5, GunRect.y + 5, (GunRect.width - 10) * progress, GunRect.height - 10, Fade(foregroundColour2, 0.9));
}