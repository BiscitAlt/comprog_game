#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include <vector>
#include <cmath>

// สร้าง Struct เก็บข้อมูลสิ่งกีดขวางแต่ละชิ้น
struct Obstacle {
    Vector2 pos;
    int type; // สำหรับเก็บ tileID
};
class Map {
public:
    static const int tileSize = 32;
    Texture2D tileset;
    
    std::vector<Obstacle> obstacles; // เก็บสิ่งกีดขวางแบบไดนามิก

    Map();
    void LoadAssets();
    void UnloadAssets();

    // ฟังก์ชันใหม่สำหรับอัปเดต Endless Map
    void UpdateEndless(Vector2 playerPos); 
    bool IsWall(float x, float y);
    
    // ต้องรับตำแหน่งผู้เล่นและหน้าจอมาเพื่อวาดพื้นให้พอดีกล้อง
    void Draw(Vector2 playerPos, int screenWidth, int screenHeight); 
};

#endif