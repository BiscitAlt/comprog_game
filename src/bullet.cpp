#include "bullet.h"
#include "raymath.h"
#include "enemy.h"
#include <vector>
#include <algorithm>
std::vector<BulletExplosion> bulletExplosions;

void UpdateBullets(
    std::vector<Bullet>& bullets,
    std::vector<Enemy>& enemies,
    float dt
)
{
    // update explosion timers
    for (auto& ex : bulletExplosions)
    {
        ex.timer -= dt;
    }

    bulletExplosions.erase(
        std::remove_if(
            bulletExplosions.begin(),
            bulletExplosions.end(),
            [](BulletExplosion& e){ return e.timer <= 0; }
        ),
        bulletExplosions.end()
    );

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

                    BulletExplosion ex;
                    ex.pos = b.pos;
                    ex.timer = 0.35f;
                    ex.radius = explosionRadius;

                    bulletExplosions.push_back(ex);

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

void DrawBulletExplosions()
{
    for (auto& ex : bulletExplosions)
    {
        float progress = 1.0f - (ex.timer / 0.35f);

        float radius = ex.radius * progress;

        DrawCircleLinesV(ex.pos, radius, RED);

        DrawCircleV(
            ex.pos,
            radius * 0.5f,
            Fade(ORANGE, 0.35f)
        );
    }
}