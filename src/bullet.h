#pragma once
#include "raylib.h"

enum class BulletType
{
    NORMAL,
    LASER,
    ROCKET
};

struct Bullet
{
    Vector2 pos;
    Vector2 vel;
    float radius;
    bool active;

    BulletType type;

    float lifeTime;

    // ===== ใช้เฉพาะ Laser =====
    int bounceLeft;     // จำนวนครั้งที่เด้งได้
    float length;       // ความยาวเส้นเลเซอร์
};

void UpdateBullet(Bullet& b);
void DrawBullet(const Bullet& b);