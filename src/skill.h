#ifndef SKILL_H
#define SKILL_H

#include "raylib.h"
#include <vector>
#include "enemy.h"
#include "player.h"

struct SkillState
{
    // timers
    float blackHoleTimer = 0;
    float whiteHoleTimer = 0;
    float timeStopTimer = 0;
    float bloodAuraTimer = 0;

    // stacks
    float noHitTimer = 0;

    // passive
    bool mirrorBlade = true;
    bool execution = true;
    bool vampiricBlood = true;
    bool overheatMode = true;

     // effect timer
    float blackHoleEffect = 0;
    float whiteHoleEffect = 0;
    float timeStopEffect = 0;
};

void UpdateSkills(
    SkillState &s,
    player &pl,
    std::vector<Enemy> &enemies,
    float dt
);

void DrawSkillEffects(SkillState &s, player &pl);

#endif