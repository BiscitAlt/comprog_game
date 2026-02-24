#pragma once
#include "raylib.h"
#include <vector>
#include "magic_projectile.h"

struct MagicWeapon
{
    Vector2 pos;
    bool pickedUp;

    float cooldown;
    float timer;
};

void InitMagicWeapon(MagicWeapon& m, Vector2 pos);

void UpdateMagicWeapon(
    MagicWeapon& m,
    Vector2 plPos,
    Vector2 plSize,
    float dt
);

void UseMagicWeapon(
    MagicWeapon& m,
    std::vector<MagicProjectile>& projectiles,
    Vector2 plPos,
    Vector2 dir
);

void DrawMagicWeapon(
    const MagicWeapon& m,
    Vector2 plPos,
    Vector2 dir
);