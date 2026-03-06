#include "sword.h"
#include "raymath.h"
#include <cmath>

// ================= INIT =================
void InitSword(Sword& s, Vector2 dropPos, SwordType type)
{
    s.pos = dropPos;
    s.pickedUp = false;
    s.type = type;
    s.timer = 0;
    s.effectTimer = 0.0f;
    s.effectDuration = 0.25f;
    s.effectHitActive = false;

    switch (s.type)
    {
        case SWORD_ENERGY:
            s.cooldown = 0.5f;
            s.damage = 25;
            s.manaCost = 3;
            break;

        case SWORD_SPIN:
            s.cooldown = 1.0f;
            s.damage = 40;
            s.spinRadius = 80;
            s.manaCost = 10;
            break;

        case SWORD_LIFESTEAL:
            s.cooldown = 0.6f;
            s.damage = 12;              // ดาเมจเบา
            s.lifeStealPercent = 0.5f;  // ดูด 50% ของดาเมจ
            s.manaCost = 0;
            break;
    }
}

// ================= UPDATE =================
void UpdateSword(Sword& s,Vector2 plPos,Vector2 plSize,Vector2 dir,std::vector<Enemy>& enemies)
{
    if (s.timer > 0)
        s.timer -= GetFrameTime();

    if (s.effectTimer > 0)
        s.effectTimer -= GetFrameTime();

    if (s.type == SWORD_LIFESTEAL && s.effectTimer > 0 && s.effectHitActive)
    {
    float progress = 1.0f - (s.effectTimer / s.effectDuration);
    float radius = 40 + progress * 30;

    Vector2 center = {
        plPos.x + plSize.x / 2,
        plPos.y + plSize.y / 2
    };

    for (auto& e : enemies)
    {
        Rectangle enemyRec = {
            e.pos.x,
            e.pos.y,
            e.size.x,
            e.size.y
        };

        if (CheckCollisionCircleRec(center, radius, enemyRec))
        {
            e.hp -= s.damage;
        }
    }

    // กันตีซ้ำ
    s.effectHitActive = false;
    }    

    if (!s.pickedUp)
    {
        Rectangle swordRec = { s.pos.x, s.pos.y, 30, 10 };
        Rectangle plRec = { plPos.x, plPos.y, plSize.x, plSize.y };

        if (CheckCollisionRecs(swordRec, plRec))
            s.pickedUp = true;
    }
}

// ================= USE =================
void UseSword(
    Sword& s,
    player& pl,
    Vector2 dir,
    std::vector<SwordWave>& waves,
    std::vector<Enemy>& enemies
)
{
    if (s.timer > 0)
        return;

    // ===== MANA CHECK =====
    if (s.type == SWORD_ENERGY)
    {
        if (pl.mana < s.manaCost)
        return;

        pl.mana -= s.manaCost;
    }
    else if (s.type == SWORD_SPIN)
    {
        if (pl.mana < s.manaCost)
        return;

        pl.mana -= s.manaCost;
}

    s.timer = s.cooldown;

    // ===== ENERGY =====
    if (s.type == SWORD_ENERGY)
    {
        SwordWave w;
        w.pos = pl.pos;
        w.dir = dir;
        w.speed = 600;
        w.damage = s.damage;
        w.active = true;

        waves.push_back(w);
    }

    // ===== SPIN =====
    else if (s.type == SWORD_SPIN)
    {
        for (int i = 0; i < 8; i++)
        {
            float angle = (2 * PI / 8) * i;

            SwordWave w;
            w.pos = pl.pos;
            w.dir = { cosf(angle), sinf(angle) };
            w.speed = 500;
            w.damage = s.damage;
            w.active = true;

            waves.push_back(w);
        }
    }
    else if (s.type == SWORD_LIFESTEAL)
    {
        float attackRadius = 60.0f;
        int totalDamage = 0;

    for (auto& e : enemies)
    {
        Rectangle enemyRec = {
            e.pos.x,
            e.pos.y,
            e.size.x,
            e.size.y
        };

        if (CheckCollisionCircleRec(pl.pos, attackRadius, enemyRec))
        {
            e.hp -= s.damage;
            totalDamage += s.damage;
        }
    }

        // ===== ดูดเลือด =====
        float heal = totalDamage * s.lifeStealPercent;
        pl.hp = Clamp(pl.hp + heal, 0, pl.hpMax);

        s.effectTimer = s.effectDuration;
        s.effectHitActive = true;
}
}

// ================= UPDATE WAVES =================
void UpdateSwordWaves(
    std::vector<SwordWave>& waves,
    std::vector<Enemy>& enemies,
    float dt
)
{
    for (auto& w : waves)
    {
        if (!w.active)
            continue;

        // เคลื่อนที่
        w.pos.x += w.dir.x * w.speed * dt;
        w.pos.y += w.dir.y * w.speed * dt;

        float hitRadius = 20.0f;

        for (auto& e : enemies)
        {
            Rectangle enemyRec = {
                e.pos.x,
                e.pos.y,
                e.size.x,
                e.size.y
            };

            if (CheckCollisionCircleRec(w.pos, hitRadius, enemyRec))
            {
                e.hp -= w.damage/2;

                // wave หายเมื่อโดน
                w.active = false;

                break;
            }
        }

        // กันหลุดจอ
        if (w.pos.x < -100 || w.pos.x > 3000 ||
            w.pos.y < -100 || w.pos.y > 3000)
        {
            w.active = false;
        }
    }
}

// ================= DRAW SWORD =================
void DrawSword(Sword& s, Vector2 plPos, Vector2 plSize, Vector2 dir)
{
    if (!s.pickedUp)
    {
        Color c;

        switch (s.type)
{
    case SWORD_ENERGY: c = SKYBLUE; break;
    case SWORD_SPIN:   c = ORANGE;  break;
    case SWORD_LIFESTEAL:  c = GREEN;     break;
}

        DrawRectangleV(s.pos, { 30, 10 }, c);

        return;
    }

    Vector2 center = {
        plPos.x + plSize.x / 2,
        plPos.y + plSize.y / 2
    };

    DrawCircleV(center, 5, BLACK);

    // ===== LIFESTEAL EFFECT (ต้องอยู่ตรงนี้) =====
    if (s.type == SWORD_LIFESTEAL && s.effectTimer > 0)
    {
        float progress = 1.0f - (s.effectTimer / s.effectDuration);
        float radius = 40 + progress * 30;

        Color effectColor = Fade(GREEN, 1.0f - progress);

        DrawCircleLines(center.x, center.y, radius, effectColor);
        DrawCircleLines(center.x, center.y, radius + 8, effectColor);
        DrawCircleLines(center.x, center.y, radius + 16, effectColor);
    }
}

// ================= DRAW WAVES =================
void DrawSwordWaves(const std::vector<SwordWave>& waves)
{
    for (const auto& w : waves)
    {
        if (!w.active)
            continue;

        float radius = 70.0f;
        int segments = 16;
        float arcSize = PI / 8.0f;

        float baseAngle = atan2f(w.dir.y, w.dir.x);
        float startAngle = baseAngle - arcSize;
        float endAngle = baseAngle + arcSize;

        Vector2 prevPoint = {
            w.pos.x + cosf(startAngle) * radius,
            w.pos.y + sinf(startAngle) * radius
        };

        for (int i = 1; i <= segments; i++)
        {
            float t = (float)i / segments;
            float angle = startAngle + (endAngle - startAngle) * t;

            Vector2 newPoint = {
                w.pos.x + cosf(angle) * radius,
                w.pos.y + sinf(angle) * radius
            };

            DrawLineEx(prevPoint, newPoint, 6, SKYBLUE);
            DrawLineEx(prevPoint, newPoint, 2, WHITE);

            prevPoint = newPoint;
        }
    }
}