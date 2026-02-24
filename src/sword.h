#ifndef SWORD_H
#define SWORD_H

#include "raylib.h"

// =======================
// โครงสร้างดาบ
// =======================
struct Sword
{
    Vector2 pos;        // ตำแหน่งดาบ (ตอนอยู่บนพื้น)
    bool pickedUp;      // ถูกเก็บแล้วหรือยัง
    bool swinging;      // กำลังฟันอยู่ไหม
    float swingTimer;   // เวลาอนิเมชันฟัน
};

// ฟังก์ชันดาบ
void InitSword(Sword& s, Vector2 pos);

void UpdateSword(
    Sword& s,
    Vector2 playerPos,
    Vector2 playerSize,
    Vector2 dir
);

Rectangle GetSwordHitbox(
    const Sword& s,
    Vector2 playerPos,
    Vector2 playerSize,
    Vector2 dir
);

void DrawSword(
    const Sword& s,
    Vector2 playerPos,
    Vector2 playerSize,
    Vector2 dir
);

#endif