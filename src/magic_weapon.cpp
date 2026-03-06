#include "magic_weapon.h"
#include "raymath.h"

void InitMagicWeapon(MagicWeapon& m, Vector2 pos)
{
    m.pos = pos;
    m.pickedUp = false;
    m.cooldown = 0.4f;
    m.timer = 0.0f;
}

void UpdateMagicWeapon(
    MagicWeapon& m,
    Vector2 plPos,
    Vector2 plSize,
    float dt
)
{
    if (m.timer > 0) m.timer -= dt;

    Rectangle weaponRec = { m.pos.x, m.pos.y, 20, 40 };
    Rectangle playerRec = { plPos.x, plPos.y, plSize.x, plSize.y };

    if (!m.pickedUp && CheckCollisionRecs(weaponRec, playerRec))
    {
        m.pickedUp = true;
    }

    if (m.pickedUp)
    {
        m.pos = Vector2Add(plPos, { 10, 10 });
    }
}

void UseMagicWeapon(
    MagicWeapon& m,
    std::vector<MagicProjectile>& projectiles,
    Vector2 plPos,
    Vector2 dir
)
{
    if (!m.pickedUp || m.timer > 0) return;

    MagicProjectile p;
    InitMagicProjectile(
        p,
        Vector2Add(plPos, { 10, 10 }),
        dir
    );

    projectiles.push_back(p);
    m.timer = m.cooldown;
}

void DrawMagicWeapon(
    const MagicWeapon& m,
    Vector2 plPos,
    Vector2 dir
)
{
    Vector2 drawPos = m.pickedUp ? plPos : m.pos;
    DrawRectangleV(drawPos, { 8, 30 }, PURPLE);
}