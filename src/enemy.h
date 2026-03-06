#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "raymath.h"
#include "map.h"
#include "item.h"
#include <string>
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
    int maxHp;
    
    Color color;
    EnemyType type;
    std::string namemonster;
    
    float attackTimer;
    int atk;
    int attack;

    float shootTimer;
    float shootCooldown;
    float shootRange;
    std::vector<Bullet> bullets;

    float poisonRadius;
    float poisonTimer;
    float poisonInterval;
    bool poisonActive;

    float explodeRadius;
};

void InitEnemy(Enemy& e, Vector2 pos, EnemyType type);
void InitEnemy(Enemy& e, Vector2 pos, std::string name, int hp, int atk);
void UpdateEnemy(Enemy& e, Vector2 playerPos);
void DrawEnemy(const Enemy& e);

#endif