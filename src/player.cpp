#include "player.h"
#include "raymath.h"

// ======================
// Update Player
// ======================
void plUpdate(player &pl, Map &map)
{
    plMovement(pl.pos, pl.speed);

    plCollision(
        pl.pos,
        pl.size,
        pl.speed,
        map
    );
}

// ======================
// Movement
// ======================
void plMovement(Vector2 &plPos, float speed)
{
    if (IsKeyDown(KEY_W)) plPos.y -= speed;
    if (IsKeyDown(KEY_S)) plPos.y += speed;
    if (IsKeyDown(KEY_A)) plPos.x -= speed;
    if (IsKeyDown(KEY_D)) plPos.x += speed;
}

// ======================
// Collision (ยังไม่ใช้ Map)
// ======================
void plCollision(
    Vector2 &plPos,
    Vector2 plSize,
    float plSpeed,
    Map &map
)
{
    // กันหลุดจอ (แทน collision map)
    if (plPos.x < 0) plPos.x = 0;
    if (plPos.y < 0) plPos.y = 0;

    if (plPos.x > GetScreenWidth() - plSize.x)
        plPos.x = GetScreenWidth() - plSize.x;

    if (plPos.y > GetScreenHeight() - plSize.y)
        plPos.y = GetScreenHeight() - plSize.y;
}

// ======================
// Draw Player
// ======================
void DrawPlayer(player &pl)
{
    DrawRectangleV(pl.pos, pl.size, pl.color);
}