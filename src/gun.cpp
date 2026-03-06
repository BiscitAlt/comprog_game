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
            gun.fireRate = 0.6f;
            gun.bulletSpeed = 500;
            gun.manaCost = 0;
            break;

        case GunType::LASER:
            gun.fireRate = 0;
            gun.bulletSpeed = 1200;
            gun.manaCost = 20;
            break;

        case GunType::ROCKET:
            gun.fireRate = 1.0f;
            gun.bulletSpeed = 400;
            gun.manaCost = 0;
            break;
    }
}

void UpdateGun(Gun& gun, Vector2 plPos, Vector2 plSize)
{
    if (gun.fireTimer > 0)
        gun.fireTimer -= GetFrameTime();

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

    Vector2 center = {
        plPos.x + plSize.x / 2,
        plPos.y + plSize.y / 2
    };

    // ===== Shotgun =====
    if (gun.type == GunType::SHOTGUN)
    {
        if (gun.fireTimer > 0) return;

        const int pellet = 6;
        const float spread = 0.4f;

        for (int i = 0; i < pellet; i++)
        {
            float angle = atan2f(dir.y, dir.x);
            angle += GetRandomValue(-100, 100) / 100.0f * spread;

            Vector2 d = { cosf(angle), sinf(angle) };

            bullets.push_back({
                center,
                Vector2Scale(d, gun.bulletSpeed),
                4,
                true,
                BulletType::NORMAL,
                1.5f,
                0,
                0
            });
        }

        gun.fireTimer = gun.fireRate;
    }

    // ===== Laser =====
    else if (gun.type == GunType::LASER)
    {
        if (mana <= 0) return;

        mana -= gun.manaCost * GetFrameTime();

        bullets.push_back({
            center,
            Vector2Scale(dir, gun.bulletSpeed),
            0,
            true,
            BulletType::LASER,
            0.15f,
            3,          // เด้งได้ 3 ครั้ง
            300.0f      // ความยาวเส้น
        });
    }

    // ===== Rocket =====
    else if (gun.type == GunType::ROCKET)
    {
        if (gun.fireTimer > 0) return;

        bullets.push_back({
            center,
            Vector2Scale(dir, gun.bulletSpeed),
            8,
            true,
            BulletType::ROCKET,
            3.0f,
            0,
            0
        });

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