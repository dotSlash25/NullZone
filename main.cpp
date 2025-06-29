#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include "src/globals.h"
#include "src/game.h"

Game game = Game();

int main() {
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "NullZone");
    InitAudioDevice();
    if (!soundEnabled) SetMasterVolume(0);
    else SetMasterVolume(100);
    game.init();
    SetTargetFPS(144);
    while(!WindowShouldClose()) {
        game.update();
        
        BeginDrawing();
        ClearBackground(BLACK);
        game.draw();
        EndDrawing();
    }
    game.unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
