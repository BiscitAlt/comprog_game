#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include <vector>
#include <string>
#include "bullet.h"

// =======================
// ENEMY TYPE
// =======================

enum EnemyType
{
    MELEE,
    RANGED,
    POISON,
    EXPLODER
};

// =======================
// ENEMY STRUCT
// =======================

struct Enemy
{
    Vector2 pos;
    Vector2 size;

    EnemyType type;

    std::string namemonster;

    int hp;
    int hpMax;

    int atk;

    float speed;

    Color color;

    float attackTimer;

    Texture2D texture;

    Rectangle frameRec;

    int frameCount;
    int currentFrame;
    float frameTimer;
    float frameSpeed;

   bool facingRight = true;

    // ===================
    // RANGED
    // ===================

    float shootCooldown;
    float shootTimer;
    float shootRange;

    std::vector<Bullet> bullets;

    // ===================
    // POISON
    // ===================

    float poisonRadius;
    float poisonInterval;
    float poisonTimer;
    bool poisonActive = false;

    // ===================
    // EXPLODER
    // ===================

    float explodeRadius;

    // ===================
    // STATUS EFFECT
    // ===================

    float burnTimer = 0;
    float slowTimer = 0;
    float freezeTimer = 0;
    float electrifiedTimer = 0;

    int iceStack = 0;

    float slowPower = 0;
};

// =======================
// FUNCTION
// =======================

void InitEnemy(Enemy& e, Vector2 pos, EnemyType type);

void InitEnemy(Enemy& e, Vector2 pos, std::string name, int hp, int atk);

void UpdateEnemy(Enemy& e, Vector2 playerPos);

void DrawEnemy(const Enemy& e);

#endif