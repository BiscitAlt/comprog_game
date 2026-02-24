#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h" 
#include "map.h"

struct player {
    Vector2 pos;
    Vector2 size;
    float speed;
    float stamina;      
    Color color;
    int hp;
    int hpMax;
    int level;          
    int exp;
    int expNext;
};

void plUpdate(player &pl, Map &map);

#endif