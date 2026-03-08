#pragma once
#include "raylib.h"
#include <vector>
#include "magic_projectile.h"

// ===== ประเภทคฑา =====
enum MagicWeaponType
{
    FIRE_STAFF,
    ICE_WAND,
    LIGHTNING_ROD
};

struct MagicWeapon
{
    Vector2 pos;
    Vector2 size;
    bool pickedUp;

    MagicWeaponType type;

    float cooldown;
    float cooldownTimer;
};


// ===== Texture ของอาวุธ =====
extern Texture2D fireStaffTex;
extern Texture2D iceWandTex;
extern Texture2D lightningRodTex;

// ===== Function =====
void InitMagicWeapon(
    MagicWeapon& m,
    Vector2 pos,
    MagicWeaponType type
);

void UpdateMagicWeapon(MagicWeapon& magic, Vector2 playerPos, Vector2 playerSize, float dt);

void TryShootMagic(
    MagicWeapon& m,
    std::vector<MagicProjectile>& projectiles,
    Vector2 playerPos,
    Vector2 mousePos
);

void DrawMagicWeapon(
    const MagicWeapon& m,
    Vector2 playerPos,
    Vector2 dir
);