#pragma once

class LevelMaker {
private:
    Color mapCells[50][50] = { 0 };
    camera cam;
    Tilemap tiles = Tilemap(0, Vector2{16.0f, 16.0f});
    Vector2 camPos = { 0 };
public:
    LevelMaker();
    ~LevelMaker();

    void draw();
    void update();
    void newLevel();
    void saveLevel(std::string path);
};

LevelMaker::LevelMaker() {
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 50; j++) {
            mapCells[i][j] = Color{0,0,0,0};
        }
    }
    tiles.clear();
    cam.setZoom(1);
}

LevelMaker::~LevelMaker() {
}

void LevelMaker::draw() {
    BeginMode2D(cam.cam);
    tiles.draw();
    EndMode2D();
}

void LevelMaker::update() {
    if (IsMouseButtonDown(0)) {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), cam.cam);
        Vector2 pointClicked = {floor(mousePos.x/96), floor(mousePos.y/96)};
        std::cout<<mousePos.x<<" "<<mousePos.y<<"\t"<<pointClicked.x<<" "<<pointClicked.y<<"\n";
        if (pointClicked.x < 50 && pointClicked.y < 50 && pointClicked.x >= 0 && pointClicked.y >= 0) {
            if (tiles.getTile({pointClicked.x, pointClicked.y}) != 4) {
                tiles.setTile({pointClicked.x, pointClicked.y}, 4);
                mapCells[(int)mousePos.x][(int)mousePos.y] = Color{0,0,0,1};
            }
        }
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) and IsKeyPressed(KEY_S)) {
        saveLevel("lvl_1.png");
    }

    if (IsKeyDown(KEY_A)) {
        camPos.x -= 5;
    } else if (IsKeyDown(KEY_D)) {
        camPos.x += 5;
    }
    if (IsKeyDown(KEY_W)) {
        camPos.y -= 5;
    } else if (IsKeyDown(KEY_S)) {
        camPos.y += 5;
    }
    cam.zoomBy(GetMouseWheelMove()/5);
    cam.update(camPos);
}

inline void LevelMaker::newLevel() {
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 50; j++) {
            mapCells[i][j] = Color{0,0,0,0};
        }
    }
    tiles.clear();
}

inline void LevelMaker::saveLevel(std::string path) {
    Image img = GenImageColor(50, 50, {0,0,0,0});
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 50; j++) {
            ImageDrawPixel(&img, j, i, mapCells[i][j]);
        }
    }
    ExportImage(img, path.c_str());
    UnloadImage(img);
}
