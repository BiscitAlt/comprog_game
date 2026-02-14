#ifndef PLAYER_H
#define PLAYER_H

// รวม header ไม่ใส่ใน .cpp เพราะจะได้ไม่ต้อง include ซ้ำหลายๆ ที่
#include "raylib.h" 
#include "map.h"
#include "item.h"

struct player
{
    Vector2 pos;
    Vector2 size;
    float speed;
    Color color;
};

void plMovement(Vector2 &plPos, float speed);
void plCollision(Vector2 &plPos, Vector2 plSize, float plSpeed, Map &map);

#endif

