#ifndef SWORD_H
#define SWORD_H

#include "player.h"
#include "enemy.h"
#include "raylib.h"
#include <vector>


// ================= SWORD TYPE =================
enum SwordType
{
    SWORD_ENERGY,
    SWORD_SPIN,
    SWORD_LIFESTEAL
};

    extern Texture2D spiritgrimoireTex;
    extern Texture2D spiritorbTex;
    extern Texture2D spiritswordTex;
    
// ================= SWORD WAVE =================
struct SwordWave
{
    Vector2 pos;
    Vector2 dir;
    float speed;
    int damage;
    bool active;
    Texture2D tex;
};

// ================= SWORD =================
struct Sword
{
    Vector2 pos;
    Vector2 size;
    bool pickedUp;

    SwordType type;

    float cooldown;
    float timer;

    int damage;

    // mana (ใช้กับ ENERGY และ SPIN)
    float manaCost;

    // spin
    float spinRadius;

    // ดูดเลือด
    float lifeStealPercent;
    float effectTimer = 0.0f;
    float effectDuration = 0.2f;

    bool effectHitActive;
};

// ================= FUNCTION PROTOTYPES =================
void InitSword(Sword& s, Vector2 dropPos, SwordType type);

void UpdateSword(Sword& s, Vector2 plPos, Vector2 plSize, Vector2 dir, std::vector<Enemy>& enemies);

void UseSword(
    Sword& s,
    player& pl,
    Vector2 dir,
    std::vector<SwordWave>& waves,
    std::vector<Enemy>& enemies
);

void UpdateSwordWaves(
    std::vector<SwordWave>& waves,
    std::vector<Enemy>& enemies,
    float dt
);

void DrawSword(
   const Sword& s,
    Vector2 plPos,
    Vector2 plSize,
    Vector2 dir
);

void DrawSwordWaves(
    const std::vector<SwordWave>& waves
);

#endif