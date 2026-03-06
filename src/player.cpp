#include "raylib.h"
#include "player.h"

void plCollision(Vector2 &plPos, Vector2 plSize, float plSpeed, Map &map)
{
    Vector2 nextPos = plPos;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) nextPos.x += plSpeed;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) nextPos.x -= plSpeed;
    if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) nextPos.y -= plSpeed;
    if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) nextPos.y += plSpeed;

    bool collision =
        map.IsWall(nextPos.x, nextPos.y) ||
        map.IsWall(nextPos.x + plSize.x, nextPos.y) ||
        map.IsWall(nextPos.x, nextPos.y + plSize.y) ||
        map.IsWall(nextPos.x + plSize.x, nextPos.y + plSize.y);

    if (!collision)
    {
        plPos = nextPos;
    }
}