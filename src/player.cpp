#include "player.h"
#include "raymath.h"

// ======================
// Update Player
// ======================
void plUpdate(player &pl, Map &map)
{
    // เก็บตำแหน่งปัจจุบันไว้ก่อน หากเดินชนกำแพงจะได้เด้งกลับมาจุดนี้
    Vector2 oldPos = pl.pos; 
    
    plMovement(pl.pos, pl.speed);

    // เช็คไม่ให้หลุดขอบแผนที่
    plCollision(pl.pos, pl.size, pl.speed, map);

    // เช็คว่าตำแหน่งใหม่ชนกำแพงหรือไม่ (เช็คจาก 4 มุมของตัวละคร)
    bool hitWall = map.IsWall(pl.pos.x, pl.pos.y) || 
                   map.IsWall(pl.pos.x + pl.size.x, pl.pos.y) || 
                   map.IsWall(pl.pos.x, pl.pos.y + pl.size.y) || 
                   map.IsWall(pl.pos.x + pl.size.x, pl.pos.y + pl.size.y);

    // ถ้าตำแหน่งใหม่เป็นกำแพง ให้ดึงตัวละครกลับมาจุดก่อนเดิน
    if (hitWall) {
        pl.pos = oldPos;
    }
}

// ======================
// Movement
// ======================
void plMovement(Vector2 &plPos, float speed)
{
    if (IsKeyDown(KEY_W)) plPos.y -= speed;
    if (IsKeyDown(KEY_S)) plPos.y += speed;
    if (IsKeyDown(KEY_A)) plPos.x -= speed;
    if (IsKeyDown(KEY_D)) plPos.x += speed;
}

// ======================
// Collision (พึ่งพา map.IsWall ใน plUpdate เพื่อกันชนสิ่งกีดขวางอย่างเดียว)
// ======================
void plCollision(Vector2 &plPos, Vector2 plSize, float plSpeed, Map &map)
{
    // คำนวณขนาดที่แท้จริงของแผนที่ (960 x 960)
//    float mapWidth = map.cols * map.tileSize;
//   float mapHeight = map.rows * map.tileSize;

    // กันหลุดขอบซ้าย-บน
//    if (plPos.x < 0) plPos.x = 0;
//    if (plPos.y < 0) plPos.y = 0;

    // กันหลุดขอบขวา-ล่าง (ใช้ขนาดแผนที่แทน GetScreenWidth/Height)
//    if (plPos.x > mapWidth - plSize.x)
//        plPos.x = mapWidth - plSize.x;

//    if (plPos.y > mapHeight - plSize.y)
//        plPos.y = mapHeight - plSize.y;
}

// ======================
// Draw Player
// ======================
void DrawPlayer(player &pl)
{
    DrawRectangleV(pl.pos, pl.size, pl.color);
}