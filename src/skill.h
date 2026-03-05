#ifndef SKILL_H
#define SKILL_H

#include "raylib.h"
#include <vector>
#include "enemy.h"
#include "player.h"

struct SkillState
{
    // ===== Skill Cooldown =====
    float blackHoleTimer = 0;
    float whiteHoleTimer = 0;
    float timeStopTimer = 0;
    float bloodNovaTimer = 0;
    float lightningTimer = 0;
    float shockwaveTimer = 0;

    // ===== Effect Timer (สำหรับวาดเอฟเฟกต์) =====
    float blackHoleEffect = 0;
    float whiteHoleEffect = 0;
    float timeStopEffect = 0;
    float bloodNovaEffect = 0;
    float lightningEffect = 0;
    float shockwaveEffect = 0;
};

void UpdateSkills(
    SkillState &s,
    player &pl,
    std::vector<Enemy> &enemies,
    float dt
);

void DrawSkillEffects(SkillState &s, player &pl);

#endif