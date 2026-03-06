#pragma once
#include "raylib.h"
#include <vector>

struct Enemy;
enum class BulletType
{
    NORMAL,
    ROCKET
};

struct Bullet
{
    Vector2 pos;
    Vector2 vel;
    float radius;
    bool active;

    BulletType type;

    float lifeTime;

};

void UpdateBullets(
    std::vector<Bullet>& bullets,
    std::vector<Enemy>& enemies,
    float dt
);
void DrawBullet(const Bullet& b);