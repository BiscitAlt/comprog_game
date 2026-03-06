#pragma once
#include "raylib.h"
#include <vector>
#include "enemy.h"

enum MagicType
{
    FIRE,
    ICE,
    LIGHTNING
};

enum MagicEffectType
{
    EFFECT_NONE,
    EFFECT_HIT,        // โดนปกติ
    EFFECT_ICE_EXPLODE // ระเบิด 3 stack
};

struct MagicProjectile
{
    Vector2 pos;
    Vector2 dir;
    float speed;
    float radius;
    float damage;
    bool active;
    float effectTimer;
    Vector2 hitPos;
    MagicEffectType effectType;
    float effectRadius;   // ใช้เก็บรัศมีระเบิด

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
    std::vector<Enemy>& enemies,
    float dt
);

void DrawMagicProjectiles(
    const std::vector<MagicProjectile>& list
);