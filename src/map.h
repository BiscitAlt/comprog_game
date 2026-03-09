#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include <vector>
#include <cmath>

// Struct เก็บข้อมูลสิ่งกีดขวางแต่ละชิ้น
struct Obstacle {
    Vector2 pos;
    int ID;
};

// Struct สำหรับเก็บข้อมูลของตกแต่ง
struct Decoration {
    Vector2 pos;
    int ID; 
};

struct BlueprintPiece {
    int offsetX;
    int offsetY;
    int ID;
};

// DecorationDef — describes one decoration type.
struct DecorationDef {
    std::vector<BlueprintPiece> pieces;
    bool multiTile = false; // false = pick one random piece; true = place all pieces
};

class Map {
public:
    static const int tileSize = 32;
    Texture2D tileset;
    
    std::vector<Obstacle> obstacles; // เก็บสิ่งกีดขวาง
    std::vector<Decoration> decorations; // เก็บข้อมูลของตกแต่ง

    // ตั้งค่า Ravine
    float ravineBaseX = 0.0f;  // world-space X centre of the river
    static constexpr int RAVINE_HALF_WIDTH = 2;   // river width in tiles
    static constexpr int CROSSING_EVERY = 25;  // gap every N tile-rows
    static constexpr int CROSSING_WIDTH = 4;   // gap width in tiles

    void LoadAssets();
    void UnloadAssets();
    void UpdateMap(Vector2 playerPos); 
    void Draw(Vector2 playerPos, int screenWidth, int screenHeight); 

    bool IsWall(float x, float y);
    bool HitSpike(float x, float y);

    // ----------------------------------------------------------
    // Ravine helpers (deterministic — same input → same output)
    // ----------------------------------------------------------
    float GetRavineCenterX(int tileY) const; // winding centre X for a given tile row
    bool  IsRavineTile(int tileX, int tileY) const;
    bool  IsCrossingRow(int tileY) const;   // true = walkable ford gap
    Color GetRavineTint(int distFromCenter) const; // darkness tint for a tile's distance from river centre
};

#endif