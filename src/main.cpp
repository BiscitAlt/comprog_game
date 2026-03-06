    #include "raylib.h"
    #include "rlgl.h"
    #include "raymath.h"
    #include <vector>

    // รวมไฟล์ Header
    #include "enemy.h"
    #include "player.h"
    #include "item.h"
    #include "map.h"
    #include "ui.h"
    #include "sword.h"
    #include "gun.h"
    #include "bullet.h"
    #include "heal_potion.h"
    #include "magic_weapon.h"
    #include "magic_projectile.h"
    #include "loot_box.h"
    #include "exp_orb.h"

    std::vector<ExpOrb> expOrbs;

    // ===== ประเภทอาวุธ =====
    enum WeaponType {
        WEAPON_NONE,          // มือเปล่า
        WEAPON_SWORD,         // ดาบ
        WEAPON_GUN,           // ปืน
        WEAPON_MAGIC          // ★ เวทมนตร์ (เพิ่ม)
    };

    int main()
    {
        // =========================
        // Init
        // =========================
        const int screenWidth = 720;
        const int screenHeight = 720;

        InitWindow(screenWidth, screenHeight, "ชื่อเกมยังไม่ตั้ง");
        SetTargetFPS(60);
        HideCursor();

        // ===== Player =====
        Vector2 plPos = { screenWidth / 2.0f, screenHeight / 2.0f };
        player pl = { plPos, {20, 20}, 2.0f, RED, 100, 100, 100.0f, 100.0f,
              1,        // level
              0,        // exp
              50        // expToNext
        };
        // ===== Sword =====
        Sword sword;
        SwordType randomType = (SwordType)GetRandomValue(0, 2);
        InitSword(sword, { pl.pos.x + 100, pl.pos.y }, randomType);
        std::vector<SwordWave> swordWaves;

        // ===== Gun =====
        Gun gun;
        GunType type = (GunType)GetRandomValue(0, 2);
        InitGun(gun, { pl.pos.x - 120, pl.pos.y }, type);
        std::vector<Bullet> bullets;

        // ===== Heal Potion =====
        HealPotion healPotion;
        InitHealPotion(
            healPotion,
            { pl.pos.x + 60, pl.pos.y + 60 },
            30
        );

        // ===== Magic  =====
        MagicWeapon magic;
        InitMagicWeapon(magic, { pl.pos.x - 160, pl.pos.y });
        std::vector<MagicProjectile> magicProjectiles;

        // ===== Loot Box =====
        LootBox lootBox;
        InitLootBox(lootBox, { pl.pos.x + 200, pl.pos.y });

        LootType lootResult;
        bool lootOpened = false;

        // ===== Weapon state =====
        WeaponType currentWeapon = WEAPON_NONE;

        // ===== Enemy =====
        int enemyCount = GetRandomValue(2, 5);
        std::vector<Enemy> enemies;
        for (int i = 0; i < enemyCount; i++)
        {
            Enemy e;
            Vector2 spawnPos = {
                plPos.x + (float)GetRandomValue(-200, 200),
                plPos.y + (float)GetRandomValue(-200, 200)
            };
            InitEnemy(e, spawnPos);
            enemies.push_back(e);
        }

        // ===== Map =====
        SetRandomSeed(GetTime());
        Map gridMap(30, 30, 20, GetRandomValue(0, 50));

        // ===== Camera =====
        Camera2D camera = { 0 };
        camera.zoom = 1.0f;
        camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };

        float plHitTimer = 0.0f;
        float plInvincibleTimer = 0.0f;

        // =========================
        // Game Loop
        // =========================
        while (!WindowShouldClose())
        {
            float dt = GetFrameTime();
            
            // ===== Mana regen =====
            pl.mana += 0.5f * dt;   // ฟื้น 0.5/วินาที
            if (pl.mana > pl.manaMax)
            pl.mana = pl.manaMax;

            // ตัวกระพริบเมื่อโดนตี
            if (plHitTimer > 0) plHitTimer -= dt;
            if (plInvincibleTimer > 0) plInvincibleTimer -= dt;

            // ===== Camera update =====
            camera.target = { pl.pos.x + 10, pl.pos.y + 10 };
            float scale = 0.2f * GetMouseWheelMove();
            camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125f, 64.0f);
            if (IsKeyPressed(KEY_R)) camera.zoom = 1.0f;

             // ===== Player movement =====
            plCollision(pl.pos, pl.size, pl.speed, gridMap);

            // ===== Heal Potion update ===== (ของใหม่)
            UpdateHealPotion(healPotion, pl);

            // ===== Enemy update =====
            for (Enemy& e : enemies)
                UpdateEnemy(e, pl.pos);

            Rectangle playerRec = {
                pl.pos.x, pl.pos.y,
                pl.size.x, pl.size.y
            };
            for (ExpOrb& orb : expOrbs)
            {
                UpdateExpOrb(orb, playerRec, pl.exp);
            }
            if (pl.exp >= pl.expToNext)
            {
            pl.level++;
            pl.exp = 0;
            pl.expToNext += 20;  // เพิ่มความยาก

             pl.hpMax += 20;
            pl.manaMax += 15;

            pl.hp = pl.hpMax;        // เติมเต็ม
            pl.mana = pl.manaMax;
            }
            
            // ===== loot boox update =====
            lootOpened = false;
            UpdateLootBox(lootBox, playerRec, lootResult, lootOpened);

            if (lootOpened)
            {
            switch (lootResult)
            {
            case LOOT_HEAL:
            pl.hp = Clamp(pl.hp + 30, 0, pl.hpMax);
            break;

            case LOOT_SWORD:
            sword.pickedUp = true;
            currentWeapon = WEAPON_SWORD;
            break;

            case LOOT_GUN:
            gun.pickedUp = true;
            currentWeapon = WEAPON_GUN;
            break;

            case LOOT_MAGIC:
            magic.pickedUp = true;
            currentWeapon = WEAPON_MAGIC;
            break;
        }
    }

            // ===== Mouse direction =====
            Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);
            Vector2 diff = Vector2Subtract(mouseWorld, pl.pos);
            Vector2 dir = (Vector2Length(diff) > 0.01f)
                ? Vector2Normalize(diff)
                : (Vector2){ 1, 0 };

            // =========================
            // Weapon pickup
            // =========================

            // ----- Sword -----
            UpdateSword(sword, pl.pos, pl.size, dir, enemies);
            if (sword.pickedUp && currentWeapon != WEAPON_SWORD)
            {
                if (currentWeapon == WEAPON_GUN)
                {
                    gun.pickedUp = false;
                    gun.pos = Vector2Add(pl.pos, { -40, 0 });
                }

                if (currentWeapon == WEAPON_MAGIC)
                {
                    magic.pickedUp = false;
                    magic.pos = Vector2Add(pl.pos, { 60, 0 });
                }
                currentWeapon = WEAPON_SWORD;
            }

            // ----- Gun -----
            UpdateGun(gun, pl.pos, pl.size);
            if (gun.pickedUp && currentWeapon != WEAPON_GUN)
            {
                if (currentWeapon == WEAPON_SWORD)
                {
                    sword.pickedUp = false;
                    sword.pos = Vector2Add(pl.pos, { 40, 0 });
                }

                if (currentWeapon == WEAPON_MAGIC)
                {
                    magic.pickedUp = false;
                    magic.pos = Vector2Add(pl.pos, { 60, 0 });
                }
                currentWeapon = WEAPON_GUN;
            }

            // ----- Magic  -----
            UpdateMagicWeapon(magic, pl.pos, pl.size, dt);
            if (magic.pickedUp && currentWeapon != WEAPON_MAGIC)
            {
                if (currentWeapon == WEAPON_SWORD)
                {
                    sword.pickedUp = false;
                    sword.pos = Vector2Add(pl.pos, { 40, 0 });
                }
                if (currentWeapon == WEAPON_GUN)
                {
                    gun.pickedUp = false;
                    gun.pos = Vector2Add(pl.pos, { -40, 0 });
                }
                currentWeapon = WEAPON_MAGIC;
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

        // --- ส่วนการวาดกราฟิก  ---
        BeginDrawing();
            ClearBackground(BLACK);

            // การวาดหน้าจอเมนูหลัก
            if (currentState == STATE_MENU) 
            {
                UpdateAndDrawMenuBackground(lobbygame, embers, screenWidth, screenHeight);

                const char* title = "HELL PROJECT"; 
                int titleWidth = MeasureText(title, 50);
                DrawText(title, (screenWidth/2 - titleWidth/2) + 2, 152, 50, Fade(BLACK, 0.5f)); // เงา
                DrawText(title, screenWidth/2 - titleWidth/2, 150, 50, GOLD);
                
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

                // วาด UI HUD 
                PlayerInfo uiData = { (int)pl.hp, pl.maxHp, 50.0f, 50.0f, pl.speed, 1, 100, pl.skillList };
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
            }
              
            // เช็คเมาส์ว่าทำงานไหมระหว่างเล่นเกม
            if (currentState != STATE_PLAYING) {
                DrawFantasyCursor(); 
            } 
        EndDrawing();
    }

    // ปิดหน้าต่างและคืนค่าทรัพยากร
    UnloadTexture(lobbygame);
    CloseWindow();
    return 0;
}
