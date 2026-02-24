#include "bullet.h"

void UpdateBullet(Bullet& b)
{
    if (!b.active) return;

    b.pos.x += b.vel.x * GetFrameTime();
    b.pos.y += b.vel.y * GetFrameTime();
}

void DrawBullet(const Bullet& b)
{
    if (!b.active) return;
    DrawCircleV(b.pos, b.radius, YELLOW);
}