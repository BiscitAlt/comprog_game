#include "summon.h"
#include "raymath.h"

// ========================
// Init
// ========================
void InitSummon(Summon& s, Vector2 pos)
{
    s.pos = pos;
    s.radius = 10.0f;
    s.damage = 1;
    s.speed = 90.0f;
    s.lifetime = 6.0f;
    s.active = true;
}

// ========================
// Update
// ========================
void UpdateSummon(Summon& s, float dt, Vector2 targetPos)
{
    if (!s.active) return;

    // หมดอายุ
    s.lifetime -= dt;
    if (s.lifetime <= 0)
    {
        s.active = false;
        return;
    }

    // เคลื่อนที่ไปหาเป้า
    Vector2 dir = Vector2Subtract(targetPos, s.pos);
    float dist = Vector2Length(dir);

    if (dist > 2.0f)
    {
        dir = Vector2Normalize(dir);
        s.pos = Vector2Add(
            s.pos,
            Vector2Scale(dir, s.speed * dt)
        );
    }
}

// ========================
// Draw
// ========================
void DrawSummon(const Summon& s)
{
    if (!s.active) return;

    DrawCircleV(s.pos, s.radius, PURPLE);

    // เอฟเฟกต์เล็ก ๆ
    DrawCircleLines(
        (int)s.pos.x,
        (int)s.pos.y,
        s.radius + 2,
        DARKPURPLE
    );
}