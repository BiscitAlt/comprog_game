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
// Draw Player
// ======================
void DrawPlayer(player &pl)
{
    DrawRectangleV(pl.pos, pl.size, pl.color);
}