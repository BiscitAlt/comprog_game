#include "magic_weapon.h"
#include "raymath.h"

// =======================
// INIT
// =======================

// ===== Texture ของอาวุธ =====
Texture2D fireStaffTex;
Texture2D iceWandTex;
Texture2D lightningRodTex;

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
    Texture2D tex;

switch (m.type)
{
    case FIRE_STAFF:
        tex = fireStaffTex;
        break;
    case ICE_WAND:
        tex = iceWandTex;
        break;
    case LIGHTNING_ROD:
        tex = lightningRodTex;
        break;
}

    if (!m.pickedUp)
    {
        DrawTexture(tex, m.pos.x, m.pos.y, WHITE);
    }
    else
                 {
    
    
              // วาดตอนถือ
                  float scale = 0.05f;

              Vector2 drawPos = {
              playerPos.x + dir.x * 18 - (tex.width * 0.05f)/2,
              playerPos.y + dir.y * 18 - (tex.height * 0.05f)/2
            };

             DrawTextureEx(tex, drawPos, 0.0f, scale, WHITE);
             }
}