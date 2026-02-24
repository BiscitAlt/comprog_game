#include "sword.h"
#include "raylib.h"
#include "raymath.h"

// ================================
// สร้างดาบเริ่มต้น (วางบนพื้น)
// ================================
void InitSword(Sword& s, Vector2 pos)
{
    s.pos = pos;
    s.pickedUp = false;
    s.swinging = false;
    s.swingTimer = 0.0f;
}

// ================================
// อัปเดตสถานะดาบ
// - เก็บดาบ
// - เริ่มฟัน
// - จับเวลาฟัน
// ================================
void UpdateSword(
    Sword& s,
    Vector2 playerPos,
    Vector2 playerSize,
    Vector2 dir)
{
    // ถ้ายังไม่เก็บ → เช็กชนผู้เล่น
    if (!s.pickedUp)
    {
        Rectangle swordRec = {
            s.pos.x, s.pos.y,
            16, 6
        };

        Rectangle playerRec = {
            playerPos.x, playerPos.y,
            playerSize.x, playerSize.y
        };

        if (CheckCollisionRecs(swordRec, playerRec))
        {
            s.pickedUp = true;
        }

        return;
    }

    // กด SPACE เพื่อฟัน
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !s.swinging)
{
    s.swinging = true;
    s.swingTimer = 0.2f;
}
    // นับเวลาฟัน
    if (s.swinging)
    {
        s.swingTimer -= GetFrameTime();

        if (s.swingTimer <= 0.0f)
        {
            s.swinging = false;
        }
    }
}

// =================================
// hitbox ดาบตอนฟัน
// =================================
Rectangle GetSwordHitbox(
    const Sword& s,
    Vector2 playerPos,
    Vector2 playerSize,
    Vector2 dir)
{
    Vector2 center = {
        playerPos.x + playerSize.x / 2,
        playerPos.y + playerSize.y / 2
    };

    Rectangle hitbox = {
        center.x + dir.x * 25 - 8,
        center.y + dir.y * 25 - 4,
        16,
        8
    };

    return hitbox;
}

// =================================
// วาดดาบ (พื้น / ถือ / ฟัน)
// =================================
void DrawSword(
    const Sword& s,
    Vector2 playerPos,
    Vector2 playerSize,
    Vector2 dir)
{
    // ยังไม่เก็บ → วาดบนพื้น
    if (!s.pickedUp)
    {
        DrawRectangleV(s.pos, {16, 6}, GRAY);
        return;
    }

    Vector2 center = {
        playerPos.x + playerSize.x / 2,
        playerPos.y + playerSize.y / 2
    };

    // ดาบตอนถือ
    Rectangle holdSword = {
        center.x + dir.x * 12,
        center.y + dir.y * 12,
        14,
        4
    };

    DrawRectangleRec(holdSword, DARKGRAY);

    // ดาบตอนฟัน
    if (s.swinging)
    {
        Rectangle hitbox =
            GetSwordHitbox(s, playerPos, playerSize, dir);

        DrawRectangleRec(hitbox, ORANGE);
    }
}