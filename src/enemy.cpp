#include "enemy.h"
#include "raymath.h"

void InitEnemy(Enemy& e, Vector2 pos, EnemyType type)
{
    e.pos = pos;
    e.size = { 18, 18 };
    e.hp = 2;
    e.attackTimer = 0;
    e.atk = 5;
    e.type = type;

    if (type == MELEE)
    {
        e.speed = 1.0f;
        e.color = BLUE;
    }
    if (type == RANGED) // RANGED
    {
        e.speed = 0.6f;
        e.color = ORANGE;

        e.shootTimer = 0.0f;
        e.shootCooldown = 1.5f;
        e.shootRange = 300.0f;
    }else if (type == POISON)
{
    e.speed = 1.2f;
    e.color = GREEN;

    e.poisonRadius = 60.0f;
    e.poisonDamage = 2;
    e.poisonInterval = 0.5f;  // ทุก 0.5 วิ
    e.poisonTimer = 0.0f;
    e.poisonActive = false;
}else if (type == EXPLODER)
{
    e.speed = 1.3f;
    e.color = YELLOW;
    e.explodeRadius = 10.0f;
    e.explodeDamage = 25;
}
}

void UpdateEnemy(Enemy& e, Vector2 playerPos)
{
    float delta = GetFrameTime();
    Vector2 dir = Vector2Subtract(playerPos, e.pos);
    float distance = Vector2Length(dir);

    if (e.type == MELEE)
    {
        if (distance > 1)
        {
            dir = Vector2Normalize(dir);
            e.pos = Vector2Add(e.pos, Vector2Scale(dir, e.speed));
        }
    }
    else if (e.type == RANGED)
    {
        // รักษาระยะ
        if (distance > 200)
        {
            dir = Vector2Normalize(dir);
            e.pos = Vector2Add(e.pos, Vector2Scale(dir, e.speed));
        }

        // ยิง
        e.shootTimer += delta;

        if (distance <= e.shootRange && e.shootTimer >= e.shootCooldown)
        {
            Bullet b;
            b.pos = e.pos;
            b.radius = 5;
            b.active = true;

            dir = Vector2Normalize(dir);
            b.velocity = Vector2Scale(dir, 250.0f);

            e.bullets.push_back(b);
            e.shootTimer = 0.0f;
        }

        // อัปเดตกระสุน
        for (auto& b : e.bullets)
        {
            if (b.active)
            {
                b.pos = Vector2Add(b.pos, Vector2Scale(b.velocity, delta));

                if (b.pos.x < -1000 || b.pos.x > 5000 ||
                    b.pos.y < -1000 || b.pos.y > 5000)
                {
                    b.active = false;
                }
            }
        }
    }
    else if (e.type == POISON)
{
    if (distance > 1)
    {
        dir = Vector2Normalize(dir);
        e.pos = Vector2Add(e.pos, Vector2Scale(dir, e.speed));
    }

    // ถ้าเข้าใกล้ผู้เล่น เปิดโหมดพิษ
    if (distance < 120.0f)
        e.poisonActive = true;
    else
        e.poisonActive = false;

    if (e.poisonActive)
        e.poisonTimer += delta;
}
    else if (e.type == EXPLODER)
{
    if (distance > 1)
    {
        dir = Vector2Normalize(dir);
        e.pos = Vector2Add(e.pos, Vector2Scale(dir, e.speed));
    }
}
}

void DrawEnemy(const Enemy& e)
{
    DrawRectangleV(e.pos, e.size, e.color);

    if (e.type == RANGED)
    {
        for (const auto& b : e.bullets)
        {
            if (b.active)
                DrawCircleV(b.pos, b.radius, RED);
        }
    }
}