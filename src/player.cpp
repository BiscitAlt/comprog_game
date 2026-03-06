#include "raylib.h"
#include "player.h"

void plCollision(Vector2 &plPos, Vector2 plSize, float plSpeed, Map &map)
{
    // สร้างตำแหน่งจำลองว่า "ถ้าเดินไปแล้วจะเป็นยังไง"
    Vector2 nextPos = plPos;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) nextPos.x += plSpeed;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) nextPos.x -= plSpeed;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) nextPos.y -= plSpeed;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) nextPos.y += plSpeed;

    // เช็ค 4 มุมของตัวละครว่าชนกำแพงไหม
    bool collision = 
        map.IsWall(nextPos.x, nextPos.y) ||                         // มุมบนซ้าย
        map.IsWall(nextPos.x + plSize.x, nextPos.y) ||              // มุมบนขวา
        map.IsWall(nextPos.x, nextPos.y + plSize.y) ||              // มุมล่างซ้าย
        map.IsWall(nextPos.x + plSize.x, nextPos.y + plSize.y);     // มุมล่างขวา

    if (!collision) {
        // ถ้าไม่ชนทุกมุม ถึงจะอนุญาตให้ตำแหน่งปัจจุบันเปลี่ยนไปเป็นตำแหน่งใหม่
        plPos = nextPos;
    }
}