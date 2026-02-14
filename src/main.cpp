#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include <vector>
#include <iostream>

// รวมไฟล์ Header ของแต่ละคน
#include "enemy.h"
#include "player.h"
#include "item.h"
#include "map.h"
#include "ui.h"

// MAIN GAME
int main()
{
    // ตั้งค่าหน้าต่างเกม
    const int screenWidth = 720;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "ชื่อเกมยังไม่ตั้ง");
    SetTargetFPS(60); // ล็อกไว้ที่ 60 fps
    HideCursor();
    
    Vector2 plPos = { screenWidth/2.0f, screenHeight/2.0f }; // ตำแหน่งเริ่มต้นของผู้เล่น (อยู่ตรงกลางหน้าจอ)
    player pl = { plPos, {20.0f, 20.0f}, 2.0f, RED };

    Map gridMap;

    //กล้อง
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.target = (Vector2){ pl.pos.x + 10.0f, pl.pos.y + 10.0f };

    // Main Game Loop
    while (!WindowShouldClose())  // กดปิด หรือ ESC 
    {
        // Update
        //----------------------------------------------------------------------------------
        camera.target = (Vector2){ pl.pos.x + 10.0f, pl.pos.y + 10.0f }; // ให้กล้องตามผู้เล่น
        
        // Zoom ด้วย Mouse Wheel
        float scale = 0.2f*GetMouseWheelMove();
        camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 0.125f, 64.0f);

        if (IsKeyPressed(KEY_R)) camera.zoom = 1.0f; // รีเซ็ตการซูม

        plCollision(pl.pos, pl.size, pl.speed, gridMap);
        Rectangle playerRect = { pl.pos.x, pl.pos.y, pl.size.x, pl.size.y };

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);
            BeginMode2D(camera);

            gridMap.Draw();
            DrawRectangleV(pl.pos, pl.size, pl.color); // วาดผู้เล่น

            EndMode2D();

            textKey(pl.speed);
            cursor(GetMousePosition());

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // คืนค่าหน่วยความจำ
    CloseWindow();

    return 0;
}

