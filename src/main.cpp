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

enum StartWeapon
{
    START_SHOTGUN,
    START_ROCKET,

    START_SWORD_ENERGY,
    START_SWORD_SPIN,
    START_SWORD_LIFESTEAL,

    START_FIRE_STAFF,
    START_ICE_WAND,
    START_LIGHTNING_ROD
};

struct PlayerBullet {
    Vector2 pos;
    Vector2 vel;
    bool active;
};

int GetRandomSkill()
{
    return GetRandomValue(0,6);
}

int main() 
{
    const int screenWidth = 720;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Hell project");
    SetTargetFPS(60); 

    // ===== Load Weapon Textures =====
        fireStaffTex = LoadTexture("assets/weapons/fire_staff.png");
        iceWandTex = LoadTexture("assets/weapons/ice_wand.png");
        lightningRodTex = LoadTexture("assets/weapons/lightning_rod.png");
        shotgunTex = LoadTexture("assets/weapons/shot_gun.png");
        magnumTex = LoadTexture("assets/weapons/mag_num.png");
       spiritgrimoireTex = LoadTexture("assets/weapons/spirit_grimoire.png");
       spiritorbTex = LoadTexture("assets/weapons/spirit_orb.png");
        spiritswordTex = LoadTexture("assets/weapons/spirit_sword.png");

        // ===== Load Monster Textures =====
        Texture2D slimeTex = LoadTexture("assets/monster/slime.png");
        Texture2D goblinTex = LoadTexture("assets/monster/goblin.png");
        Texture2D poisonTex = LoadTexture("assets/monster/poison.png");
        Texture2D bombTex = LoadTexture("assets/monster/bomb.png");

    enum GameState { STATE_MENU, STATE_PLAYING, STATE_GAMEOVER, STATE_PAUSED};
    GameState currentState = STATE_MENU; 

    Vector2 plPos = { screenWidth / 2.0f, screenHeight / 2.0f };
    player pl;

    pl.pos = plPos;
    pl.size = {22.0f,22.0f};
    pl.speed = 2.4f;
    pl.color = RED;

    pl.name = "Hero";

    pl.hp = 50;
    pl.hpMax = 50;

    pl.mana = 100;
    pl.manaMax = 100;

    pl.attack = 10;

    pl.level = 1;
    pl.exp = 0;
    pl.expNext = 10;
    pl.texture = LoadTexture("assets/player_walk.png");
    pl.frame = 0;
    pl.direction = 0;
    pl.frameTime = 0;
    pl.faceRight = true;

    SkillState skills = {};
    skills.voidMeteorTimer = -9999;
    skills.phantomBladeTimer = -9999;
    skills.chainLightningTimer = -9999;
    skills.poisonMistTimer = -9999;
    skills.blackHoleTimer = -9999;
    skills.bloodAuraTimer = -9999;
    skills.shockwaveTimer = -9999;

    float attackAnimTimer = 0.0f; 
    bool isAttacking = false;     
    float plInvincTimer = 0;
    float screenShake = 0; 
    
    std::vector<Enemy> enemies;            
    std::vector<Gem> gems;           
    std::vector<PlayerBullet> pBullets;

    // ===== Weapon =====
    Sword sword;
    std::vector<SwordWave> swordWaves;

    Gun gun;
    std::vector<Bullet> bullets;

    MagicWeapon magic;
    std::vector<MagicProjectile> magicProjectiles;

    WeaponType currentWeapon = WEAPON_NONE;
    StartWeapon startWeapon;
    std::vector<MonsterTemplate> db = GetMonsterDb(); // ดึงข้อมูลมอนสเตอร์จากฐานข้อมูล  
    Map worldMap; // สุ่มสร้างแผนที่ใหม่
    worldMap.LoadAssets(); // โหลดทรัพยากรของแผนที่ (เรียกครั้งเดียวตอนเริ่มเกม)

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


    std::vector<int> activeSkills;

    const int MAX_SKILLS = 5;
    while (!WindowShouldClose()) 
    {
        float dt = GetFrameTime();
        UpdateSkills(skills, pl, enemies, dt);

        if (currentState == STATE_PLAYING) 
        {
            HideCursor();
            if (plInvincTimer > 0) plInvincTimer -= dt;
            if (screenShake > 0) screenShake -= dt;

            plUpdate(pl, worldMap);
            Vector2 dir = {
            (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A)),
            (float)(IsKeyDown(KEY_S) - IsKeyDown(KEY_W))
            };

            if(currentWeapon == WEAPON_GUN)
{
            UpdateGun(gun, pl.pos, pl.size, dt);
}

            if(currentWeapon == WEAPON_SWORD)
{
            UpdateSword(sword, pl.pos, pl.size, dir, enemies);
}

            if(currentWeapon == WEAPON_MAGIC)
{
            UpdateMagicWeapon(magic, pl.pos, pl.size, dt);
}

            float scale = 0.2f * GetMouseWheelMove();
            camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.8f, 1.4f);
            Vector2 finalTarget = pl.pos;
            if (screenShake > 0) {
                finalTarget.x += GetRandomValue(-6, 6);
                finalTarget.y += GetRandomValue(-6, 6);
            }
            camera.target = finalTarget;


if (!enemies.empty())
{
    float minDist = 999999;
    int targetIndex = -1;

    for (int i = 0; i < enemies.size(); i++)
    {
        float d = Vector2Distance(pl.pos, enemies[i].pos);
        if (d < minDist)
        {
            minDist = d;
            targetIndex = i;
        }
    }

    if (targetIndex != -1)
    {
        Vector2 targetPos = enemies[targetIndex].pos;
        Vector2 diff = Vector2Subtract(targetPos, pl.pos);

        Vector2 shootDir =
        (Vector2Length(diff) > 0.01f)
        ? Vector2Normalize(diff)
        : (Vector2){1,0};
        if (currentWeapon == WEAPON_GUN)
{
        ShootGun(gun, bullets, pl.pos, pl.size, shootDir, pl.mana);
}

        if (currentWeapon == WEAPON_SWORD)
            UseSword(sword, pl, shootDir, swordWaves, enemies);

        if (currentWeapon == WEAPON_MAGIC)
            TryShootMagic(magic, magicProjectiles, pl.pos, targetPos);
        }
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
                UpdateEnemy(enemies[i], pl.pos, worldMap);
                // ===== POISON DAMAGE =====
                if (enemies[i].type == POISON && enemies[i].poisonActive)
                {
                 if (enemies[i].poisonTimer >= enemies[i].poisonInterval)
                {
                    pl.hp -= enemies[i].atk;
                    enemies[i].poisonTimer = 0;

                    screenShake = 0.15f;
                }
                 
                
                }// ===== ENEMY BULLET HIT PLAYER =====
                for (auto& b : enemies[i].bullets)
                {
                if (!b.active) continue;

                 Rectangle playerRect = {pl.pos.x, pl.pos.y, pl.size.x, pl.size.y};

                 if (CheckCollisionCircleRec(b.pos, b.radius, playerRect))
                {
                    if (plInvincTimer <= 0)
                {
                 pl.hp -= enemies[i].atk;
                 plInvincTimer = 0.4f;
                 screenShake = 0.2f;
                             }

                 b.active = false;
                    }
                }
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
                    if (pl.exp >= pl.expNext) 
{
                pl.level++;
                pl.exp = 0;
                pl.expNext += 10;
                pl.hp = pl.hpMax;

                if(pl.level == 3 || pl.level == 5 || pl.level == 9 || pl.level == 15)
{
                if(activeSkills.size() < MAX_SKILLS)
                {
                    int newSkill = GetRandomSkill();
                    activeSkills.push_back(newSkill);

                    switch(newSkill)
                    {
                        case 0: skills.voidMeteorTimer = 0; break;
                        case 1: skills.phantomBladeTimer = 0; break;
                        case 2: skills.chainLightningTimer = 0; break;
                        case 3: skills.poisonMistTimer = 0; break;
                        case 4: skills.blackHoleTimer = 0; break;
                        case 5: skills.bloodAuraTimer = 0; break;
                        case 6: skills.shockwaveTimer = 0; break;
                    }
                }
}
}
                }
            }

            if (worldMap.HitSpike(pl.pos.x, pl.pos.y)) {
                if (plInvincTimer <= 0) {
                    pl.hp -= 10; 
                    plInvincTimer = 0.6f;
                    screenShake = 0.25f; 
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
                startWeapon = (StartWeapon)GetRandomValue(0,7);
                gun.pickedUp = false;
                sword.pickedUp = false;
                magic.pickedUp = false;
                bullets.clear();
                swordWaves.clear();
                magicProjectiles.clear();

                switch(startWeapon)
                {
                case START_SHOTGUN:
                    InitGun(gun, pl.pos, GunType::SHOTGUN);
                    gun.pickedUp = true;
                    currentWeapon = WEAPON_GUN;
                   
                    break;

                case START_ROCKET:
                    InitGun(gun, pl.pos, GunType::ROCKET);
                    gun.pickedUp = true;
                    currentWeapon = WEAPON_GUN;
                   
                    break;

                case START_SWORD_ENERGY:
                    InitSword(sword, pl.pos, SWORD_ENERGY);
                    sword.pickedUp = true;
                    currentWeapon = WEAPON_SWORD;
                    break;

                case START_SWORD_SPIN:
                    InitSword(sword, pl.pos, SWORD_SPIN);
                    sword.pickedUp = true;
                    currentWeapon = WEAPON_SWORD;
                    break;

                case START_SWORD_LIFESTEAL:
                    InitSword(sword, pl.pos, SWORD_LIFESTEAL);
                    sword.pickedUp = true;
                    currentWeapon = WEAPON_SWORD;
                    break;

                case START_FIRE_STAFF:
                    InitMagicWeapon(magic, pl.pos, FIRE_STAFF);
                    magic.pickedUp = true;
                    currentWeapon = WEAPON_MAGIC;
                    break;

                case START_ICE_WAND:
                    InitMagicWeapon(magic, pl.pos, ICE_WAND);
                    magic.pickedUp = true;
                    currentWeapon = WEAPON_MAGIC;
                    break;

                case START_LIGHTNING_ROD:
                    InitMagicWeapon(magic, pl.pos, LIGHTNING_ROD);
                    magic.pickedUp = true;
                    currentWeapon = WEAPON_MAGIC;
                    break;
}
                skills = {};   // รีเซ็ต struct ทั้งหมด

                    skills.voidMeteorTimer = -9999;
                    skills.phantomBladeTimer = -9999;
                    skills.chainLightningTimer = -9999;
                    skills.poisonMistTimer = -9999;
                    skills.blackHoleTimer = -9999;
                    skills.bloodAuraTimer = -9999;
                    skills.shockwaveTimer = -9999;

                    activeSkills.clear();
                
                // 4. เข้าเกม!
                worldMap.obstacles.clear(); // เคลียร์สิ่งกีดขวางเก่า
                worldMap.decorations.clear(); // เคลียร์ของตกแต่งเก่า
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

            worldMap.UpdateMap(pl.pos);

            BeginMode2D(camera);
                worldMap.Draw(pl.pos, screenWidth, screenHeight); 
                DrawSkillEffects(skills, pl);
                for (const auto& g : gems) DrawCircleV(g.pos, 5, SKYBLUE); 
                for (const auto& b : pBullets) DrawCircleV(b.pos, 5, YELLOW); 
                
                if (isAttacking) { 
                    DrawCircleLines(pl.pos.x + 11, pl.pos.y + 11, 40, Fade(PURPLE, 0.6f));
                }
                
                if (plInvincTimer <= 0 || (int)(GetTime()*15)%2 == 0) DrawPlayer(pl); 
                
                for (const Enemy& e : enemies) { 
                    if(e.hp > 0) DrawEnemy(e);    
                }

                // วาดอาวุธ (อยู่นอก loop)
                if(currentWeapon == WEAPON_SWORD)
{
                DrawSword(sword, pl.pos, pl.size, dir);
}

                if(currentWeapon == WEAPON_GUN)
{
                DrawGun(gun, pl.pos, pl.size, dir);
}

                if(currentWeapon == WEAPON_MAGIC)
{
                DrawMagicWeapon(magic, pl.pos, dir);
}

                // projectiles
                DrawSwordWaves(swordWaves);
               DrawMagicProjectiles(magicProjectiles);

                for(const Bullet& b : bullets)
                {
                    DrawBullet(b);
                }
                DrawBulletExplosions();
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
    worldMap.UnloadAssets();

    for (auto& e : enemies)
    {
    UnloadTexture(e.texture);
    }
    UnloadTexture(spiritgrimoireTex);
    UnloadTexture(spiritorbTex);
    UnloadTexture(spiritswordTex);
    UnloadTexture(shotgunTex);
    UnloadTexture(magnumTex);
    UnloadTexture(pl.texture);
    UnloadTexture(fireStaffTex);
    UnloadTexture(iceWandTex);
    UnloadTexture(lightningRodTex);
    UnloadTexture(lobbygame);
    CloseWindow();
    return 0;
}