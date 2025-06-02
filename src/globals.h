#pragma once

const bool DEBUG = false;
const bool soundEnabled = true;

const int SCREENWIDTH = 1200;
const int SCREENHEIGHT = 900;

const int tileDrawSize = 96;

const int MAX_EXPLOSIVES = 20;

static float delta = 0;

Color backgroundColour = Color{23, 0, 29, 255};
Color backgroundColour2 = Color{69, 3, 39, 255};
Color foregroundColour2 = Color{156, 23, 59, 255};
Color foregroundColour = Color{255, 5, 70, 255};

int manhattanDistance(Vector2 from, Vector2 to) {
    return fabs(from.x - to.x) + fabs(from.y - to.y);
}

float lerpAngle(float from, float to, float rate) {
    /*float result;
    if (from < 0) from += 2*PI;
    if (from > 2*PI) from -= 2*PI;
    if (to < 0) to += 2*PI;
    if (to > 2*PI) to -= 2*PI;

    float diff = from - to;

    if(diff < -PI) {
        to += 2*PI;
        result = Lerp(from, to ,value);
        if(result >= 2*PI) result -= 2*PI;
    } else if(diff > PI) {
        to -= 2*PI;
        result = Lerp(from, to ,value);
        if(result < 0) result += 2*PI;
    } else {
        result = Lerp(from, to, value);
    }


    return result;*/
    //TODO:: Make framerate indepenndent
    float difference = fabs(to - from);
    if (difference > PI)
    {
        if (to > from)
        {
            from += 2*PI;
        }
        else
        {
            to += 2*PI;
        }
    }
    float val = (from + ((to - from) * exp2(-rate*delta)));
    float rangeZero = 2*PI;

    if (val >= 0 && val <= 2*PI)
        return val;

    if (val < 0) return val+2*PI;
    if (val > 2*PI) return val - 2*PI;

    return 0;
}

const Rectangle gunSizes[] = {
    Rectangle{0,0,6,3},
    Rectangle{6, 0, 7, 4},
    Rectangle{13, 0, 12, 3},
    Rectangle{25, 0, 9, 4},
    Rectangle{34, 0, 12, 3},
    Rectangle{46, 0, 12, 5}
};

struct gunData {
    int numbullets;
    int clips;
    float damage;
    float firerate;
    float reloadTime;
    bool automatic;
    float inaccuracy;
    float kickback;
    int spreadNumber;
    float spreadAngle;
    float scope;
    float range;
};

const std::string gunNames[] = {
    "NONE", 
    "PISTOL",
    "SMG",
    "RIFLE",
    "SHOTGUN",
    "SNIPER",
    "RPG"
};

const gunData GunData[] = {
    {7, 49, 9, 0.1f, 1, false, 3*DEG2RAD, 4, 1, 0, 0.8f, 350},
    {30, 150, 3, 0.1f, 1, true, 8*DEG2RAD, 2, 1, 0, 1.25f, 300},
    {25, 175, 6, 0.18f, 1, true, 3*DEG2RAD, 3, 1, 0, 1, 400},
    {5, 25, 18, 0.5f, 1, false, 8*DEG2RAD, 6, 3, 5*DEG2RAD, 1.4f, 200},
    {5, 15, 20, 0.8f, 1, false, 1*DEG2RAD, 8, 1, 0, 0.6f, 600},
    {1, 10, 25, 1, 1, false, 4*DEG2RAD, 7, 1, 0, 0.75f, 500}
};

void drawBar(Rectangle rect, float ratio) {
    DrawRectangleRec(rect, Fade(WHITE, 0.3));
    DrawRectangleLinesEx(rect, 1, WHITE);
    float w = ratio * (rect.width - 2);
    DrawRectangleRec({rect.x + 1, rect.y + 1, w, rect.height - 2}, ratio > 0.25 ? WHITE : RED);
}

Vector2 lerp(Vector2 value, Vector2 target, float rate) {
    return Vector2Lerp(target, value, exp2(-rate*delta));
}

float lerp(float value, float target, float rate) {
    return Lerp(target, value, exp2(-rate*delta));
}
