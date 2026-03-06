#ifndef SKILL_H
#define SKILL_H

#include "raylib.h"
#include <vector>
#include "enemy.h"
#include "player.h"

struct SkillState
{
    //อุกาบาต
    float voidMeteorTimer = 0;
    float voidMeteorEffect = 0;


    // ดาบรอบตัว
    float phantomBladeAngle = 0;
    float phantomBladeTimer = 0;


    //สายฟ้าชิ่ง
    float chainLightningTimer = 0;
    float chainLightningEffect = 0;


    // บ่อพิษ
    float poisonMistTimer = 0;
    float poisonMistEffect = 0;

    // หลุมดำ
    float blackHoleTimer = 0;
    float blackHoleEffect = 0;

    // BLOOD AURA
    float bloodAuraTimer = 0;
    float bloodAuraEffect = 0;

    // SHOCKWAVE
    float shockwaveTimer = 0;
    float shockwaveEffect = 0;

    // SHARED EFFECT POSITION 
    Vector2 effectPos = {0,0};
};

void UpdateSkills(
    SkillState &s,
    player &pl,
    std::vector<Enemy> &enemies,
    float dt
);

void DrawSkillEffects(
    SkillState &s,
    player &pl
);

#endif