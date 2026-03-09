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

    // ===== Animation =====
    Texture2D texture;
    int frame;
    int direction;
    float frameTime;
    bool faceRight;

    // ===== HP =====
    int hp;
    int hpMax;

    // ===== Mana =====
    float mana;
    float manaMax;

    // ===== Combat =====
    int attack;

    // ===== RPG =====
    int level;
    int exp;
    int expNext;

    // ===== Skills =====
    std::vector<Skill> skillList;
};

// ======================
// Functions
// ======================

void plUpdate(player &pl, Map &map);

void plMovement(player &pl);

void DrawPlayer(player &pl);

#endif