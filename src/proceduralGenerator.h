#pragma once

namespace Generator {

    Rectangle rooms[100];
    int roomsCount = 0;

    int maxSteps = 1500;
    int maxStepsPerTurn = 4;
    int stepChangeFrequency = 2;
    int roomFrequency = 20;

    bool isValidPoint(Vector2 point) {
        bool invalid = point.x <= 0 || point.y <= 0 || point.x >= 99 || point.y >= 99;
        return !invalid;
    }
    
    void createRoom(Vector2* walkedAreas, int& totalSteps, Vector2 position) {
        int width = GetRandomValue(3, 5);
        int height = GetRandomValue(3, 5);
        int posX = ceil(position.x - width / 2);
        int posY = ceil(position.y - height / 2);
        Rectangle room = {posX, posY, width, height};
        for (int i = 0; i < roomsCount; i++) {
            if (CheckCollisionRecs(rooms[i], room)) return;
        }
        for (int i = posX; i < posX + width; i++) {
            for (int j = posY; j < posY + height; j++) {
                if (totalSteps == maxSteps - 1) return;
                Vector2 newPoint = (Vector2){(float)i, (float)j};
                if (isValidPoint(newPoint))
                walkedAreas[totalSteps++] = newPoint;
            }
        }
        rooms[roomsCount++] = room;
    }
    
    void changeDirection(int* direction) {
        int newDirection;
        do {
            newDirection = GetRandomValue(0, 3);
        } while (newDirection == *direction);
        *direction = newDirection;
    }

    Vector2 toDir(int dir) {
        switch (dir) {
            case 0:
            return {1, 0};
            
            case 1:
            return {0, -1};

            case 2:
            return {-1, 0};

            case 3:
            return {0, 1};
        }
        return {1, 0};
    }

    void placeCollectible(mapData* data, Vector2 position) {
        data->collectiblePositions[data->numCollectibleSpawnPositions] = Vector2Add(Vector2Scale(position, tileDrawSize), {tileDrawSize/2, tileDrawSize/2});
        int val = GetRandomValue(0, 3);
        data->collectibleTypes[data->numCollectibleSpawnPositions] = val;
        int dat = GetRandomValue(0, 6);
        if (val != 0) dat = GetRandomValue(40, 80);
        data->collectibleData[data->numCollectibleSpawnPositions] = dat; 
        data->numCollectibleSpawnPositions++;
    }

    void placeEnemy(mapData* data, Vector2 position) {
        data->enemySpawnPositions[data->numEnemySpawnPositions] = Vector2Add(Vector2Scale(position, tileDrawSize), {tileDrawSize/2, tileDrawSize/2});
        data->enemyTypes[data->numEnemySpawnPositions] = GetRandomValue(0, 2);
        data->numEnemySpawnPositions++;
    }

    mapData generateLevelnew(int seed, Tilemap* tilemap) {
        SetRandomSeed(seed);
        tilemap->clear();
        mapData data = { 0 };
        int numWalkers = 1;
        Vector2 walkers[5] = { 0 };
        int walkerDirections[5] = { 0 };
        int stepsSinceLastTurn[5] = { 0 };
        int totalSteps = 0;

        walkers[0] = {GetRandomValue(30, 70), GetRandomValue(30, 70)};
        walkerDirections[0] = GetRandomValue(0, 3);
        
        Vector2* walkedAreas = (Vector2*)malloc(maxSteps*sizeof(Vector2));
        while (totalSteps < maxSteps) {
            for (int i = 0; i < numWalkers; i++) {
                walkedAreas[totalSteps++] = walkers[i];
                if (totalSteps == maxSteps) break;
                if (GetRandomValue(0, 100) < stepChangeFrequency || stepsSinceLastTurn[i] >= maxStepsPerTurn) {
                    changeDirection(&walkerDirections[i]);
                    stepsSinceLastTurn[i] = 0;
                } else {
                    Vector2 targetPosition = Vector2Add(walkers[i], toDir(walkerDirections[i]));
                    while (!isValidPoint(targetPosition)) {
                        changeDirection(&walkerDirections[i]);
                        targetPosition = Vector2Add(walkers[i], toDir(walkerDirections[i]));
                    }
                    walkers[i] = targetPosition;
                    stepsSinceLastTurn[i]++;
                }
                if (GetRandomValue(0, 100) < 3 && numWalkers < 5) {
                    walkers[numWalkers] = walkedAreas[GetRandomValue(0, totalSteps-1)];
                    walkerDirections[numWalkers] = GetRandomValue(0, 3);
                    numWalkers++;
                }
                if (GetRandomValue(0, 100) < 5 && data.numEnemySpawnPositions < 39) {
                    if (manhattanDistance(walkers[i], walkedAreas[0]) > 15)
                        placeEnemy(&data, walkers[i]);
                }
                if (GetRandomValue(0, 100) < 5 && data.numCollectibleSpawnPositions < 39) {
                    if (manhattanDistance(walkers[i], walkedAreas[0]) > 7)
                        placeCollectible(&data, walkers[i]);
                }
            }
        }
        if (IsRenderTextureValid(data.minimap)) UnloadRenderTexture(data.minimap);
        data.minimap = LoadRenderTexture(100, 100);
        data.playerPosition = Vector2Add(Vector2Scale(walkedAreas[0], tileDrawSize), {tileDrawSize/2, tileDrawSize/2});
        BeginTextureMode(data.minimap);
        DrawRectangle(0, 0, 100, 100, backgroundColour2);
        for (int i = 0; i < maxSteps; i++) {
            tilemap->setTile(walkedAreas[i], GetRandomValue(8, 11));
            DrawPixelV(walkedAreas[i], foregroundColour2);
        }
        for (int i = 0; i < data.numEnemySpawnPositions; i++) {
            DrawPixelV(Vector2Scale(Vector2Subtract(data.enemySpawnPositions[i], {tileDrawSize/2, tileDrawSize/2}), 1.0/tileDrawSize), foregroundColour2);
        }
        EndTextureMode();
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                if (tilemap->getTile({i, j}) <= 3 && tilemap->getTile({i, j+1}) > 7) {
                    tilemap->setTile({i, j+1}, GetRandomValue(4, 7));
                }
            }
        }

        free(walkedAreas);
        printf("Enemies: %d\tCollectibles: %d\n", data.numEnemySpawnPositions, data.numCollectibleSpawnPositions);

        return data;
    }

    mapData generateLevel(int seed, Tilemap* tilemap) {
        SetRandomSeed(seed);
        tilemap->clear();

        mapData data = { 0 };

        Vector2 walkerPosition = {GetRandomValue(1, 49), GetRandomValue(1, 49)};
        Vector2 targetPosition = { 0 };
        int walkerDirection = GetRandomValue(0, 3);
        int stepsSinceLastTurn = 0;
        int totalSteps = 0;

        int requiredSteps = maxSteps;

        Vector2* walkedAreas = (Vector2*)malloc(requiredSteps*sizeof(Vector2));

        createRoom(walkedAreas, totalSteps, walkerPosition);
        while (totalSteps < requiredSteps) {
            if (GetRandomValue(0, 100) < 5 && data.numEnemySpawnPositions < 31) {
                placeEnemy(&data, walkerPosition);
            }
            if (totalSteps > 800) {
                if (GetRandomValue(0, 100) < 5 && data.numCollectibleSpawnPositions < 31) {
                    placeCollectible(&data, walkerPosition);
                }
            }
            walkedAreas[totalSteps++] = walkerPosition;
            if (GetRandomValue(0, 100) < roomFrequency) createRoom(walkedAreas, totalSteps, walkerPosition);
            if (GetRandomValue(0, 100) < stepChangeFrequency && stepsSinceLastTurn >= maxStepsPerTurn) {
                changeDirection(&walkerDirection);
                stepsSinceLastTurn = 0;
            } else {
                targetPosition = Vector2Add(walkerPosition, toDir(walkerDirection));
                while (!isValidPoint(targetPosition)) {
                    changeDirection(&walkerDirection);
                    targetPosition = Vector2Add(walkerPosition, toDir(walkerDirection));
                }
                walkerPosition = targetPosition;
                stepsSinceLastTurn++;
            }
        }

        data.playerPosition = Vector2Add(Vector2Scale(walkedAreas[totalSteps - 1], tileDrawSize), {tileDrawSize/2, tileDrawSize/2});
        for (int i = 0; i < requiredSteps; i++) {
            tilemap->setTile(walkedAreas[i], GetRandomValue(8, 11));
        }
        for (int i = 0; i < 50; i++) {
            for (int j = 0; j < 50; j++) {
                if (tilemap->getTile({i, j}) <= 3 && tilemap->getTile({i, j+1}) > 7) {
                    tilemap->setTile({i, j+1}, GetRandomValue(4, 7));
                }
            }
        }

        free(walkedAreas);

        return data;
    }
}
