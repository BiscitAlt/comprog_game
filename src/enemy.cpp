#include "enemy.h"
#include "raymath.h"


void InitEnemy(Enemy& e, Vector2 pos,std::string name, int hp, int atk)
{
    e.pos = pos; // กำหนดจุดเกิดเริ่มต้นของศัตรู
    e.size = { 18, 18 }; // ขนาดของศัตรู
    e.speed = 1.0f; // ความเร็ว
    e.hp = hp; // เลือด
    e.color = BLUE;
    e.namemonster = name;
    e.hp = hp;
    e.maxHp = hp;   // ตั้งค่าเลือดสูงสุดให้เท่ากับเลือดที่เกิดมาก่อนยังไม่โดนตี
    e.attack = atk;
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

void DrawEnemy(const Enemy& e) // วาด
{
    DrawRectangleV(e.pos, e.size, e.color);
}