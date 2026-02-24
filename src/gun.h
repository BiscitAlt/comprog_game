#pragma once
#include <vector>
#include "raylib.h"
#include "bullet.h"

// โครงสร้างปืน
struct Gun
{
    Vector2 pos;        // ตำแหน่งปืนบนแมพ
    Vector2 size;       // ขนาดปืน
    bool pickedUp;      // ถูกเก็บแล้วหรือยัง

    float fireRate;     // เวลาระหว่างการยิง
    float fireTimer;    // ตัวจับเวลา cooldown
    float bulletSpeed;  // ความเร็วกระสุน
};

// สร้างปืน (วางบนพื้น)
void InitGun(Gun& gun, Vector2 pos);

// อัปเดตปืน (เก็บ + ลด cooldown)
void UpdateGun(Gun& gun, Vector2 plPos, Vector2 plSize);

// ยิงปืน (กระสุนไม่จำกัด)
void ShootGun(Gun& gun,
              std::vector<Bullet>& bullets,
              Vector2 plPos,
              Vector2 plSize,
              Vector2 dir);

// วาดปืน (พื้น / ถือ)
void DrawGun(const Gun& gun,
             Vector2 plPos,
             Vector2 plSize,
             Vector2 dir);