#include "skill.h"
#include "raymath.h"

void UpdateSkills(
    SkillState &s,
    player &pl,
    std::vector<Enemy> &enemies,
    float dt
)
{
    // ===== Cooldown =====
    s.blackHoleTimer += dt;
    s.whiteHoleTimer += dt;
    s.timeStopTimer += dt;
    s.bloodNovaTimer += dt;
    s.lightningTimer += dt;
    s.shockwaveTimer += dt;

    // ======================
    // Black Hole
    // ======================
    if (s.blackHoleTimer > 6.0f)
    {
        s.blackHoleEffect = 0.6f;

        for (Enemy &e : enemies)
        {
            Vector2 dir = Vector2Subtract(pl.pos, e.pos);
            float dist = Vector2Length(dir);

            if (dist < 200)
            {
                dir = Vector2Normalize(dir);
                e.pos = Vector2Add(e.pos, Vector2Scale(dir, 5));

                if (dist < 35)
                    e.hp -= 40;
            }
        }

        s.blackHoleTimer = 0;
    }

    // ======================
    // White Hole
    // ======================
    if (s.whiteHoleTimer > 5.0f)
    {
        s.whiteHoleEffect = 0.5f;

        for (Enemy &e : enemies)
        {
            Vector2 dir = Vector2Subtract(e.pos, pl.pos);
            float dist = Vector2Length(dir);

            if (dist < 150)
            {
                dir = Vector2Normalize(dir);
                e.pos = Vector2Add(e.pos, Vector2Scale(dir, 80));
                e.hp -= 10;
            }
        }

        s.whiteHoleTimer = 0;
    }

    // ======================
    // Time Stop
    // ======================
    if (s.timeStopTimer > 10.0f)
    {
        s.timeStopEffect = 0.7f;

        for (Enemy &e : enemies)
        {
            e.freezeTimer = 1.2f;
        }

        s.timeStopTimer = 0;
    }

    // ======================
    // Blood Nova
    // ======================
    if (s.bloodNovaTimer > 4.0f)
    {
        s.bloodNovaEffect = 0.5f;

        for (Enemy &e : enemies)
        {
            float dist = Vector2Distance(pl.pos, e.pos);

            if (dist < 120)
                e.hp -= 35;
        }

        s.bloodNovaTimer = 0;
    }

    // ======================
    // Lightning Strike
    // ======================
    if (s.lightningTimer > 3.0f)
    {
        s.lightningEffect = 0.3f;

        for (Enemy &e : enemies)
        {
            float dist = Vector2Distance(pl.pos, e.pos);

            if (dist < 250)
            {
                e.hp -= 25;
                break; // ฟ้าผ่าตัวเดียว
            }
        }

        s.lightningTimer = 0;
    }

    // ======================
    // Shockwave
    // ======================
    if (s.shockwaveTimer > 5.0f)
    {
        s.shockwaveEffect = 0.5f;

        for (Enemy &e : enemies)
        {
            float dist = Vector2Distance(pl.pos, e.pos);

            if (dist < 160)
            {
                Vector2 dir = Vector2Subtract(e.pos, pl.pos);
                dir = Vector2Normalize(dir);

                e.pos = Vector2Add(e.pos, Vector2Scale(dir, 70));
                e.hp -= 20;
            }
        }

        s.shockwaveTimer = 0;
    }

    // ===== ลดเวลา effect =====

    if (s.blackHoleEffect > 0) s.blackHoleEffect -= dt;
    if (s.whiteHoleEffect > 0) s.whiteHoleEffect -= dt;
    if (s.timeStopEffect > 0) s.timeStopEffect -= dt;
    if (s.bloodNovaEffect > 0) s.bloodNovaEffect -= dt;
    if (s.lightningEffect > 0) s.lightningEffect -= dt;
    if (s.shockwaveEffect > 0) s.shockwaveEffect -= dt;
}

void DrawSkillEffects(SkillState &s, player &pl)
{
    // Black Hole
    if (s.blackHoleEffect > 0)
    {
        DrawCircleLines(pl.pos.x, pl.pos.y, 200, PURPLE);
        DrawCircle(pl.pos.x, pl.pos.y, 30, Fade(PURPLE, 0.4f));
    }

    // White Hole
    if (s.whiteHoleEffect > 0)
    {
        DrawCircleLines(pl.pos.x, pl.pos.y, 150, SKYBLUE);
        DrawCircleLines(pl.pos.x, pl.pos.y, 100, BLUE);
    }

    // Time Stop
    if (s.timeStopEffect > 0)
    {
        DrawCircleLines(pl.pos.x, pl.pos.y, 250, YELLOW);
        DrawRing(pl.pos, 200, 240, 0, 360, 40, Fade(YELLOW, 0.3f));
    }

    // Blood Nova
    if (s.bloodNovaEffect > 0)
    {
        DrawCircle(pl.pos.x, pl.pos.y, 120, Fade(RED, 0.25f));
        DrawCircleLines(pl.pos.x, pl.pos.y, 120, RED);
    }

    // Lightning
    if (s.lightningEffect > 0)
    {
        DrawLine(pl.pos.x, pl.pos.y - 200, pl.pos.x, pl.pos.y, YELLOW);
    }

    // Shockwave
    if (s.shockwaveEffect > 0)
    {
        DrawCircleLines(pl.pos.x, pl.pos.y, 160, ORANGE);
    }
}