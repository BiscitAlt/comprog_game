#pragma once
#include "raylib.h"
#include <vector>

enum MagicType
{
    FIRE,
    ICE,
    LIGHTNING
};

struct MagicProjectile
{
    Vector2 pos;
    Vector2 dir;
    float speed;
    float radius;
    float damage;
    bool active;

    MagicType type;
};

void SpawnMagicProjectile(
    std::vector<MagicProjectile>& list,
    Vector2 startPos,
    Vector2 targetPos,
    MagicType type
);

void UpdateMagicProjectiles(
    std::vector<MagicProjectile>& list,
    float dt
);

void DrawMagicProjectiles(
    const std::vector<MagicProjectile>& list
);