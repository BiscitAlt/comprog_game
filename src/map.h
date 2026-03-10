#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include <vector>
#include <cmath>

struct Obstacle {
    Vector2 pos;
    int ID;
};

struct Decoration {
    Vector2 pos;
    int ID; 
};

struct BlueprintPiece {
    int offsetX;
    int offsetY;
    int ID;
};

struct DecorationDef {
    std::vector<BlueprintPiece> pieces;
    bool multiTile = false; // false = pick one random piece; true = place all pieces
};

class Map {
public:
    static const int tileSize = 32;
    Texture2D tileset;
    
    std::vector<Obstacle> obstacles;
    std::vector<Decoration> decorations;

    void LoadAssets();
    void UnloadAssets();
    void UpdateMap(Vector2 playerPos); 
    void Draw(Vector2 playerPos, int screenWidth, int screenHeight); 

    bool IsWall(float x, float y);
    bool HitSpike(float x, float y);
};

#endif