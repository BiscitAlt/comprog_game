#include "magic_projectile.h"
#include "raymath.h"
#include "enemy.h"

void SpawnMagicProjectile(
    std::vector<MagicProjectile>& list,
    Vector2 startPos,
    Vector2 targetPos,
    MagicType type
)
{
    MagicProjectile m;

    m.pos = startPos;
    m.dir = Vector2Normalize(Vector2Subtract(targetPos, startPos));
    m.speed = 400.0f;
    m.radius = 6;
    m.damage = 30;
    m.active = true;
    m.type = type;

    list.push_back(m);
}

void UpdateMagicProjectiles(
    std::vector<MagicProjectile>& list,
    std::vector<Enemy>& enemies,
    float dt
)
{
    for (auto& m : list)
    {
        if (!m.active) continue;

        m.pos = Vector2Add(
            m.pos,
            Vector2Scale(m.dir, m.speed * dt)
        );

        for (auto& e : enemies)
        {
            Rectangle enemyRec = {
                e.pos.x, e.pos.y,
                e.size.x, e.size.y
            };

            if (CheckCollisionCircleRec(m.pos, m.radius, enemyRec))
            {
                // ===== FIRE =====
                if (m.type == FIRE)
                {
                    e.hp -= m.damage;

                    // ระเบิดรอบตัว
                    for (auto& other : enemies)
                    {
                        if (Vector2Distance(other.pos, e.pos) < 60)
                        {
                            other.hp -= 15;
                            other.burnTimer = 3.0f;
                        }
                    }
                }

                // ===== ICE =====
                if (m.type == ICE)
                {
                    e.hp -= m.damage;

                    e.iceStack++;
                    e.slowTimer = 2.0f;
                    e.slowPower = 0.6f;
                }    
                    // ===== ครบ 3 stack ระเบิด =====
                    if (e.iceStack >= 3)
                {
                        float explosionRadius = 80.0f;

                        for (auto& other : enemies)
                        {
                            float dist = Vector2Distance(e.pos, other.pos);

                            if (dist < explosionRadius)
                                {
                                        other.hp -= 300;          // ดาเมจระเบิด
                                        other.slowTimer = 3.0f;   // สโล 3 วิ
                                        other.slowPower = 0.2f;   // เหลือ 20% speed (80% slow)
        }
    }

                e.iceStack = 0;
                }

                // ===== LIGHTNING =====
                if (m.type == LIGHTNING)
                {
                    float damage = m.damage;
                    Enemy* current = &e;

                    for (int i = 0; i < 3; i++)
                    {
                        if (!current) break;

                        current->hp -= damage;
                        current->electrifiedTimer = 2.0f;

                        damage *= 0.8f; // ลด 20%

                        // หาเป้าหมายใกล้สุด
                        Enemy* next = nullptr;
                        float minDist = 99999;

                        for (auto& other : enemies)
                        {
                            if (&other == current) continue;

                            float d = Vector2Distance(
                                current->pos,
                                other.pos
                            );

                            if (d < minDist && d < 150)
                            {
                                minDist = d;
                                next = &other;
                            }
                        }

                        current = next;
                    }
                }

                m.active = false;
                break;
            }
        }
    }
}

void DrawMagicProjectiles(
    const std::vector<MagicProjectile>& list
)
{
    for (const auto& m : list)
    {
        if (!m.active) continue;

        Color c = WHITE;

        if (m.type == FIRE) c = ORANGE;
        if (m.type == ICE) c = SKYBLUE;
        if (m.type == LIGHTNING) c = YELLOW;

        DrawCircleV(m.pos, m.radius, c);
    }
}