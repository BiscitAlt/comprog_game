#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "raymath.h"
#include <vector>

enum EnemyType
{
    MELEE,
    RANGED,
    EXPLODER,
    POISON
    
};

struct Bullet
{
    Vector2 pos;
    Vector2 velocity;
    float radius;
    bool active;
};

struct Enemy
{
    Vector2 pos;
    Vector2 size;
    float speed;
    int hp;
    Color color;

    // melee
    float attackTimer;
    int atk;

    // type
    EnemyType type;

    // ranged
    float shootTimer;
    float shootCooldown;
    float shootRange;
    std::vector<Bullet> bullets;

    // poison
    float poisonRadius;
    int poisonDamage;
    float poisonInterval;
    float poisonTimer;
    bool poisonActive;

    // exploder
    float explodeRadius;
    int explodeDamage;
};

void InitEnemy(Enemy& e, Vector2 pos, EnemyType type);
void UpdateEnemy(Enemy& e, Vector2 playerPos);
void DrawEnemy(const Enemy& e);

#endif