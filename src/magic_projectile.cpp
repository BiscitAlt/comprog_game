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
    m.damage = 18;
    m.active = true;
    m.type = type;
    m.effectTimer = 0.0f;
    m.hitPos = startPos;
    m.effectType = EFFECT_NONE;
    m.effectRadius = 0;

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
        if (m.effectTimer > 0) // ===== ลดเวลาเอฟเฟค =====
    {
        m.effectTimer -= dt;
    }   
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
                        if (Vector2Distance(other.pos, e.pos) < 50)
                        {
                            other.hp -= 8;
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

                    // ===== ครบ 3 stack ระเบิด =====
                    if (e.iceStack >= 3)
                        {
                        e.hp -= 35; // 3 stack boom
                        float explosionRadius = 80.0f;

                    for (auto& other : enemies)
        {
                    float dist = Vector2Distance(e.pos, other.pos);

                    if (dist < explosionRadius)
            {
                    other.hp -= 10;
                    other.slowTimer = 3.0f;
                    other.slowPower = 0.2f;
            }
        }

            e.iceStack = 0;

                //  ตั้งค่าเอฟเฟคระเบิด
                m.effectType = EFFECT_ICE_EXPLODE;
                m.effectRadius = explosionRadius;
    }
                else
            {
                //  โดนปกติ
                m.effectType = EFFECT_HIT;
                m.effectRadius = 30;
    }
}

                // ===== LIGHTNING =====
                if (m.type == LIGHTNING)
                {
                    float damage = m.damage;
                    Enemy* current = &e;

                    for (int i = 0; i < 4; i++)
                    {
                        if (!current) break;

                        current->hp -= damage;
                        current->electrifiedTimer = 2.0f;

                        damage *= 0.65f; // ลด 35%

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
                m.effectTimer = 0.3f;    // ให้เอฟเฟคอยู่ 0.3 วิ
                m.hitPos = m.pos;        // จำตำแหน่งที่ชน
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
        // ===== วาดลูกกระสุน =====
        if (m.active)
        {
            Color c = WHITE;

            if (m.type == FIRE) c = ORANGE;
            if (m.type == ICE) c = SKYBLUE;
            if (m.type == LIGHTNING) c = YELLOW;

            DrawCircleV(m.pos, m.radius, c);
        }

        // ===== วาดเอฟเฟคตอนชน =====
        if (m.effectTimer > 0)
        {
            float progress = 1.0f - (m.effectTimer / 0.3f);

            // 🔥 FIRE ระเบิดขยาย
            if (m.type == FIRE)
            {
                float radius = 20 + progress * 40;

                DrawCircleLinesV(
                    m.hitPos,
                    radius,
                    RED
                );

                DrawCircleV(
                    m.hitPos,
                    radius * 0.5f,
                    Fade(ORANGE, 0.3f)
                );
            }

            // ===== ICE EFFECT =====
            if (m.type == ICE && m.effectTimer > 0)
{
            float totalTime = 0.3f;
            float progress = 1.0f - (m.effectTimer / totalTime);

            // ----------------------
            // ❄ ICE HIT ธรรมดา
            // ----------------------
            if (m.effectType == EFFECT_HIT)
             {
            float radius = 30;

            // วงขอบ
            DrawCircleLinesV(
            m.hitPos,
            radius,
            SKYBLUE
        );

            // แกนกลาง glow
            DrawCircleV(
            m.hitPos,
            radius * 0.5f,
            Fade(SKYBLUE, 0.4f)
        );
    }

            // ----------------------
            // 💥 ICE EXPLOSION (3 stack)
            // ----------------------
        if (m.effectType == EFFECT_ICE_EXPLODE)
    {
        float radius = m.effectRadius;

        // วงระเบิดจริง
        DrawCircleLinesV(
            m.hitPos,
            radius,
            BLUE
        );

        // วงขยาย animation
        float animatedRadius = radius * progress;

        DrawCircleV(
            m.hitPos,
            animatedRadius,
            Fade(SKYBLUE, 0.35f)
        );

        // ===== HITBOX DEBUG =====
        // เส้นแดงคือรัศมีที่คำนวณ damage จริง
        DrawCircleLinesV(
            m.hitPos,
            radius,
            RED
        );
    }
}

            // ⚡ LIGHTNING แฟลชสายฟ้า
            if (m.type == LIGHTNING)
            {
                DrawLineEx(
                    Vector2Subtract(m.hitPos, {15,15}),
                    Vector2Add(m.hitPos, {15,15}),
                    4,
                    YELLOW
                );

                DrawLineEx(
                    Vector2Add(m.hitPos, {15,-15}),
                    Vector2Subtract(m.hitPos, {15,-15}),
                    4,
                    WHITE
                );
            }
        }
    }
}