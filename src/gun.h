#pragma once
#include <vector>
#include "raylib.h"
#include "bullet.h"

enum class GunType
{
    SHOTGUN,
    ROCKET
};

struct Gun
{
    Vector2 pos;
    Vector2 size;
    bool pickedUp;

    GunType type;

    float fireRate;
    float fireTimer;
    float bulletSpeed;

};

void InitGun(Gun& gun, Vector2 pos, GunType type);
void UpdateGun(Gun& gun, Vector2 plPos, Vector2 plSize, float dt);

void ShootGun(Gun& gun,
              std::vector<Bullet>& bullets,
              Vector2 plPos,
              Vector2 plSize,
              Vector2 dir,
              float& mana);

void DrawGun(const Gun& gun,
             Vector2 plPos,
             Vector2 plSize,
             Vector2 dir);