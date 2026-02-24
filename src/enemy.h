#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "raymath.h"
#include <vector>

enum EnemyType { MELEE, RANGED, EXPLODER, POISON };

struct Bullet {
    Vector2 pos;
    Vector2 velocity;
    float radius;
    bool active;
};

struct Enemy {
    Vector2 pos;
    Vector2 size;
    float speed;
    int hp;
    int hpMax;
    Color color;
    EnemyType type;
    
    float attackTimer;
    int atk;

    // Ranged
    float shootTimer;
    float shootCooldown;
    float shootRange;
    std::vector<Bullet> bullets;

    // Poison
    float poisonRadius;
    float poisonTimer;
    float poisonInterval;
    bool poisonActive;

    // Exploder
    float explodeRadius;
};

void InitEnemy(Enemy& e, Vector2 pos, EnemyType type);
void UpdateEnemy(Enemy& e, Vector2 playerPos);
void DrawEnemy(const Enemy& e);

#endif