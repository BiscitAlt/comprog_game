#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <vector>

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
    player pl = { plPos, {20.0f, 20.0f}, 2.0f, RED, 100, 100 };
     
    int enemyCount = GetRandomValue(2, 5);
    std::vector<Enemy> enemies;

    for (int i = 0; i < enemyCount; i++) // สุ่มememy 2-5 ตัว
    {
        Enemy e;
        Vector2 spawnPos = {
            plPos.x + (float)GetRandomValue(-200, 200),
            plPos.y + (float)GetRandomValue(-200, 200)
        };

        InitEnemy(e, spawnPos);
        enemies.push_back(e);
    }

    SetRandomSeed(GetTime());
    Map gridMap(30, 30, 20, GetRandomValue(0, 50)); // สร้างแมพขนาด 30x30 ช่อง แต่ละช่องกว้าง 20 pixel และมีกล่องสุ่มเพิ่มในแมพ 20 กล่อง

    //กล้อง
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.target = (Vector2){ pl.pos.x + 10.0f, pl.pos.y + 10.0f };
    
    float plHitTimer = 0.0f; // ตัวกระพริบอตอนโดนตี
    float plInvincibleTimer = 0.0f; // ตัวป้องกันการโดนตี(อมตะชั่วคราว)

    // Main Game Loop
    while (!WindowShouldClose())  // กดปิด หรือ ESC 
    {
        // ลดเวลาตัวกระพริบจนหมด
        if (plHitTimer > 0)
            plHitTimer -= GetFrameTime();

        // ลดเวลาอมตะจนหมด
        if (plInvincibleTimer > 0)
            plInvincibleTimer -= GetFrameTime();

        // Update
        //----------------------------------------------------------------------------------
        camera.target = (Vector2){ pl.pos.x + 10.0f, pl.pos.y + 10.0f }; // ให้กล้องตามผู้เล่น
        
        // Zoom ด้วย Mouse Wheel
        float scale = 0.2f * GetMouseWheelMove();
        camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125f, 64.0f);

        if (IsKeyPressed(KEY_R)) camera.zoom = 1.0f; // รีเซ็ตการซูม

        plCollision(pl.pos, pl.size, pl.speed, gridMap); // เช็คการชนของผู้เล่นกับกำแพง

        for (Enemy& e : enemies) // enemy เคลื่อนตามผู้เล่น
        {
            UpdateEnemy(e, pl.pos);
        }

        Rectangle playerRec = { pl.pos.x, pl.pos.y, pl.size.x, pl.size.y }; // สร้างขอบเขตการชน

        // ศัตรูโจมตีผู้เล่น
        for (Enemy& e : enemies)
        {
            e.attackTimer -= GetFrameTime();

            Rectangle enemyRec = { e.pos.x, e.pos.y, e.size.x, e.size.y };

            if (CheckCollisionRecs(playerRec, enemyRec)
                && e.attackTimer <= 0
                && plInvincibleTimer <= 0)
            {
                pl.hp -= e.atk;

                e.attackTimer = 0.5f;
                plInvincibleTimer = 0.7f;
                plHitTimer = 0.3f;

                if (pl.hp < 0) pl.hp = 0;
            }
        }

        // ผู้เล่นโจมตี + ลบศัตรูที่ตาย
        if (IsKeyPressed(KEY_SPACE))
        {
            for (int i = enemies.size() - 1; i >= 0; i--)
            {
                Rectangle enemyRec = {
                    enemies[i].pos.x, enemies[i].pos.y,
                    enemies[i].size.x, enemies[i].size.y
                };

                if (CheckCollisionRecs(playerRec, enemyRec))
                {
                    enemies[i].hp -= 1;

                    if (enemies[i].hp <= 0)
                    {
                        enemies.erase(enemies.begin() + i);
                    }
                }
            }
        }

        if (pl.hp <= 0) // Game over
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("GAME OVER",
                screenWidth / 2 - 120,
                screenHeight / 2 - 20,
                40, RED);
            EndDrawing();

            WaitTime(2.0);
            break;
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);

        gridMap.Draw();
        // กระตอนโดนโจมตี
        if (plInvincibleTimer <= 0 || ((int)(plInvincibleTimer * 10) % 2 == 0))
        {
            DrawRectangleV(pl.pos, pl.size, pl.color); // วาดผู้เล่น
        }

        for (const Enemy& e : enemies)
        {
            DrawEnemy(e);
        }

        EndMode2D();

        float barWidth = 200;
        float barHeight = 20;
        float hpPercent = (float)pl.hp / pl.hpMax;

        DrawRectangle(10, 40, barWidth, barHeight, DARKGRAY);
        DrawRectangle(10, 40, barWidth * hpPercent, barHeight, RED);
        DrawRectangleLines(10, 40, barWidth, barHeight, BLACK);

        DrawText(TextFormat("HP: %d / %d", pl.hp, pl.hpMax), 10, 65, 20, BLACK);

        textKey(pl.speed);
        cursor(GetMousePosition());

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // คืนค่าหน่วยความจำ
    CloseWindow();

    return 0;
}

