#include "bullet.h"
#include "raymath.h"

void UpdateBullet(Bullet& b)
{
    if (!b.active) return;

    float dt = GetFrameTime();

    b.pos.x += b.vel.x * dt;
    b.pos.y += b.vel.y * dt;

    b.lifeTime -= dt;
    if (b.lifeTime <= 0)
    {
        b.active = false;
        return;
    }

    // ===== เด้งเฉพาะ Laser =====
    if (b.type == BulletType::LASER)
    {
        if ((b.pos.x < 0 || b.pos.x > GetScreenWidth()) && b.bounceLeft > 0)
        {
            b.vel.x *= -1;
            b.bounceLeft--;
        }

        if ((b.pos.y < 0 || b.pos.y > GetScreenHeight()) && b.bounceLeft > 0)
        {
            b.vel.y *= -1;
            b.bounceLeft--;
        }

        if (b.bounceLeft <= 0)
            b.active = false;
    }
    else
    {
        // ===== กระสุนอื่น ไม่เด้ง =====
        if (b.pos.x < 0 || b.pos.x > GetScreenWidth() ||
            b.pos.y < 0 || b.pos.y > GetScreenHeight())
        {
            b.active = false;
        }
    }
}

void DrawBullet(const Bullet& b)
{
    if (!b.active) return;

    if (b.type == BulletType::LASER)
    {
        Vector2 dir = Vector2Normalize(b.vel);
        Vector2 end = {
            b.pos.x + dir.x * b.length,
            b.pos.y + dir.y * b.length
        };

        DrawLineEx(b.pos, end, 3, RED);
    }
    else if (b.type == BulletType::ROCKET)
    {
        DrawCircleV(b.pos, b.radius, ORANGE);
    }
    else
    {
        DrawCircleV(b.pos, b.radius, YELLOW);
    }
}