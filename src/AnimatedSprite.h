#pragma once

extern SpritesManager spriteManager;

class AnimatedSprite
{
private:
    int spriteSheetID;
    float frameTime;
    int numAnimations;
    int animationID = 0;
    int* totalFrames;
    float counter = 0;
    int width;
    int height;
    int swidth;
    int sheight;
    bool paused = false;
    public:
    int frame = 0;
    AnimatedSprite(int sprite, int numAnims, int* frames, int FPS);
    ~AnimatedSprite();
    void play(int animID);
    void playOnce(int animID);
    void update();
    void draw(Vector2 position);
    void draw(Vector2 position, Color tint);
    void refresh();
    Vector2 getSize();
    
    void debugText();
    
    bool playingOnce = false;
    bool flipH = false;
    float scale = 1;
};

AnimatedSprite::AnimatedSprite(int sprite, int numAnims, int* frames, int FPS) {
    spriteSheetID = sprite;
    numAnimations = numAnims;
    totalFrames = frames;
    frameTime = 1/(float)FPS;
}

AnimatedSprite::~AnimatedSprite()
{
}

inline void AnimatedSprite::play(int animID)
{
    paused = false;
    if(animID == animationID) return;
    animationID = animID;
    frame = 0;
    counter = 0;
}

inline void AnimatedSprite::playOnce(int animID) {
    playingOnce = true;
    play(animID);
}

inline void AnimatedSprite::update()
{
    if (paused) return;
    counter += GetFrameTime();
    if (counter >= frameTime) {
        frame++;
        counter = 0;
        if (frame == totalFrames[animationID]) {
            if (playingOnce) {
                paused = true;
                frame--;
            }
            else frame -= totalFrames[animationID];
        }
    }
}

inline void AnimatedSprite::draw(Vector2 position) {
    int k = 1;
    if(flipH) k = -1;
    Rectangle sourceRec = { (float)frame*swidth, (float)animationID*sheight, (float)k*swidth, (float)sheight};
    Rectangle dstRec = { position.x - swidth*scale/2, position.y - sheight*scale/2, swidth*scale, sheight*scale};
    DrawTexturePro(spriteManager.sprite(spriteSheetID), sourceRec, dstRec, Vector2Zero(), 0, WHITE);
}

inline void AnimatedSprite::refresh()
{
    height = spriteManager.sprite(spriteSheetID).height;
    width = spriteManager.sprite(spriteSheetID).width;
    int max = totalFrames[0];
    for (int i = 0; i < numAnimations; i++) if (totalFrames[i] > max) max = totalFrames[i];
    swidth = width/max;
    sheight = height/numAnimations;
}

inline Vector2 AnimatedSprite::getSize()
{
    return Vector2{(float)swidth*scale, (float)sheight*scale};
}

inline void AnimatedSprite::debugText() {
    std::cout<<"\n######\n";
    std::cout<<"Width:"<<width<<"  Height:"<<height<<std::endl;
    std::cout<<"Sprite Dimensions:"<<swidth<<"x"<<sheight<<std::endl;
}

inline void AnimatedSprite::draw(Vector2 position, Color tint) {
    int k = 1;
    if(flipH) k = -1;
    Rectangle sourceRec = { (float)frame*swidth, (float)animationID*sheight, (float)k*swidth, (float)sheight};
    Rectangle dstRec = { position.x - swidth*scale/2, position.y - sheight*scale/2, swidth*scale, sheight*scale};
    BeginShaderMode(flashShader);
    DrawTexturePro(spriteManager.sprite(spriteSheetID), sourceRec, dstRec, Vector2Zero(), 0, WHITE);
    EndShaderMode();
}