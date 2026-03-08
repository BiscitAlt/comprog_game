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

struct BulletExplosion
{
    Vector2 pos;
    float radius;
    float timer;
};

extern std::vector<BulletExplosion> bulletExplosions;

void UpdateBullets(
    std::vector<Bullet>& bullets,
    std::vector<Enemy>& enemies,
    float dt
);
void DrawBullet(const Bullet& b);
void DrawBulletExplosions();