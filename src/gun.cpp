#include "gun.h"
#include "raymath.h"

void InitGun(Gun& gun, Vector2 pos, GunType type)
{
    gun.pos = pos;
    gun.size = { 24, 12 };
    gun.pickedUp = false;

    gun.type = type;
    gun.fireTimer = 0;

    switch (type)
    {
        case GunType::SHOTGUN:
            gun.fireRate = 0.9f;
            gun.bulletSpeed = 500;
            break;

        case GunType::ROCKET:
            gun.fireRate = 1.4f;
            gun.bulletSpeed = 650;
            break;
    }
}

void UpdateGun(Gun& gun, Vector2 plPos, Vector2 plSize, float dt)
{
    if (gun.fireTimer > 0)
    {
        gun.fireTimer -= dt;
        if (gun.fireTimer < 0)
            gun.fireTimer = 0;
    }

    if (gun.pickedUp) return;

    Rectangle gunRec = { gun.pos.x, gun.pos.y, gun.size.x, gun.size.y };
    Rectangle plRec = { plPos.x, plPos.y, plSize.x, plSize.y };

    if (CheckCollisionRecs(gunRec, plRec))
        gun.pickedUp = true;
}

void ShootGun(Gun& gun,
              std::vector<Bullet>& bullets,
              Vector2 plPos,
              Vector2 plSize,
              Vector2 dir,
              float& mana)
{
    if (!gun.pickedUp) return;
    dir = Vector2Normalize(dir);

    Vector2 center = {
        plPos.x + plSize.x / 2,
        plPos.y + plSize.y / 2
    };

    // ===== Shotgun =====
    if (gun.type == GunType::SHOTGUN)
    {
        if (gun.fireTimer > 0) return;

        const int pellet = 4;
        const float spread = 0.35f;

        for (int i = 0; i < pellet; i++)
        {
            float angle = atan2f(dir.y, dir.x);
            angle += GetRandomValue(-120, 120) / 100.0f * spread;

            Vector2 d = { cosf(angle), sinf(angle) };

            Bullet b;
            b.pos = Vector2Add(center, Vector2Scale(d, 16));
            b.vel = Vector2Scale(d, gun.bulletSpeed);
            b.radius = 4;
            b.active = true;
            b.type = BulletType::NORMAL;
            b.lifeTime = 1.5f;

            bullets.push_back(b);
        }

        gun.fireTimer = gun.fireRate;
    }


    // ===== Rocket =====
    else if (gun.type == GunType::ROCKET)
    {
        if (gun.fireTimer > 0) return;

        Bullet b;
        b.pos = Vector2Add(center, Vector2Scale(dir, 16));
        b.vel = Vector2Scale(dir, gun.bulletSpeed);
        b.radius = 8;
        b.active = true;
        b.type = BulletType::ROCKET;
        b.lifeTime = 2.2f;

        bullets.push_back(b);

        gun.fireTimer = gun.fireRate;
    }
}

void DrawGun(const Gun& gun,
             Vector2 plPos,
             Vector2 plSize,
             Vector2 dir)
{
    if (!gun.pickedUp)
    {
        DrawRectangleV(gun.pos, gun.size, DARKGRAY);
        return;
    }

    Vector2 center = {
        plPos.x + plSize.x / 2,
        plPos.y + plSize.y / 2
    };

    Vector2 muzzle = Vector2Add(center, Vector2Scale(dir, 18));
    DrawLineEx(center, muzzle, 4, BLACK);
}