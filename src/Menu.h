void drawMenuBackground(std::string menuName) {
    ClearBackground(backgroundColour);
    DrawTextPro(GetFontDefault(), menuName.c_str(), {20, SCREENHEIGHT - 20}, {0,0}, -90, 80, 10, foregroundColour2);
}

bool drawButton(Rectangle rect, std::string text, int textSize, Color colour) {
    DrawRectangleLinesEx(rect, 1, colour);
    DrawRectangleRec(rect, Fade(colour, 0.2));
    float textDrawX = rect.x + rect.width/2 - MeasureText(text.c_str(), textSize)/2;
    float textDrawY = rect.y + rect.height/2 - textSize/2;
    DrawText(text.c_str(), textDrawX, textDrawY, textSize, colour);
    return CheckCollisionPointRec(GetMousePosition(), rect) and IsMouseButtonReleased(0);
}

int drawLevelButtons(Rectangle rect, int numButtons) {
    int x = 0;
    for (short i = 0; i < numButtons; i++)
    {
        float delX = i%4 * 80.0f;
        float delY = (int)(i/4)*80.0f;
        x = drawButton({rect.x + delX, rect.y + delY, 60, 60}, std::to_string(i+1), 30, foregroundColour2);
        if (x == 1) return i;
    }
    return -1;
}