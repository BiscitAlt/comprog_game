#include "enemy.h"
#include "raymath.h"

void InitEnemy(Enemy& e, Vector2 pos)
{
    e.pos = pos; // กำหนดจุดเกิดเริ่มต้นของศัตรู
    e.size = { 18, 18 }; // ขนาดของศัตรู
    e.speed = 1.0f; // ความเร็ว
    e.hp = 2; // เลือด
    e.atk = 10;
    e.attackTimer = 0.0f;
    e.color = BLUE;
}

void UpdateEnemy(Enemy& e, Vector2 playerPos)
{
    Vector2 dir = Vector2Subtract(playerPos, e.pos); // 

    if (Vector2Length(dir) > 1) 
    {
        dir = Vector2Normalize(dir); 
        e.pos = Vector2Add(e.pos, Vector2Scale(dir, e.speed)); // เคลื่อนที่ไปหาผู้เล่น
    }
}

void DrawEnemy(const Enemy& e) // วาด
{
    DrawRectangleV(e.pos, e.size, e.color);
}