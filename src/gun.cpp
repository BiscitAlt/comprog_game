#include "gun.h"
#include "raymath.h"   // ✅ เพิ่มบรรทัดนี้

// สร้างปืนและตั้งค่าเริ่มต้น
void InitGun(Gun& gun, Vector2 pos)
{
    gun.pos = pos;
    gun.size = { 24, 12 };
    gun.pickedUp = false;

    gun.fireRate = 0.2f;     // ยิงได้ทุก 0.2 วินาที
    gun.fireTimer = 0.0f;
    gun.bulletSpeed = 600.0f;
}

// อัปเดตสถานะปืน
void UpdateGun(Gun& gun, Vector2 plPos, Vector2 plSize)
{
    // ลด cooldown การยิง
    if (gun.fireTimer > 0)
        gun.fireTimer -= GetFrameTime();

    // ถ้าเก็บแล้ว ไม่ต้องเช็คชน
    if (gun.pickedUp) return;

    // กล่องชนของปืน
    Rectangle gunRec = {
        gun.pos.x, gun.pos.y,
        gun.size.x, gun.size.y
    };

    // กล่องชนของผู้เล่น
    Rectangle plRec = {
        plPos.x, plPos.y,
        plSize.x, plSize.y
    };

    // เดินชน = เก็บปืน
    if (CheckCollisionRecs(gunRec, plRec))
    {
        gun.pickedUp = true;
    }
}

// ยิงปืน (สร้างกระสุน)
void ShootGun(Gun& gun,
              std::vector<Bullet>& bullets,
              Vector2 plPos,
              Vector2 plSize,
              Vector2 dir)
{
    // ยังไม่ได้เก็บปืน
    if (!gun.pickedUp) return;

    // ยัง cooldown อยู่
    if (gun.fireTimer > 0) return;

    // ตำแหน่งยิง (กลางตัวผู้เล่น)
    Vector2 center = {
        plPos.x + plSize.x / 2,
        plPos.y + plSize.y / 2
    };

    Bullet b;
    b.pos = center;
    b.vel = Vector2Scale(dir, gun.bulletSpeed); // ✅ ใช้ได้แล้ว
    b.radius = 4;
    b.active = true;

    bullets.push_back(b);

    // รีเซ็ต cooldown
    gun.fireTimer = gun.fireRate;
}

// วาดปืน
void DrawGun(const Gun& gun,
             Vector2 plPos,
             Vector2 plSize,
             Vector2 dir)
{
    // ยังไม่เก็บ → วาดบนพื้น
    if (!gun.pickedUp)
    {
        DrawRectangleV(gun.pos, gun.size, DARKGRAY);
        return;
    }

    // เก็บแล้ว → วาดปืนที่ตัวผู้เล่น
    Vector2 center = {
        plPos.x + plSize.x / 2,
        plPos.y + plSize.y / 2
    };

    Vector2 muzzle =
        Vector2Add(center, Vector2Scale(dir, 18)); // ✅ ใช้ได้แล้ว

    DrawLineEx(center, muzzle, 4, BLACK);
}