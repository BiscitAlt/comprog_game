#include "enemy.h"
#include "raymath.h"

void InitEnemy(Enemy& e, Vector2 pos, const MonsterTemplate& tmpl)
{
    e.pos = pos; 
    e.size = { 18, 18 }; 
    
    e.namemonster = tmpl.name;
    e.hp = tmpl.hp;
    e.maxHp = tmpl.hp; 
    e.attack = tmpl.atk;
    e.color = tmpl.color; 
    e.speed = tmpl.speed;
    e.attackRange = tmpl.attackRange;
    e.defense = tmpl.defense;
}

void UpdateEnemy(Enemy& e, Vector2 playerPos)
{
    if (e.hp <= 0) return;
    
    Vector2 dir = Vector2Subtract(playerPos, e.pos); 
    float distance = Vector2Length(dir); 
    

    if (distance > e.attackRange) 
    {
        dir = Vector2Normalize(dir); 
        e.pos = Vector2Add(e.pos, Vector2Scale(dir, e.speed)); 
    }
}

void DrawEnemy(const Enemy& e) 
{
    DrawRectangleV(e.pos, e.size, e.color);
}