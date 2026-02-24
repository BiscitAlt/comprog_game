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

struct PlayerBullet {
    Vector2 pos;
    Vector2 vel;
    bool active;
};

int main() {
    const int screenWidth = 720;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "ชื่อเกมยังไม่ได้ตั้ง");
    SetTargetFPS(60);
    HideCursor();

    player pl = { {screenWidth/2.0f, screenHeight/2.0f}, {22, 22}, 2.4f, 100.0f, RED, 100, 100, 1, 0, 10 };

    std::vector<Enemy> enemies;
    std::vector<Gem> gems;           
    std::vector<PlayerBullet> pBullets; 

    Map gridMap(40, 40, 25, 60); 
    Camera2D camera = { {screenWidth/2.0f, screenHeight/2.0f}, {0,0}, 0.0f, 1.0f };

    float plInvincTimer = 0;
    float shootTimer = 0;
    float screenShake = 0; 

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (plInvincTimer > 0) plInvincTimer -= dt;
        if (screenShake > 0) screenShake -= dt;

       
        plUpdate(pl, gridMap);
        
       
        Vector2 finalTarget = { pl.pos.x + 11, pl.pos.y + 11 };
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

        for (int i = gems.size() - 1; i >= 0; i--) {
            float d = Vector2Distance(pl.pos, gems[i].pos);
            if (d < 80.0f) gems[i].pos = Vector2MoveTowards(gems[i].pos, pl.pos, 4.0f); 
            if (d < 15.0f) {
                pl.exp += gems[i].value;
                gems.erase(gems.begin() + i);
                if (pl.exp >= pl.expNext) { pl.level++; pl.exp = 0; pl.expNext += 10; pl.hp = pl.hpMax; }
            }
        }

        if (pl.hp <= 0) break;

        
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
            gridMap.Draw();
            for (const auto& g : gems) DrawCircleV(g.pos, 5, SKYBLUE); 
            for (const auto& b : pBullets) DrawCircleV(b.pos, 5, YELLOW); 
            
            if (plInvincTimer <= 0 || (int)(GetTime()*15)%2 == 0) DrawRectangleV(pl.pos, pl.size, pl.color);
            for (const auto& e : enemies) DrawEnemy(e);
        EndMode2D();

        
        DrawRectangle(10, 10, 200, 20, DARKGRAY);
        DrawRectangle(10, 10, 200 * ((float)pl.hp/pl.hpMax), 20, RED);
        DrawText(TextFormat("LV: %i  EXP: %i/%i", pl.level, pl.exp, pl.expNext), 10, 40, 20, WHITE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}