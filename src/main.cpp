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
    #include "summon.h"
    #include "summon_staff.h"
    #include "heal_potion.h"
    #include "magic_weapon.h"
    #include "magic_projectile.h"
    #include "loot_box.h"

    // ===== ประเภทอาวุธ =====
    enum WeaponType {
        WEAPON_NONE,          // มือเปล่า
        WEAPON_SWORD,         // ดาบ
        WEAPON_GUN,           // ปืน
        WEAPON_SUMMON_STAFF,  // คทาซัมมอน
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
        player pl = { plPos, {20, 20}, 2.0f, RED, 100, 100 ,100.0f, 100.0f };

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

        // ===== Summon Staff =====
        SummonStaff staff;
        InitSummonStaff(staff, { pl.pos.x + 150, pl.pos.y });
        std::vector<Summon> summons;

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

            case LOOT_SUMMON:
            staff.pickedUp = true;
            currentWeapon = WEAPON_SUMMON_STAFF;
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
            UpdateSword(sword, pl.pos, pl.size, dir);
            if (sword.pickedUp && currentWeapon != WEAPON_SWORD)
            {
                if (currentWeapon == WEAPON_GUN)
                {
                    gun.pickedUp = false;
                    gun.pos = Vector2Add(pl.pos, { -40, 0 });
                }
                if (currentWeapon == WEAPON_SUMMON_STAFF)
                {
                    staff.pickedUp = false;
                    staff.pos = Vector2Add(pl.pos, { 40, 0 });
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
                if (currentWeapon == WEAPON_SUMMON_STAFF)
                {
                    staff.pickedUp = false;
                    staff.pos = Vector2Add(pl.pos, { -40, 0 });
                }
                if (currentWeapon == WEAPON_MAGIC)
                {
                    magic.pickedUp = false;
                    magic.pos = Vector2Add(pl.pos, { 60, 0 });
                }
                currentWeapon = WEAPON_GUN;
            }

            // ----- Summon Staff -----
            UpdateSummonStaff(staff, pl.pos, pl.size, dt);
            if (staff.pickedUp && currentWeapon != WEAPON_SUMMON_STAFF)
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
                if (currentWeapon == WEAPON_MAGIC)
                {
                    magic.pickedUp = false;
                    magic.pos = Vector2Add(pl.pos, { 60, 0 });
                }
                currentWeapon = WEAPON_SUMMON_STAFF;
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
                if (currentWeapon == WEAPON_SUMMON_STAFF)
                {
                    staff.pickedUp = false;
                    staff.pos = Vector2Add(pl.pos, { -60, 0 });
                }
                currentWeapon = WEAPON_MAGIC;
            }

            // =========================
            // Attack
            // =========================

            // ยิงปืน
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && currentWeapon == WEAPON_GUN)
            {
                ShootGun(gun,bullets,pl.pos,pl.size,dir,pl.mana);
            }

            // ใช้ดาบ
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
            && currentWeapon == WEAPON_SWORD
            && sword.pickedUp)
            {
            UseSword(sword, pl.pos, dir, swordWaves, pl.mana);
            }

            // ใช้คทาซัมมอน
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
                && currentWeapon == WEAPON_SUMMON_STAFF
                && staff.pickedUp)
            {
                UseSummonStaff(staff, summons, pl.pos);
            }

            // ใช้เวทมนตร์
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
                && currentWeapon == WEAPON_MAGIC
                && magic.pickedUp)
            {
                UseMagicWeapon(magic, magicProjectiles, pl.pos, dir);
            }

            // อัปเดตกระสุน
            for (Bullet& b : bullets) UpdateBullet(b);

            UpdateSwordWaves(swordWaves, dt); // คลื่นดาบ

            // อัปเดตเวทมนตร์
            for (MagicProjectile& m : magicProjectiles)
                UpdateMagicProjectile(m, dt);

            // ===== Update Summon =====
            for (Summon& s : summons)
            {
                if (!s.active) continue;
                if (!enemies.empty())
                    UpdateSummon(s, dt, enemies[0].pos);
            }

            // ===== Summon โดนศัตรู =====
            for (Summon& s : summons)
            {
                if (!s.active) continue;
                for (int j = enemies.size() - 1; j >= 0; j--)
                {
                    Rectangle enemyRec = {
                        enemies[j].pos.x, enemies[j].pos.y,
                        enemies[j].size.x, enemies[j].size.y
                    };

                    if (CheckCollisionCircleRec(s.pos, s.radius, enemyRec))
                    {
                        enemies[j].hp -= s.damage;
                        s.active = false;
                        if (enemies[j].hp <= 0)
                            enemies.erase(enemies.begin() + j);
                        break;
                    }
                }
            }

            // ลบ summon ที่หมดอายุ
            for (int i = summons.size() - 1; i >= 0; i--)
                if (!summons[i].active)
                    summons.erase(summons.begin() + i);

            // ===== ศัตรูโจมตีผู้เล่น =====
            for (Enemy& e : enemies)
            {
                e.attackTimer -= dt;
                Rectangle enemyRec = {
                    e.pos.x, e.pos.y,
                    e.size.x, e.size.y
                };

                if (CheckCollisionRecs(playerRec, enemyRec)
                    && e.attackTimer <= 0
                    && plInvincibleTimer <= 0)
                {
                    pl.hp -= e.atk;
                    e.attackTimer = 0.5f;
                    plInvincibleTimer = 0.7f;
                    plHitTimer = 0.3f;
                }
            }

            if (pl.hp <= 0) break;

            // =========================
            // Draw
            // =========================
            BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);

            gridMap.Draw();
            
            // ===== loot box draw =====
            DrawLootBox(lootBox);
            if (lootBox.active &&
            CheckCollisionRecs(playerRec, {
            lootBox.pos.x, lootBox.pos.y,
            lootBox.size.x, lootBox.size.y
            } ) )
            {
            DrawText("Press E",
            lootBox.pos.x - 10,
            lootBox.pos.y - 20,
            12,
            BLACK
            );
            }

            // ===== Heal Potion draw =====
            DrawHealPotion(healPotion);

            if (plInvincibleTimer <= 0
                || ((int)(plInvincibleTimer * 10) % 2 == 0))
            {
                DrawRectangleV(pl.pos, pl.size, pl.color);
            }

            for (const Enemy& e : enemies) DrawEnemy(e);

            // วาดซัมมอน
            for (const Summon& s : summons) DrawSummon(s);

            // วาดอาวุธ
            if (!sword.pickedUp || currentWeapon == WEAPON_SWORD)
                DrawSword(sword, pl.pos, pl.size, dir);

            if (!gun.pickedUp || currentWeapon == WEAPON_GUN)
                DrawGun(gun, pl.pos, pl.size, dir);

            DrawSummonStaff(staff, pl.pos, dir);

            // วาด Magic (เพิ่ม)
            DrawMagicWeapon(magic, pl.pos, dir);
            for (const MagicProjectile& m : magicProjectiles)
                DrawMagicProjectile(m);

            for (const Bullet& b : bullets) DrawBullet(b);
            DrawSwordWaves(swordWaves);

            EndMode2D();

            DrawText(
                TextFormat("HP: %d / %d", pl.hp, pl.hpMax),
                10, 40, 20, BLACK
            );

            DrawText(
            TextFormat("Mana: %.0f / %.0f", pl.mana, pl.manaMax),
            10, 65, 20, BLUE
            );

            cursor(GetMousePosition());
            EndDrawing();
        }

        CloseWindow();
    return 0;
}