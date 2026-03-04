#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <vector>
#include <string>

// รวมไฟล์ Header ของระบบต่างๆ ที่แยกโมดูลไว้
#include "enemy.h"     
#include "player.h"    
#include "item.h"      
#include "map.h"       
#include "ui.h"        
#include "database.h"  

// --- ส่วนที่ 1: การจัดการสถานะหน้าจอ (Game State Management) ---
// ใช้สำหรับแบ่งแยกการทำงานระหว่าง หน้าเมนู, หน้าเล่นเกม และหน้าจบเกม
enum GameState { STATE_MENU, STATE_PLAYING, STATE_GAMEOVER };
GameState currentState = STATE_MENU; 

/**
 * ฟังก์ชันวาดปุ่มกด (UI Button)
 * ใช้สำหรับตรวจจับเมาส์และวาดกราฟิกปุ่มพร้อมข้อความ
 */
bool DrawMenuButton(Rectangle rect, const char* text, Color baseColor) {
    Vector2 mousePos = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePos, rect);
    Color drawColor = isHovered ? ColorBrightness(baseColor, 0.3f) : baseColor;
    
    DrawRectangleRec(rect, drawColor);
    DrawRectangleLinesEx(rect, 3, isHovered ? GOLD : DARKGRAY);
    
    int fontSize = 20;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, rect.x + (rect.width/2 - textWidth/2), rect.y + (rect.height/2 - fontSize/2), fontSize, RAYWHITE);
    
    return (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

int main() 
{
    // --- การตั้งค่าหน้าต่างและตัวแปรเริ่มต้น (INITIALIZATION) ---
    const int screenWidth = 720;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Hell project");
    SetTargetFPS(60); 
    ShowCursor(); 

    // สร้างอ็อบเจกต์ผู้เล่นและกำหนดค่าสถานะเริ่มต้น
    Vector2 plPos = { screenWidth / 2.0f, screenHeight / 2.0f };
    player pl = { plPos, {20.0f, 20.0f}, 2.0f, RED, "Hero", 100, 100, 50, 50, 10, {} };
    
    float attackAnimTimer = 0.0f; // ตัวนับเวลาสำหรับอนิเมชั่นการโจมตี
    bool isAttacking = false;     // สถานะเช็คการโจมตี
    
    std::vector<Enemy> enemies;            // รายการศัตรูในฉาก
    std::vector<MonsterTemplate> db = GetMonsterDb(); // ดึงข้อมูลมอนสเตอร์จากฐานข้อมูล
    Map gridMap(30, 30, 20, 0);            // อ็อบเจกต์แผนที่

    // ตั้งค่ากล้อง 2D สำหรับติดตามตัวละคร
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
     


// พื้นหลัง lobby
Texture2D lobbygame = LoadTexture("image_1_1772589236218.jpg");


// --- สร้างละอองไฟหน้า lobby ---
struct fireeffect {
    Vector2 pos;
    float speed;
    float alpha;
};

std::vector<fireeffect> embers;
for (int i = 0; i < 60; i++) { // สร้าง 60 เม็ด
    embers.push_back({
        {(float)GetRandomValue(0, 720), (float)GetRandomValue(0, 720)}, 
        (float)GetRandomValue(5, 15) / 10.0f, 
        (float)GetRandomValue(3, 8) / 10.0f
    });
}



    // --- 2. ลูปหลักของเกม (MAIN GAME LOOP) ---
    while (!WindowShouldClose()) 
    {
        // --- ส่วนอัปเดตตรรกะเกม (UPDATE LOGIC) ---
        // ระบบจะทำงานเฉพาะเมื่ออยู่ในสถานะเล่นเกม (STATE_PLAYING) เท่านั้น
        if (currentState == STATE_PLAYING) 
        {
            HideCursor();
            // อัปเดตตำแหน่งกล้องตามตัวละคร
            camera.target = (Vector2){ pl.pos.x + 10.0f, pl.pos.y + 10.0f };
            
            // ระบบซูมภาพด้วยเมาส์ (Camera Zoom)
            float scale = 0.2f * GetMouseWheelMove();
            camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125f, 64.0f);

            // ตรวจสอบการเคลื่อนที่และการชนกำแพงของผู้เล่น
            plCollision(pl.pos, pl.size, pl.speed, gridMap);

            // อัปเดตสถานะและการชนของศัตรูแต่ละตัว
            for (Enemy& e : enemies) {
                if (e.hp > 0) {
                    UpdateEnemy(e, pl.pos);
                    // ตรวจสอบการชนระหว่างศัตรูกับผู้เล่น เพื่อลดค่า HP
                    if (CheckCollisionRecs({pl.pos.x, pl.pos.y, pl.size.x, pl.size.y}, {e.pos.x, e.pos.y, e.size.x, e.size.y})) {
                        pl.hp -= e.attack * GetFrameTime();
                    }
                }
            }

            // ระบบการโจมตี (Player Attack)
            if (IsKeyPressed(KEY_SPACE)) {
                isAttacking = true;
                attackAnimTimer = 0.15f;
                // กำหนดขอบเขตการโจมตี (Attack Hitbox)
                Rectangle atkArea = { pl.pos.x - 20, pl.pos.y - 20, pl.size.x + 40, pl.size.y + 40 };
                for (Enemy& e : enemies) {
                    if (e.hp > 0 && CheckCollisionRecs(atkArea, {e.pos.x, e.pos.y, e.size.x, e.size.y})) {
                        e.hp -= pl.attack;
                        // ระบบผลักศัตรู (Knockback)
                        Vector2 dir = Vector2Normalize(Vector2Subtract(e.pos, pl.pos));
                        e.pos = Vector2Add(e.pos, Vector2Scale(dir, 15.0f));
                    }
                }
            }

            // จัดการเวลาการแสดงผลอนิเมชั่นโจมตี
            if (isAttacking) {
                attackAnimTimer -= GetFrameTime();
                if (attackAnimTimer <= 0) isAttacking = false;
            }

            // ตรวจสอบเงื่อนไขเมื่อผู้เล่นตาย
            if (pl.hp <= 0) {
                pl.hp = 0;
                currentState = STATE_GAMEOVER;
            }
        }

        // --- ส่วนการวาดกราฟิก (DRAWING) ---
        BeginDrawing();
            ClearBackground(BLACK);

            // การวาดหน้าจอเมนูหลัก
            if (currentState == STATE_MENU) 
            {
                   DrawTexturePro(lobbygame, 
        { 0, 0, (float)lobbygame.width, (float)lobbygame.height }, // ขนาดรูปต้นฉบับ
        { 0, 0, (float)screenWidth, (float)screenHeight },        // ขนาดที่จะวาดลงจอ
        { 0, 0 }, 0.0f, WHITE);

        // แสดงผลเอฟเฟคไทยดิว่ะ
    float breathing = sinf(GetTime() * 0.5f) * 5.0f; 
    DrawTexturePro(lobbygame, 
        { 0, 0, (float)lobbygame.width, (float)lobbygame.height }, 
        { -breathing, -breathing, 720 + (breathing * 2), 720 + (breathing * 2) }, 
        { 0, 0 }, 0.0f, WHITE);

    // 2. เอฟเฟกต์ไฟกะพริบ (Torch Flicker) ทำให้สีจอส้มวูบวาบนิดๆ
    float flicker = sinf(GetTime() * 4.0f) * 0.1f + 0.9f;
    DrawRectangle(0, 0, 720, 720, Fade(ORANGE, 0.03f * (1.0f - flicker)));

    // 3. วาดละอองไฟ (Embers) ลอยขึ้น
    for (auto& p : embers) {
        p.pos.y -= p.speed; // ให้ลอยขึ้นข้างบน
        p.pos.x += sinf(GetTime() + p.pos.y) * 0.3f; // ส่ายนิดๆ
        if (p.pos.y < -10) p.pos.y = 730; // ถ้าหลุดจอให้กลับไปเริ่มข้างล่างใหม่

        DrawCircleV(p.pos, 1.5f, Fade(GOLD, p.alpha * flicker)); // วาดจุดไฟสีทองจางๆ
    }

                DrawText(" we love anutin", 180, 150, 40, GOLD);
                
                // ปุ่มเริ่มเกม: ทำการรีเซ็ตค่าสถานะและสร้างโลกใหม่ (Reset Logic)
                if (DrawMenuButton({ 260, 300, 200, 60 }, "START ", DARKPURPLE)) {
                    pl.hp = 100;
                    pl.pos = { screenWidth / 2.0f, screenHeight / 2.0f };
                    enemies.clear(); 
                    
                    // สุ่มสร้างศัตรูใหม่ตามจำนวนที่กำหนด
                    int enemyCount = GetRandomValue(3, 6);
                    for (int i = 0; i < enemyCount; i++) {
                        Enemy e;
                        Vector2 spawnPos = { pl.pos.x + (float)GetRandomValue(-200, 200), pl.pos.y + (float)GetRandomValue(-200, 200) };
                        int randIdx = GetRandomValue(0, (int)db.size() - 1);
                        InitEnemy(e, spawnPos, db[randIdx].name, db[randIdx].hp, db[randIdx].atk);
                        e.color = db[randIdx].color;
                        enemies.push_back(e);
                    }
                    gridMap = Map(30, 30, 20, GetRandomValue(10, 50)); // สุ่มสร้างแผนที่ใหม่
                    currentState = STATE_PLAYING;
                }

                if (DrawMenuButton({ 260, 380, 200, 60 }, "EXIT", MAROON)) break;
                DrawCircleLines(GetMouseX(), GetMouseY(), 10, DARKGRAY); // วาดเป้าเล็งเมาส์
            }
            // การวาดภาพระหว่างการเล่นเกม
            else if (currentState == STATE_PLAYING) 
            {
                BeginMode2D(camera);
                    gridMap.Draw(); // วาดแผนที่
                    
                    if (isAttacking) { // วาดเอฟเฟกต์การโจมตี
                        DrawCircleLines(pl.pos.x + 10, pl.pos.y + 10, 40, Fade(PURPLE, 0.6f));
                    }
                    
                    DrawRectangleV(pl.pos, pl.size, pl.color); // วาดผู้เล่น
                    
                    for (const Enemy& e : enemies) { 
                        if(e.hp > 0) DrawEnemy(e); // วาดศัตรูที่ยังมีชีวิต
                    }
                EndMode2D();

                // วาด UI HUD (Heads-up Display)
                PlayerInfo uiData = { (int)pl.hp, pl.maxHp, (float)pl.mp, pl.speed, 1, 100, pl.skillList };
                DrawRoguelikeHUD(uiData, GetMousePosition());
            }
            // การวาดหน้าจอ Game Over
            else if (currentState == STATE_GAMEOVER) 
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(RED, 0.5f));
                DrawText("  YOU DIE", screenWidth/2 - 140, screenHeight/2 - 50, 45, RAYWHITE);
                
                if (DrawMenuButton({ 260, 380, 200, 60 }, "BACK TO MENU", DARKGRAY)) {
                    currentState = STATE_MENU;
                }
                DrawCircleLines(GetMouseX(), GetMouseY(), 10, RAYWHITE);
            }

        EndDrawing();
    }

    // ปิดหน้าต่างและคืนค่าทรัพยากร
    UnloadTexture(lobbygame);
    CloseWindow();
    return 0;
}