#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h" 
#include "map.h"
#include "item.h"
#include <string>  
#include <vector>

struct Skill {
    std::string skillName;
    int damage;
    int mpCost;
};

struct player {
    Vector2 pos;
    Vector2 size;
    float speed;
    float stamina;      
    Color color;
    std::string name;
    
    int hp;
    int hpMax;
    int maxHp;    
    
    int mp;
    int maxMp;    
    int attack;
    
    int level;          
    int exp;
    int expNext;
    
    std::vector<Skill> skillList;
};

void plUpdate(player &pl, Map &map);
void plMovement(Vector2 &plPos, float speed);
void plCollision(Vector2 &plPos, Vector2 plSize, float plSpeed, Map &map);

#endif