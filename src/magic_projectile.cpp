#include "magic_projectile.h"
#include "raymath.h"

void SpawnMagicProjectile(
    std::vector<MagicProjectile>& list,
    Vector2 startPos,
    Vector2 targetPos,
    MagicType type
)
{
    MagicProjectile m;

    m.pos = startPos;
    m.dir = Vector2Normalize(Vector2Subtract(targetPos, startPos));
    m.speed = 400.0f;
    m.radius = 6;
    m.damage = 30;
    m.active = true;
    m.type = type;

    list.push_back(m);
}

void UpdateMagicProjectiles(
    std::vector<MagicProjectile>& list,
    float dt
)
{
    for (auto& m : list)
    {
        if (!m.active) continue;

        m.pos = Vector2Add(
            m.pos,
            Vector2Scale(m.dir, m.speed * dt)
        );
    }
}

void DrawMagicProjectiles(
    const std::vector<MagicProjectile>& list
)
{
    for (const auto& m : list)
    {
        if (!m.active) continue;

        Color c = WHITE;

        if (m.type == FIRE) c = ORANGE;
        if (m.type == ICE) c = SKYBLUE;
        if (m.type == LIGHTNING) c = YELLOW;

        DrawCircleV(m.pos, m.radius, c);
    }
}