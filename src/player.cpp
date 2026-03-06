#include "player.h"

void plUpdate(player &pl, Map &map) {
    Vector2 move = { 0, 0 };
    float currentSpeed = pl.speed;

   
    if (IsKeyDown(KEY_LEFT_SHIFT) && pl.stamina > 0) {
        currentSpeed *= 1.7f;
        pl.stamina -= 1.5f;
    } else {
        if (pl.stamina < 100) pl.stamina += 0.4f;
    }

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) move.x += currentSpeed;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) move.x -= currentSpeed;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) move.y -= currentSpeed;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) move.y += currentSpeed;

    
    float p = 3.0f; 

    
    Vector2 nextX = { pl.pos.x + move.x, pl.pos.y };
    if (!map.IsWall(nextX.x + p, nextX.y + p) && 
        !map.IsWall(nextX.x + pl.size.x - p, nextX.y + p) &&
        !map.IsWall(nextX.x + p, nextX.y + pl.size.y - p) &&
        !map.IsWall(nextX.x + pl.size.x - p, nextX.y + pl.size.y - p)) {
        pl.pos.x = nextX.x;
    }

  
    Vector2 nextY = { pl.pos.x, pl.pos.y + move.y };
    if (!map.IsWall(nextY.x + p, nextY.y + p) && 
        !map.IsWall(nextY.x + pl.size.x - p, nextY.y + p) &&
        !map.IsWall(nextY.x + p, nextY.y + pl.size.y - p) &&
        !map.IsWall(nextY.x + pl.size.x - p, nextY.y + pl.size.y - p)) {
        pl.pos.y = nextY.y;
    }
}