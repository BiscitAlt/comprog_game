#pragma once
#include "raylib.h"

struct Bullet
{
    Vector2 pos;
    Vector2 vel;
    float radius;
    bool active;
};

void UpdateBullet(Bullet& b);
void DrawBullet(const Bullet& b);