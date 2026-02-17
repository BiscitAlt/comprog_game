#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <vector>
#include <string>

// รวมไฟล์ Header ของแต่ละคน
#include "enemy.h"
#include "player.h"
#include "item.h"
#include "map.h"
#include "ui.h"
#include "database.h"


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
    player pl = { plPos, {20.0f, 20.0f}, 2.0f, RED ,"Hero", 100, 100, 50, 50, 10, {}};
    
    int enemyCount = GetRandomValue(2, 5);
    std::vector<Enemy> enemies;

for (int i = 0; i < enemyCount; i++)
{
    Enemy e;
    Vector2 spawnPos = {
        plPos.x + (float)GetRandomValue(-200, 200),
        plPos.y + (float)GetRandomValue(-200, 200)
    };

    InitEnemy(e, spawnPos,"Slime", 100, 10);
    enemies.push_back(e);
}

    SetRandomSeed(GetTime());
    Map gridMap(30, 30, 20, GetRandomValue(0, 50)); // สร้างแมพขนาด 30x30 ช่อง แต่ละช่องกว้าง 20 pixel และมีกล่องสุ่มเพิ่มในแมพ 20 กล่อง

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

        plCollision(pl.pos, pl.size, pl.speed, gridMap); // เช็คการชนของผู้เล่นกับกำแพง

        for (Enemy& e : enemies)
{
        UpdateEnemy(e, pl.pos);
}


        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);
            BeginMode2D(camera);

            gridMap.Draw();
            DrawRectangleV(pl.pos, pl.size, pl.color); // วาดผู้เล่น
           for (const Enemy& e : enemies)
{
             DrawEnemy(e);
}

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


