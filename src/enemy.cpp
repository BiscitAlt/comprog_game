#include "enemy.h"
#include "raymath.h"


void InitEnemy(Enemy& e, Vector2 pos,std::string name, int hp, int atk)
{
    e.pos = pos; // กำหนดจุดเกิดเริ่มต้นของศัตรู
    e.size = { 18, 18 }; // ขนาดของศัตรู
    e.speed = 1.0f; // ความเร็ว
    e.hpMax = 60; // เลือด
    e.hp = e.hpMax;
    e.atk = 4;
    e.attackTimer = 0.0f;
    e.color = BLUE;
    e.expDrop = 10;   // ศัตรูตัวนี้ให้ exp 10
}

void UpdateEnemy(Enemy& e, Vector2 playerPos)
{
    if (e.hp <= 0) return;
    Vector2 dir = Vector2Subtract(playerPos, e.pos); // 

    if (Vector2Length(dir) > 1) 
    {
        dir = Vector2Normalize(dir); 
        e.pos = Vector2Add(e.pos, Vector2Scale(dir, e.speed)); // เคลื่อนที่ไปหาผู้เล่น
    }
}

void DrawEnemy(const Enemy& e)
{
    // วาดตัวศัตรู
    DrawRectangleV(e.pos, e.size, e.color);

    // ===== HP BAR =====
    float barWidth = e.size.x;
    float barHeight = 4;

    float hpPercent = (float)e.hp / e.hpMax;
    if (hpPercent < 0) hpPercent = 0;

    Vector2 barPos = {
        e.pos.x,
        e.pos.y - 8
    };

    // พื้นหลังหลอดเลือด
    DrawRectangle(barPos.x, barPos.y, barWidth, barHeight, DARKGRAY);

    // เลือดปัจจุบัน
    DrawRectangle(
        barPos.x,
        barPos.y,
        barWidth * hpPercent,
        barHeight,
        RED
    );
}