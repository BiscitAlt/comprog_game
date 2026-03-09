#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "map.h"
#include <string>
#include <vector>

// ======================
// Skill
// ======================
struct Skill
{
    std::string skillName;
    int damage;
    int mpCost;
};

// ======================
// Player
// ======================
struct player
{
    Vector2 pos;
    Vector2 size;

    float speed;
    Color color;

    std::string name;

    // ===== HP =====
    int hp;
    int hpMax;

    // ===== Mana =====
    float mana;
    float manaMax;

    // ===== Combat =====
    int attack;

    // ===== RPG System =====
    int level;
    int exp;
    int expNext;

    // ===== Skill List =====
    std::vector<Skill> skillList;
};

// ======================
// Functions
// ======================

void plUpdate(player &pl, Map &map);

void plMovement(Vector2 &plPos, float speed);

void DrawPlayer(player &pl);

#endif