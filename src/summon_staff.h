#pragma once
#include "raylib.h"
#include <vector>
#include "summon.h"

struct SummonStaff
{
    Vector2 pos;          // ตำแหน่งบนพื้น
    Vector2 size;
    bool pickedUp;        // ถูกเก็บหรือยัง

    float cooldown;       // คูลดาวน์เรียก
    float timer;          // ตัวนับเวลา
};

// init คทาบนพื้น
void InitSummonStaff(SummonStaff& staff, Vector2 pos);

// update (pickup + cooldown)
void UpdateSummonStaff(SummonStaff& staff,
                       Vector2 plPos,
                       Vector2 plSize,
                       float dt);

// ใช้งานคทา (เรียกซัมมอน)
void UseSummonStaff(SummonStaff& staff,
                    std::vector<Summon>& summons,
                    Vector2 plPos);

// วาด
void DrawSummonStaff(const SummonStaff& staff,
                     Vector2 plPos,
                     Vector2 dir);