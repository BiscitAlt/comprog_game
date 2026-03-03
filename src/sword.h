#ifndef SWORD_H
#define SWORD_H

#include "raylib.h"
#include <vector>

// ================= SWORD TYPE =================
enum SwordType
{
    SWORD_ENERGY,
    SWORD_SPIN,
    SWORD_DASH
};

// ================= SWORD WAVE =================
struct SwordWave
{
    Vector2 pos;
    Vector2 dir;
    float speed;
    int damage;
    bool active;
};

// ================= SWORD =================
struct Sword
{
    Vector2 pos;
    bool pickedUp;

    SwordType type;

    float cooldown;
    float timer;

    int damage;

    // mana (ใช้กับ ENERGY และ SPIN)
    float manaCost;

    // spin
    float spinRadius;

    // dash
    float dashPower;
};

// ================= FUNCTION PROTOTYPES =================
void InitSword(Sword& s, Vector2 dropPos, SwordType type);

void UpdateSword(
    Sword& s,
    Vector2 plPos,
    Vector2 plSize,
    Vector2 dir
);

void UseSword(
    Sword& s,
    Vector2& playerPos,
    Vector2 dir,
    std::vector<SwordWave>& waves,
    float& playerMana
);

void UpdateSwordWaves(
    std::vector<SwordWave>& waves,
    float dt
);

void DrawSword(
    Sword& s,
    Vector2 plPos,
    Vector2 plSize,
    Vector2 dir
);

void DrawSwordWaves(
    const std::vector<SwordWave>& waves
);

#endif