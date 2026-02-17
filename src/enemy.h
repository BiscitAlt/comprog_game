#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "map.h"
#include "item.h"
#include "database.h"
#include <string>
#include <vector>

struct Enemy
{
    Vector2 pos; 
    Vector2 size;
    Color color;
    std::string namemonster;
    int maxHp;
    int hp;
    int attack;
    float speed;
    float attackRange;
    int defense;
};

void InitEnemy(Enemy& e, Vector2 pos, const MonsterTemplate& tmpl); 
void UpdateEnemy(Enemy& e, Vector2 playerPos); 
void DrawEnemy(const Enemy& e); 

#endif