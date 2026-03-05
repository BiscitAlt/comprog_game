#include "skill.h"
#include "raymath.h"

void UpdateSkills(
    SkillState &s,
    player &pl,
    std::vector<Enemy> &enemies,
    float dt
)
{
    s.blackHoleTimer += dt;
    s.whiteHoleTimer += dt;
    s.timeStopTimer += dt;
    s.bloodAuraTimer += dt;
    s.noHitTimer += dt;

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
                e.pos = Vector2Add(e.pos, Vector2Scale(dir, 4));

                if (dist < 30)
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
        s.whiteHoleEffect = 0.4f;
        for (Enemy &e : enemies)
        {
            Vector2 dir = Vector2Subtract(e.pos, pl.pos);
            float dist = Vector2Length(dir);

            if (dist < 150)
            {
                dir = Vector2Normalize(dir);
                e.pos = Vector2Add(e.pos, Vector2Scale(dir, 80));
            }
        }

        s.whiteHoleTimer = 0;
    }

    // ======================
    // Time Fracture
    // ======================
    if (s.timeStopTimer > 10.0f)
    {
        s.timeStopEffect = 0.7f;
        for (Enemy &e : enemies)
        {
            e.freezeTimer = 1.0f;
        }

        s.timeStopTimer = 0;
    }

    // ======================
    // Blood Aura
    // ======================
    if (s.bloodAuraTimer > 1.0f)
    {
        pl.hp -= 1;

        for (Enemy &e : enemies)
        {
            float dist = Vector2Distance(pl.pos, e.pos);

            if (dist < 80)
                e.hp -= 5;
        }

        s.bloodAuraTimer = 0;
    }

    // ======================
    // Execution
    // ======================
    for (Enemy &e : enemies)
    {
        if (e.hp > 0 && e.hp < e.hpMax * 0.05f)
        {
            e.hp = 0;
        }
    }

    if (s.blackHoleEffect > 0)
    s.blackHoleEffect -= dt;

    if (s.whiteHoleEffect > 0)
    s.whiteHoleEffect -= dt;

    if (s.timeStopEffect > 0)
    s.timeStopEffect -= dt;

}

void DrawSkillEffects(SkillState &s, player &pl)
{
    // Black Hole
    if (s.blackHoleEffect > 0)
    {   
        DrawCircleLines(pl.pos.x, pl.pos.y, 200, PURPLE);
        DrawCircle(pl.pos.x, pl.pos.y, 25, Fade(PURPLE,0.4f));
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
        DrawRing(pl.pos, 200, 240, 0, 360, 40, Fade(YELLOW,0.3f));
    }

    // Blood Aura (ตลอดเวลา)
    DrawCircle(pl.pos.x, pl.pos.y, 80, Fade(RED,0.15f));
}