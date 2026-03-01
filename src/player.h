#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "map.h"
#include "item.h"

struct player
{
    Vector2 pos;
    Vector2 size;
    float speed;
    Color color;

    int hp;
    int hpMax;
    float mana;
    float manaMax;
};

void plMovement(Vector2 &plPos, float speed);
void plCollision(Vector2 &plPos, Vector2 plSize, float plSpeed, Map &map);

#endif