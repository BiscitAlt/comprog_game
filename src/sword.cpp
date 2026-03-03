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

    switch (s.type)
    {
        case SWORD_ENERGY:
            s.cooldown = 0.5f;
            s.damage = 25;
            s.manaCost = 10;
            break;

        case SWORD_SPIN:
            s.cooldown = 1.0f;
            s.damage = 40;
            s.spinRadius = 80;
            s.manaCost = 30;
            break;

        case SWORD_DASH:
            s.cooldown = 1.2f;
            s.damage = 50;
            s.dashPower = 300;
            break;
    }
}

// ================= UPDATE =================
void UpdateSword(Sword& s, Vector2 plPos, Vector2 plSize, Vector2 dir)
{
    if (s.timer > 0)
        s.timer -= GetFrameTime();

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
    Vector2& playerPos,
    Vector2 dir,
    std::vector<SwordWave>& waves,
    float& playerMana
)
{
    if (s.timer > 0)
        return;

    // ===== ENERGY MANA CHECK =====
    if (s.type == SWORD_ENERGY)
    {
        if (playerMana < s.manaCost)
            return;

        playerMana -= s.manaCost;
    }

    // ===== SPIN MANA CHECK =====
    if (s.type == SWORD_SPIN)
    {
        if (playerMana < s.manaCost)
            return;

        playerMana -= s.manaCost;
    }

    s.timer = s.cooldown;

    // ===== ENERGY =====
    if (s.type == SWORD_ENERGY)
    {
        SwordWave w;
        w.pos = playerPos;
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
            w.pos = playerPos;
            w.dir = { cosf(angle), sinf(angle) };
            w.speed = 500;
            w.damage = s.damage;
            w.active = true;

            waves.push_back(w);
        }
    }

    // ===== DASH =====
    else if (s.type == SWORD_DASH)
    {
        playerPos.x += dir.x * s.dashPower;
        playerPos.y += dir.y * s.dashPower;

        SwordWave w;
        w.pos = playerPos;
        w.dir = dir;
        w.speed = 700;
        w.damage = s.damage;
        w.active = true;

        waves.push_back(w);
    }
}

// ================= UPDATE WAVES =================
void UpdateSwordWaves(std::vector<SwordWave>& waves, float dt)
{
    for (auto& w : waves)
    {
        if (!w.active)
            continue;

        w.pos.x += w.dir.x * w.speed * dt;
        w.pos.y += w.dir.y * w.speed * dt;

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
    case SWORD_DASH:   c = RED;     break;
}

        DrawRectangleV(s.pos, { 30, 10 }, c);
        return;
    }

    Vector2 center = {
        plPos.x + plSize.x / 2,
        plPos.y + plSize.y / 2
    };

    DrawCircleV(center, 5, BLACK);
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