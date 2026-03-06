#include "magic_projectile.h"
#include "raymath.h"

void InitMagicProjectile(
    MagicProjectile& p,
    Vector2 startPos,
    Vector2 direction
)
{
    p.pos = startPos;
    p.dir = Vector2Normalize(direction);
    p.speed = 400.0f;
    p.radius = 6.0f;
    p.damage = 20;
    p.active = true;
}

void UpdateMagicProjectile(MagicProjectile& p, float dt)
{
    if (!p.active) return;

    p.pos.x += p.dir.x * p.speed * dt;
    p.pos.y += p.dir.y * p.speed * dt;
}

void DrawMagicProjectile(const MagicProjectile& p)
{
    if (!p.active) return;
    DrawCircleV(p.pos, p.radius, BLUE);
}