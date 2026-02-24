#pragma once
#include "raylib.h"

// ========================
// Summon Entity
// ========================
struct Summon
{
    Vector2 pos;        // ตำแหน่ง
    float radius;       // ขนาดชน
    int damage;         // ดาเมจ
    float speed;        // ความเร็ว
    float lifetime;     // อายุ
    bool active;        // ยังมีอยู่ไหม
};

// สร้างซัมมอน
void InitSummon(Summon& s, Vector2 pos);

// อัปเดตพฤติกรรม (ไล่เป้า)
void UpdateSummon(Summon& s, float dt, Vector2 targetPos);

// วาดซัมมอน
void DrawSummon(const Summon& s);