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
#include "database.h"

#include "sword.h"
#include "gun.h"
#include "bullet.h"
#include "magic_weapon.h"
#include "magic_projectile.h"
#include "skill.h"

enum WeaponType {
    WEAPON_NONE,
    WEAPON_SWORD,
    WEAPON_GUN,
    WEAPON_MAGIC
};

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

    enum GameState { STATE_MENU, STATE_PLAYING, STATE_GAMEOVER, STATE_PAUSED};
    GameState currentState = STATE_MENU; 

    Vector2 plPos = { screenWidth / 2.0f, screenHeight / 2.0f };
    player pl ={plPos,{22.0f,22.0f},2.4f,RED,"Hero",50,50,100,100,10,1,0,10,{}};

    SkillState skills = {};
    skills.voidMeteorTimer = -9999;
    skills.phantomBladeTimer = -9999;
    skills.chainLightningTimer = -9999;
    skills.poisonMistTimer = -9999;
    skills.blackHoleTimer = -9999;
    skills.bloodAuraTimer = -9999;
    skills.shockwaveTimer = -9999;

    int testSkill = GetRandomValue(0,6);

    switch(testSkill)
    {
    case 0: skills.voidMeteorTimer = 0; break;
    case 1: skills.phantomBladeTimer = 0; break;
    case 2: skills.chainLightningTimer = 0; break;
    case 3: skills.poisonMistTimer = 0; break;
    case 4: skills.blackHoleTimer = 0; break;
    case 5: skills.bloodAuraTimer = 0; break;
    case 6: skills.shockwaveTimer = 0; break;
    }
    
    float attackAnimTimer = 0.0f; 
    bool isAttacking = false;     
    float plInvincTimer = 0;
    float shootTimer = 0;
    float screenShake = 0; 
    
    std::vector<Enemy> enemies;            
    std::vector<Gem> gems;           
    std::vector<PlayerBullet> pBullets;

    // ===== Weapon =====
    Sword sword;
    SwordType swordType = (SwordType)GetRandomValue(0,2);
    InitSword(sword,{pl.pos.x+100,pl.pos.y},swordType);
    std::vector<SwordWave> swordWaves;

    Gun gun;
    GunType gunType = (GunType)GetRandomValue(0,1);
    InitGun(gun,{pl.pos.x-120,pl.pos.y},gunType);
    std::vector<Bullet> bullets;

    MagicWeapon magic;
    MagicWeaponType magicType =
    (MagicWeaponType)GetRandomValue(0,2);
    InitMagicWeapon(magic,{pl.pos.x-160,pl.pos.y},magicType);

    std::vector<MagicProjectile> magicProjectiles;

    WeaponType currentWeapon = WEAPON_NONE;
    std::vector<MonsterTemplate> db = GetMonsterDb(); // ดึงข้อมูลมอนสเตอร์จากฐานข้อมูล  
    Map gridMap; // สุ่มสร้างแผนที่ใหม่
    gridMap.LoadAssets(); // โหลดทรัพยากรของแผนที่ (เรียกครั้งเดียวตอนเริ่มเกม)

    Camera2D camera = { 0 };
    camera.zoom = 1.2f;
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

    float autoAttackTimer = 0;
    while (!WindowShouldClose()) 
    {
        float dt = GetFrameTime();
        UpdateSkills(skills, pl, enemies, dt);

        if (currentState == STATE_PLAYING) 
        {
            HideCursor();
            if (plInvincTimer > 0) plInvincTimer -= dt;
            if (screenShake > 0) screenShake -= dt;

            plUpdate(pl, gridMap);
            Rectangle playerRec =
{
    pl.pos.x,
    pl.pos.y,
    pl.size.x,
    pl.size.y
};

Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);
Vector2 diff = Vector2Subtract(mouseWorld, pl.pos);

Vector2 dir =
(Vector2Length(diff) > 0.01f)
? Vector2Normalize(diff)
: (Vector2){1,0};

//
// ===== Sword =====
//
UpdateSword(sword, pl.pos, pl.size, dir, enemies);

if (sword.pickedUp && currentWeapon != WEAPON_SWORD)
{
    if (currentWeapon == WEAPON_GUN)
    {
        gun.pickedUp = false;
        gun.pos = Vector2Add(pl.pos,{ -40,0 });
    }

    if (currentWeapon == WEAPON_MAGIC)
    {
        magic.pickedUp = false;
        magic.pos = Vector2Add(pl.pos,{ 60,0 });
    }

    currentWeapon = WEAPON_SWORD;
}

//
// ===== Gun =====
//
UpdateGun(gun, pl.pos, pl.size);

if (gun.pickedUp && currentWeapon != WEAPON_GUN)
{
    if (currentWeapon == WEAPON_SWORD)
    {
        sword.pickedUp = false;
        sword.pos = Vector2Add(pl.pos,{ 40,0 });
    }

    if (currentWeapon == WEAPON_MAGIC)
    {
        magic.pickedUp = false;
        magic.pos = Vector2Add(pl.pos,{ 60,0 });
    }

    currentWeapon = WEAPON_GUN;
}

//
// ===== Magic =====
//
UpdateMagicWeapon(magic, pl.pos, pl.size, dt);

if (magic.pickedUp && currentWeapon != WEAPON_MAGIC)
{
    if (currentWeapon == WEAPON_SWORD)
    {
        sword.pickedUp = false;
        sword.pos = Vector2Add(pl.pos,{ 40,0 });
    }

    if (currentWeapon == WEAPON_GUN)
    {
        gun.pickedUp = false;
        gun.pos = Vector2Add(pl.pos,{ -40,0 });
    }

    currentWeapon = WEAPON_MAGIC;
}

            float scale = 0.2f * GetMouseWheelMove();
            camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 1.0f, 1.8f);
            Vector2 finalTarget = { pl.pos.x + 11.0f, pl.pos.y + 11.0f };
            if (screenShake > 0) {
                finalTarget.x += GetRandomValue(-6, 6);
                finalTarget.y += GetRandomValue(-6, 6);
            }
            camera.target = finalTarget;

            autoAttackTimer += dt;

            if (autoAttackTimer >= 0.3f && !enemies.empty())
{
            float minDist = 9999;
            int targetIndex = -1;

            for (int i=0;i<enemies.size();i++)
    {
            float d = Vector2Distance(pl.pos,enemies[i].pos);
            if(d<minDist)
        {
            minDist=d;
            targetIndex=i;
        }
    }

            if(targetIndex!=-1)
    {
            Vector2 targetPos=enemies[targetIndex].pos;

            Vector2 dir=
            Vector2Normalize(Vector2Subtract(targetPos,pl.pos));

            if(currentWeapon==WEAPON_GUN)
            ShootGun(gun,bullets,pl.pos,pl.size,dir,pl.mana);

            if(currentWeapon==WEAPON_SWORD)
            UseSword(sword,pl,dir,swordWaves,enemies);

            if(currentWeapon==WEAPON_MAGIC)
            TryShootMagic(magic,magicProjectiles,pl.pos,targetPos);
    }

    autoAttackTimer=0;
}
            UpdateBullets(bullets,enemies,dt);
            UpdateSwordWaves(swordWaves,enemies,dt);
            UpdateMagicProjectiles(magicProjectiles,enemies,dt);

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

            if (IsKeyPressed(KEY_P)) {
         if (currentState == STATE_PLAYING) currentState = STATE_PAUSED;
         else if (currentState == STATE_PAUSED) currentState = STATE_PLAYING;
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
                // 1. รีเซ็ตผู้เล่น
                pl.hp = pl.hpMax;
                pl.level = 1; pl.exp = 0; pl.expNext = 10;
                pl.pos = { screenWidth / 2.0f, screenHeight / 2.0f };
                
                // 2. เคลียร์ของเก่า
                enemies.clear(); 
                gems.clear();
                pBullets.clear();
                
                // 3. สุ่มมอนสเตอร์ใหม่
                int enemyCount = GetRandomValue(3, 6);
                for (int i = 0; i < enemyCount; i++) {
                    Enemy e;
                    Vector2 spawnPos = { pl.pos.x + (float)GetRandomValue(-200, 200), pl.pos.y + (float)GetRandomValue(-200, 200) };
                    int randIdx = GetRandomValue(0, (int)db.size() - 1);
                    InitEnemy(e, spawnPos, (EnemyType)GetRandomValue(0,3));
                    e.color = db[randIdx].color;
                    enemies.push_back(e);
                }
                
                // 4. เข้าเกม!
                currentState = STATE_PLAYING;
            }

            if (DrawMenuButton({ 260, 380, 200, 60 }, "EXIT", MAROON)) break;
        }
        else if (currentState == STATE_PLAYING) 
        {   Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);
            Vector2 diff = Vector2Subtract(mouseWorld, pl.pos);

            Vector2 dir =
            (Vector2Length(diff) > 0.01f)
            ? Vector2Normalize(diff)
            : (Vector2){1,0};
            //DrawSkillEffects(skills, pl);

            gridMap.UpdateEndless(pl.pos);

            BeginMode2D(camera);
                gridMap.Draw(pl.pos, screenWidth, screenHeight); 
                DrawSkillEffects(skills, pl);
                for (const auto& g : gems) DrawCircleV(g.pos, 5, SKYBLUE); 
                for (const auto& b : pBullets) DrawCircleV(b.pos, 5, YELLOW); 
                
                if (isAttacking) { 
                    DrawCircleLines(pl.pos.x + 11, pl.pos.y + 11, 40, Fade(PURPLE, 0.6f));
                }
                
                if (plInvincTimer <= 0 || (int)(GetTime()*15)%2 == 0) DrawRectangleV(pl.pos, pl.size, pl.color); 
                
                for (const Enemy& e : enemies) { 
                    if(e.hp > 0) DrawEnemy(e);    
                }

                // วาดอาวุธ (อยู่นอก loop)
                DrawSword(sword, pl.pos, pl.size, dir);
                DrawGun(gun, pl.pos, pl.size, dir);
                DrawMagicWeapon(magic, pl.pos, dir);

                // projectiles
                DrawSwordWaves(swordWaves);
                DrawMagicProjectiles(magicProjectiles);

                for(const Bullet& b : bullets)
{
                DrawBullet(b);
}
            EndMode2D();

            PlayerInfo uiData = { (int)pl.hp, pl.hpMax, 50.0f, 50.0f, pl.speed, 1, 100, pl.skillList };
            DrawRoguelikeHUD(uiData);
            DrawText(TextFormat("LV: %i  EXP: %i/%i", pl.level, pl.exp, pl.expNext), 10, 80, 20, WHITE);
        }
         // --- วางต่อจากจบส่วน STATE_PLAYING ---
        else if (currentState == STATE_PAUSED) 
{
    bool resume = false;
    bool quit = false;

    // เรียกฟังก์ชันจาก ui.cpp มาวาดเมนู
    DrawPauseMenu(screenWidth, screenHeight, &resume, &quit);

    // เช็คผลลัพธ์
    if (resume) currentState = STATE_PLAYING;
    if (quit)   currentState = STATE_MENU;
}

        // แสดงผลการPAUSEDเกม
        else if (currentState == STATE_GAMEOVER) 
        {
            ShowCursor();
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(RED, 0.5f));
            DrawText("  YOU DIE", screenWidth/2 - 130, screenHeight/2 - 100, 45, RAYWHITE);
            DrawText("  Your soul returns to the realm of the gods ", screenWidth/2.65 - 130, screenHeight/1.65 - 100, 20, RAYWHITE);
            if (DrawMenuButton({ 260, 380, 200, 60 }, "BACK TO MENU", DARKGRAY)) {
                currentState = STATE_MENU;
            }
        }

          
        if (currentState != STATE_PLAYING) {
            DrawFantasyCursor(); 
        } 
        EndDrawing();
    }

    // ปิดหน้าต่างและคืนค่าทรัพยากร
    gridMap.UnloadAssets();
    UnloadTexture(lobbygame);
    CloseWindow();
    return 0;
}