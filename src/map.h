#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include <vector>
#include <iostream>

void gridmap(Camera2D camera, Vector2 plPos, Vector2 plSize, Color plColor){
    ClearBackground(RAYWHITE);

            BeginMode2D(camera);
                // วาดกริดเพื่อให้เห็นการเคลื่อนที่ของกล้อง
                rlPushMatrix();
                    rlTranslatef(0, 25*50, 0);
                    rlRotatef(90, 1, 0, 0);
                    DrawGrid(100, 20);
                rlPopMatrix();
                DrawRectangleV({50, 0}, {50, 50}, GRAY); // วาดกำแพงตัวอย่าง
                DrawRectangleV(plPos, plSize, plColor); // วาดผู้เล่น

            EndMode2D();
}

void plCollision(Vector2 &plPos, Vector2 plSize, float plSpeed)
{
    if (CheckCollisionRecs((Rectangle){plPos.x, plPos.y, plSize.x, plSize.y}, (Rectangle){50, 0, 50, 50}))
    {
        // ชนกำแพง
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) plPos.x -= plSpeed;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) plPos.x += plSpeed;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) plPos.y += plSpeed;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) plPos.y -= plSpeed;  
    }
}