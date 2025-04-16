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
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Netherium Mines");
    InitAudioDevice();
    game.init();
    SetTargetFPS(GetMonitorRefreshRate(0));
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
