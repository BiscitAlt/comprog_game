#pragma once
#include "raylib.h"

struct MagicProjectile
{
    Vector2 pos;
    Vector2 dir;
    float speed;
    float radius;
    int damage;
    bool active;
};

void InitMagicProjectile(
    MagicProjectile& p,
    Vector2 startPos,
    Vector2 direction
);

void UpdateMagicProjectile(MagicProjectile& p, float dt);
void DrawMagicProjectile(const MagicProjectile& p);