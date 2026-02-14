#include "raylib.h"

#include <vector>
#include <iostream>

struct player {
    Vector2 pos;
    Vector2 size;
    float speed;
    Color color;
};

void plMovement(Vector2 &plPos, float speed)
{
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) plPos.x += speed;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) plPos.x -= speed;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) plPos.y -= speed;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) plPos.y += speed;
}