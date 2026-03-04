#include "enemy.h"
#include "raymath.h"

void InitEnemy(Enemy& e, Vector2 pos)
{
    e.pos = pos; // กำหนดจุดเกิดเริ่มต้นของศัตรู
    e.size = { 18, 18 }; // ขนาดของศัตรู
    e.speed = 1.0f; // ความเร็ว
    e.hpMax = 300; // เลือด
    e.hp = e.hpMax;
    e.atk = 4;
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

    float dt = GetFrameTime();
    // ===== Burn (Fire DoT) =====
    if (e.burnTimer > 0)
    {
    e.burnTimer -= dt;
    e.hp -= 10 * dt; // 10 damage/sec
    }

     // ===== Freeze (หยุดทุกอย่างก่อน) =====
    if (e.freezeTimer > 0)
    {
        e.freezeTimer -= dt;
        return; // ไม่ขยับเลย
    }

    // ===== Slow =====
    float speedModifier = 1.0f;

    if (e.slowTimer > 0)
    {
    e.slowTimer -= dt;
    speedModifier *= 0.6f;
    }
    if (e.electrifiedTimer > 0)
    {
    e.electrifiedTimer -= dt;
    speedModifier *= 0.85f; // ช้าลง 15%
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