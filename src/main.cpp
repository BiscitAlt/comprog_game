#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <vector>
#include <algorithm>

#include "enemy.h"
#include "player.h"
#include "item.h"
#include "map.h"
#include "ui.h"
#include <string>     
#include "database.h" 

struct PlayerBullet {
    Vector2 pos;
    Vector2 vel;
    bool active;
};

int main() 
{
    const int screenWidth = 720;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Hell project");
    SetTargetFPS(60); 

    enum GameState { STATE_MENU, STATE_PLAYING, STATE_GAMEOVER };
    GameState currentState = STATE_MENU; 

    Vector2 plPos = { screenWidth / 2.0f, screenHeight / 2.0f };
    player pl = { plPos, {22.0f, 22.0f}, 2.4f, 100.0f, RED, "Hero", 50, 50, 10, {}, 100, 100, 1, 0, 10 };
    
    float attackAnimTimer = 0.0f; 
    bool isAttacking = false;     
    float plInvincTimer = 0;
    float shootTimer = 0;
    float screenShake = 0; 
    
    std::vector<Enemy> enemies;            
    std::vector<Gem> gems;           
    std::vector<PlayerBullet> pBullets; 

    std::vector<MonsterTemplate> db = GetMonsterDb(); 
    Map gridMap(40, 40, 25, 60); 
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };

    Texture2D lobbygame = LoadTexture("assets\\BG1\\image_1_1772589236218.jpg");
    std::vector<fireeffect> embers;
    for (int i = 0; i < 60; i++) { 
        embers.push_back({
            {(float)GetRandomValue(0, 720), (float)GetRandomValue(0, 720)}, 
            (float)GetRandomValue(5, 15) / 10.0f, 
            (float)GetRandomValue(3, 8) / 10.0f
        });
    }

    while (!WindowShouldClose()) 
    {
        float dt = GetFrameTime();

        if (currentState == STATE_PLAYING) 
        {
            HideCursor();
            if (plInvincTimer > 0) plInvincTimer -= dt;
            if (screenShake > 0) screenShake -= dt;

            plUpdate(pl, gridMap);

            float scale = 0.2f * GetMouseWheelMove();
            camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125f, 64.0f);
            Vector2 finalTarget = { pl.pos.x + 11.0f, pl.pos.y + 11.0f };
            if (screenShake > 0) {
                finalTarget.x += GetRandomValue(-6, 6);
                finalTarget.y += GetRandomValue(-6, 6);
            }
            camera.target = finalTarget;

            shootTimer += dt;
            if (shootTimer >= 0.6f && !enemies.empty()) { 
                float minDist = 400.0f; 
                int targetIdx = -1;
                for (int i = 0; i < enemies.size(); i++) {
                    float d = Vector2Distance(pl.pos, enemies[i].pos);
                    if (d < minDist) { minDist = d; targetIdx = i; }
                }
                if (targetIdx != -1) {
                    Vector2 dir = Vector2Normalize(Vector2Subtract(enemies[targetIdx].pos, pl.pos));
                    pBullets.push_back({ pl.pos, Vector2Scale(dir, 450.0f), true });
                    shootTimer = 0;
                }
            }

            for (auto& b : pBullets) {
                b.pos = Vector2Add(b.pos, Vector2Scale(b.vel, dt));
                for (auto& e : enemies) {
                    if (CheckCollisionCircleRec(b.pos, 5, {e.pos.x, e.pos.y, e.size.x, e.size.y})) {
                        e.hp -= 2; b.active = false; 
                    }
                }
            }
            pBullets.erase(std::remove_if(pBullets.begin(), pBullets.end(), [](PlayerBullet& b){ return !b.active; }), pBullets.end());

            if (enemies.size() < 7) {
                Enemy ne;
                Vector2 sp = { pl.pos.x + GetRandomValue(-500, 500), pl.pos.y + GetRandomValue(-500, 500) };
                if (Vector2Distance(sp, pl.pos) > 200) { InitEnemy(ne, sp, (EnemyType)GetRandomValue(0, 3)); enemies.push_back(ne); }
            }

            for (int i = enemies.size() - 1; i >= 0; i--) {
                UpdateEnemy(enemies[i], pl.pos);
                
                if (enemies[i].hp <= 0) {
                    gems.push_back({ enemies[i].pos, 2, true });
                    enemies.erase(enemies.begin() + i);
                    continue;
                }

                if (CheckCollisionRecs({pl.pos.x, pl.pos.y, pl.size.x, pl.size.y}, {enemies[i].pos.x, enemies[i].pos.y, enemies[i].size.x, enemies[i].size.y})) {
                    if (plInvincTimer <= 0) {
                        pl.hp -= enemies[i].atk;
                        plInvincTimer = 0.6f;
                        screenShake = 0.25f; 
                        if (enemies[i].type == EXPLODER) enemies.erase(enemies.begin() + i);
                    }
                }
            }

            if (IsKeyPressed(KEY_SPACE)) {
                isAttacking = true;
                attackAnimTimer = 0.15f;
                Rectangle atkArea = { pl.pos.x - 20, pl.pos.y - 20, pl.size.x + 40, pl.size.y + 40 };
                for (Enemy& e : enemies) {
                    if (e.hp > 0 && CheckCollisionRecs(atkArea, {e.pos.x, e.pos.y, e.size.x, e.size.y})) {
                        e.hp -= pl.attack;
                        Vector2 dir = Vector2Normalize(Vector2Subtract(e.pos, pl.pos));
                        e.pos = Vector2Add(e.pos, Vector2Scale(dir, 15.0f));
                    }
                }
            }
            if (isAttacking) {
                attackAnimTimer -= dt;
                if (attackAnimTimer <= 0) isAttacking = false;
            }

            for (int i = gems.size() - 1; i >= 0; i--) {
                float d = Vector2Distance(pl.pos, gems[i].pos);
                if (d < 80.0f) gems[i].pos = Vector2MoveTowards(gems[i].pos, pl.pos, 4.0f); 
                if (d < 15.0f) {
                    pl.exp += gems[i].value;
                    gems.erase(gems.begin() + i);
                    if (pl.exp >= pl.expNext) { pl.level++; pl.exp = 0; pl.expNext += 10; pl.hp = pl.hpMax; }
                }
            }

            if (pl.hp <= 0) {
                pl.hp = 0;
                currentState = STATE_GAMEOVER;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (currentState == STATE_MENU) 
        {
            ShowCursor();
            UpdateAndDrawMenuBackground(lobbygame, embers, screenWidth, screenHeight);

            const char* title = "HELL PROJECT"; 
            int titleWidth = MeasureText(title, 50);
            DrawText(title, (screenWidth/2 - titleWidth/2) + 2, 152, 50, Fade(BLACK, 0.5f)); 
            DrawText(title, screenWidth/2 - titleWidth/2, 150, 50, GOLD);
            
            if (DrawMenuButton({ 260, 300, 200, 60 }, "START ", DARKPURPLE)) {
                pl.hp = pl.hpMax;
                pl.level = 1; pl.exp = 0; pl.expNext = 10;
                pl.pos = { screenWidth / 2.0f, screenHeight / 2.0f };
                enemies.clear(); 
                gems.clear();
                pBullets.clear();
                
                int enemyCount = GetRandomValue(3, 6);
                for (int i = 0; i < enemyCount; i++) {
                    Enemy e;
                    Vector2 spawnPos = { pl.pos.x + (float)GetRandomValue(-200, 200), pl.pos.y + (float)GetRandomValue(-200, 200) };
                    InitEnemy(e, spawnPos, (EnemyType)GetRandomValue(0, 3));
                    enemies.push_back(e);
                }
                gridMap = Map(40, 40, 25, 60); 
                currentState = STATE_PLAYING;
            }

            if (DrawMenuButton({ 260, 380, 200, 60 }, "EXIT", MAROON)) break;
        }
        else if (currentState == STATE_PLAYING) 
        {
            BeginMode2D(camera);
                gridMap.Draw(); 
                for (const auto& g : gems) DrawCircleV(g.pos, 5, SKYBLUE); 
                for (const auto& b : pBullets) DrawCircleV(b.pos, 5, YELLOW); 
                
                if (isAttacking) { 
                    DrawCircleLines(pl.pos.x + 11, pl.pos.y + 11, 40, Fade(PURPLE, 0.6f));
                }
                
                if (plInvincTimer <= 0 || (int)(GetTime()*15)%2 == 0) DrawRectangleV(pl.pos, pl.size, pl.color); 
                
                for (const Enemy& e : enemies) { 
                    if(e.hp > 0) DrawEnemy(e); 
                }
            EndMode2D();

            PlayerInfo uiData = { (int)pl.hp, pl.hpMax, 50.0f, 50.0f, pl.speed, 1, 100, pl.skillList };
            DrawRoguelikeHUD(uiData);
            DrawText(TextFormat("LV: %i  EXP: %i/%i", pl.level, pl.exp, pl.expNext), 10, 80, 20, WHITE);
        }
        else if (currentState == STATE_GAMEOVER) 
        {
            ShowCursor();
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(RED, 0.5f));
            DrawText("  YOU DIE", screenWidth/2 - 130, screenHeight/2 - 100, 45, RAYWHITE);
            
            if (DrawMenuButton({ 260, 380, 200, 60 }, "BACK TO MENU", DARKGRAY)) {
                currentState = STATE_MENU;
            }
        }
          
        if (currentState != STATE_PLAYING) {
            DrawFantasyCursor(); 
        } 
        EndDrawing();
    }

    UnloadTexture(lobbygame);
    CloseWindow();
    return 0;
}