#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <vector>
#include <string>

// รวมไฟล์ Header ของระบบต่างๆ ที่แยกไว้ในไฟล์อื่น
#include "enemy.h"
#include "player.h"
#include "item.h"
#include "map.h"
#include "ui.h"
#include "database.h"

int main() 
{
    // --- 1. การตั้งค่าหน้าต่างและตัวแปรเริ่มต้น (INITIALIZATION) ---
    const int screenWidth = 720;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "ชื่อเกมยังไม่ตั้ง");
    SetTargetFPS(60); // ล็อกความเร็วเกมไว้ที่ 60 เฟรมต่อวินาที
    HideCursor();      // ซ่อนตัวชี้เมาส์ปกติเพื่อใช้เป้าเล็งจากระบบ UI

    // สร้างอ็อบเจกต์ผู้เล่นพร้อมกำหนดค่าพลังเริ่มต้น (ตำแหน่ง, ขนาด, พลังโจมตี ฯลฯ)
    Vector2 plPos = { screenWidth / 2.0f, screenHeight / 2.0f };
    player pl = { plPos, {20.0f, 20.0f}, 2.0f, RED, "Hero", 100, 100, 50, 50, 10, {} };

    // ตัวแปรสำหรับคุมสถานะอนิเมชั่นและระบบเกม
    float attackAnimTimer = 0.0f; // ตัวจับเวลาสำหรับแสดงผลเอฟเฟกต์การโจมตี
    bool isAttacking = false;    // สถานะเช็คว่ากำลังโจมตีอยู่หรือไม่
    bool isGameOver = false;     // สถานะเช็คว่าผู้เล่นตายแล้วหรือยัง

    // ระบบจัดการศัตรู: สุ่มจำนวนและดึงข้อมูลจาก Database
    int enemyCount = GetRandomValue(2, 5);
    std::vector<Enemy> enemies;
    std::vector<MonsterTemplate> db = GetMonsterDb();

    // ลูปสร้างศัตรูและสุ่มตำแหน่งเกิดรอบๆ ตัวผู้เล่น
    for (int i = 0; i < enemyCount; i++) {
        Enemy e;
        Vector2 spawnPos = { plPos.x + (float)GetRandomValue(-200, 200), plPos.y + (float)GetRandomValue(-200, 200) };
        int randIdx = GetRandomValue(0, db.size() - 1);
        InitEnemy(e, spawnPos, db[randIdx].name, db[randIdx].hp, db[randIdx].atk);
        e.color = db[randIdx].color; // ตั้งสีตามประเภทมอนสเตอร์ที่ดึงมาจากฐานข้อมูล
        enemies.push_back(e);
    }

    SetRandomSeed(GetTime()); // ตั้งค่า Seed การสุ่มให้เปลี่ยนไปตามเวลาจริง
    Map gridMap(30, 30, 20, GetRandomValue(0, 50)); // สร้างแผนที่แบบตาราง (Grid)

    // ตั้งค่ากล้อง 2D สำหรับติดตามตัวละคร
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };

    // --- 2. ลูปหลักของเกม (MAIN GAME LOOP) ---
    while (!WindowShouldClose()) 
    {
        // --- ส่วนอัปเดตตรรกะเกม (UPDATE LOGIC) ---
        if (!isGameOver) // จะทำงานเมื่อผู้เล่นยังไม่ตายเท่านั้น
        {
            // อัปเดตเป้าหมายกล้องให้เลื่อนตามตำแหน่งตัวละคร
            camera.target = (Vector2){ pl.pos.x + 10.0f, pl.pos.y + 10.0f };
            
            // ระบบการซูม (Zoom) ด้วยลูกกลิ้งเมาส์
            float scale = 0.2f * GetMouseWheelMove();
            camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125f, 64.0f);
            if (IsKeyPressed(KEY_R)) camera.zoom = 1.0f; // กด R เพื่อรีเซ็ตระยะการซูม

            // ตรวจสอบการชนกำแพงและการเคลื่อนที่ของผู้เล่น
            plCollision(pl.pos, pl.size, pl.speed, gridMap);

            // วนลูปจัดการศัตรูแต่ละตัว
            for (Enemy& e : enemies) {
                if (e.hp > 0) { // ทำงานเฉพาะตัวที่ยังมีชีวิต
                    UpdateEnemy(e, pl.pos); // ให้ศัตรูคำนวณทิศทางเดินหาผู้เล่น
                    // ตรวจสอบการชนระหว่างศัตรูกับผู้เล่น เพื่อลด HP ผู้เล่น
                    if (CheckCollisionRecs({pl.pos.x, pl.pos.y, pl.size.x, pl.size.y}, {e.pos.x, e.pos.y, e.size.x, e.size.y})) {
                        pl.hp -= e.attack * GetFrameTime(); // เลือดลดตามพลังโจมตีมอนสเตอร์ต่อเฟรม
                    }
                }
            }

            // ระบบการโจมตี (กด Spacebar)
            if (IsKeyPressed(KEY_SPACE)) {
                isAttacking = true;
                attackAnimTimer = 0.15f; // กำหนดระยะเวลาการแสดงเอฟเฟกต์
                // สร้างพื้นที่การโจมตี (Hitbox) รอบตัวผู้เล่น
                Rectangle atkArea = { pl.pos.x - 20, pl.pos.y - 20, pl.size.x + 40, pl.size.y + 40 };
                for (Enemy& e : enemies) {
                    if (e.hp > 0 && CheckCollisionRecs(atkArea, {e.pos.x, e.pos.y, e.size.x, e.size.y})) {
                        e.hp -= pl.attack; // ลดเลือดมอนสเตอร์
                        // ระบบผลักศัตรู (Knockback) เมื่อโดนโจมตี
                        Vector2 dir = Vector2Normalize(Vector2Subtract(e.pos, pl.pos));
                        e.pos = Vector2Add(e.pos, Vector2Scale(dir, 15.0f));
                    }
                }
            }

            // จัดการนับถอยหลังเวลาของอนิเมชั่นการโจมตี
            if (isAttacking) {
                attackAnimTimer -= GetFrameTime();
                if (attackAnimTimer <= 0) isAttacking = false;
            }

            // ตรวจสอบเงื่อนไขการแพ้ (HP หมด)
            if (pl.hp <= 0) {
                pl.hp = 0;
                isGameOver = true;
            }
        } 
        else // ส่วนการทำงานเมื่อตาย (GAME OVER STATE)
        {
            // ตรวจสอบการกด Enter เพื่อเริ่มเกมใหม่
            if (IsKeyPressed(KEY_ENTER)) {
                pl.hp = 100;
                pl.pos = { screenWidth / 2.0f, screenHeight / 2.0f };
                isGameOver = false; // กลับไปสถานะเริ่มเล่นใหม่
            }
        }

        // --- ส่วนการวาดกราฟิก (DRAWING) ---
        BeginDrawing();
            ClearBackground(RAYWHITE); // ล้างหน้าจอด้วยสีพื้นหลัง

            BeginMode2D(camera); // เริ่มโหมดวาดในโลก 2D (ขยับตามกล้อง)
                gridMap.Draw(); // วาดพื้นหลัง/แผนที่
                
                // วาดเอฟเฟกต์วงสวิงดาบเมื่อมีการโจมตี
                if (isAttacking) {
                    DrawCircleLines(pl.pos.x + 10, pl.pos.y + 10, 40, Fade(DARKPURPLE, 0.7f));
                    DrawCircleSectorLines({pl.pos.x + 10, pl.pos.y + 10}, 35, 0, 360, 16, Fade(YELLOW, 0.4f));
                }

                DrawRectangleV(pl.pos, pl.size, pl.color); // วาดตัวละครผู้เล่น
                
                // วาดศัตรูทุกตัวในลิสต์
                for (const Enemy& e : enemies) {
                    DrawEnemy(e);
                }
            EndMode2D(); // จบโหมดวาดในโลก 2D

            // ส่วนวาด UI (จะวาดทับหน้าจอปกติ ไม่ขยับตามกล้อง)
            PlayerInfo uiData = { pl.hp, pl.maxHp, (float)pl.mp, pl.speed, 1, 100, pl.skillList };
            DrawRoguelikeHUD(uiData, GetMousePosition()); // เรียกใช้ฟังก์ชันวาด HUD จากไฟล์ UI

            // แสดงข้อความและหน้าจอเมื่อตาย
            if (isGameOver) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(RED, 0.4f)); // ระบายสีแดงโปร่งแสงเต็มหน้าจอ
                DrawText("YOU DIE", screenWidth/2 - 120, screenHeight/2 - 50, 60, RAYWHITE);
                DrawText("Press ENTER to Restart", screenWidth/2 - 110, screenHeight/2 + 20, 20, YELLOW);
            }
        EndDrawing();
    }

    // ปิดการใช้งานหน้าต่างและคืนค่าทรัพยากร
    CloseWindow();
    return 0;
}