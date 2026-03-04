#include "bullet.h"
#include "raymath.h"
#include "enemy.h"
#include <vector>
#include <algorithm>   // สำหรับ remove_if

void UpdateBullets(
    std::vector<Bullet>& bullets,
    std::vector<Enemy>& enemies,
    float dt
)
{
    for (auto& b : bullets)
    {
        if (!b.active) continue;

        b.pos = Vector2Add(b.pos, Vector2Scale(b.vel, dt));

        for (auto& e : enemies)
        {
            Rectangle enemyRec = {
                e.pos.x,
                e.pos.y,
                e.size.x,
                e.size.y
            };

            if (b.type == BulletType::NORMAL)
            {
                if (CheckCollisionCircleRec(b.pos, b.radius, enemyRec))
                {
                    e.hp -= 20;
                    b.active = false;
                    break;
                }
            }
            else if (b.type == BulletType::ROCKET)
            {
                if (CheckCollisionCircleRec(b.pos, b.radius, enemyRec))
                {
                    float explosionRadius = 100.0f;

                    for (auto& other : enemies)
                    {
                        float dist = Vector2Distance(b.pos, other.pos);
                        if (dist < explosionRadius)
                            other.hp -= 60;
                    }

                    b.active = false;
                    break;
                }
            }
        }

        if (b.pos.x < 0 || b.pos.x > GetScreenWidth() ||
            b.pos.y < 0 || b.pos.y > GetScreenHeight())
        {
            b.active = false;
        }
    }

    bullets.erase(
        std::remove_if(
            bullets.begin(),
            bullets.end(),
            [](Bullet& b){ return !b.active; }
        ),
        bullets.end()
    );
}

void DrawBullet(const Bullet& b)
{
    if (!b.active) return;

    if (b.type == BulletType::ROCKET)
    {
        DrawCircleV(b.pos, b.radius, ORANGE);
    }
    else
    {
        DrawCircleV(b.pos, b.radius, YELLOW);
    }
}