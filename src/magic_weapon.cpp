#include "magic_weapon.h"
#include "raymath.h"

// =======================
// INIT
// =======================
void InitMagicWeapon(
    MagicWeapon& m,
    Vector2 pos,
    MagicWeaponType type
)
{
    m.pos = pos;
    m.size = { 24, 24 };
    m.pickedUp = false;
    m.type = type;

    m.cooldownTimer = 0.0f;

    // ตั้ง cooldown ตามประเภท
    switch (type)
    {
        case FIRE_STAFF:
            m.cooldown = 0.9f;
            break;

        case ICE_WAND:
            m.cooldown = 0.7f;
            break;

        case LIGHTNING_ROD:
            m.cooldown = 1.1f;
            break;
    }
}

// =======================
// UPDATE
// =======================
void UpdateMagicWeapon(
    MagicWeapon& m,
    Vector2 playerPos,
    Vector2 playerSize,
    float dt
)
{
    // cooldown timer
    if (m.cooldownTimer > 0)
        m.cooldownTimer -= dt;

    if (!m.pickedUp)
    {
        Rectangle playerRec = {
            playerPos.x, playerPos.y,
            playerSize.x, playerSize.y
        };

        Rectangle magicRec = {
            m.pos.x, m.pos.y,
            m.size.x, m.size.y
        };

        if (CheckCollisionRecs(playerRec, magicRec))
        {
            m.pickedUp = true;
        }
    }
}

// =======================
// SHOOT
// =======================
void TryShootMagic(
    MagicWeapon& m,
    std::vector<MagicProjectile>& projectiles,
    Vector2 playerPos,
    Vector2 mousePos
)
{
    if (!m.pickedUp) return;

    if (m.cooldownTimer <= 0)
{
        MagicType projectileType = FIRE;

        switch (m.type)
        {
            case FIRE_STAFF:
                projectileType = FIRE;
                break;

            case ICE_WAND:
                projectileType = ICE;
                break;

            case LIGHTNING_ROD:
                projectileType = LIGHTNING;
                break;
        }

        SpawnMagicProjectile(
            projectiles,
            playerPos,
            mousePos,
            projectileType
        );

        m.cooldownTimer = m.cooldown;
    }
}

// =======================
// DRAW
// =======================
void DrawMagicWeapon(
    const MagicWeapon& m,
    Vector2 playerPos,
    Vector2 dir
)
{
    Color c = PURPLE;

    switch (m.type)
    {
        case FIRE_STAFF:
            c = ORANGE;
            break;
        case ICE_WAND:
            c = SKYBLUE;
            break;
        case LIGHTNING_ROD:
            c = YELLOW;
            break;
    }

    if (!m.pickedUp)
    {
        // วาดตอนตกพื้น
        DrawCircleV(m.pos, 10, c);
    }
    else
    {
        // วาดตอนถือ
        Vector2 drawPos = Vector2Add(
            playerPos,
            Vector2Scale(dir, 18)
        );

        DrawCircleV(drawPos, 6, c);
    }
}