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
    #include "skill.h"
    // ===== ประเภทอาวุธ =====
    enum WeaponType {
        WEAPON_NONE,          // มือเปล่า
        WEAPON_SWORD,         // ดาบ
        WEAPON_GUN,           // ปืน
        WEAPON_MAGIC          // เวทมนตร์
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

        SkillState skills;

        // ปิดทุกสกิล
        skills = {};
        skills.voidMeteorTimer = -9999;
        skills.phantomBladeTimer = -9999;
        skills.chainLightningTimer = -9999;
        skills.poisonMistTimer = -9999;
        skills.blackHoleTimer = -9999;
        skills.bloodAuraTimer = -9999;
        skills.shockwaveTimer = -9999;

        // สุ่มสกิล
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
        // ===== Sword =====
        Sword sword;
        SwordType randomType = (SwordType)GetRandomValue(0, 2);
        InitSword(sword, { pl.pos.x + 100, pl.pos.y }, randomType);
        std::vector<SwordWave> swordWaves;

        // ===== Gun =====
        Gun gun;
        GunType type = (GunType)GetRandomValue(0, 1);
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
        MagicWeaponType randomMagic =
        (MagicWeaponType)GetRandomValue(0, 2);

        InitMagicWeapon(magic,{ pl.pos.x - 160, pl.pos.y },randomMagic);
        std::vector<MagicProjectile> magicProjectiles;

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
        float autoAttackTimer = 0;

        // =========================
        // Game Loop
        // =========================
        while (!WindowShouldClose())
        {
            float dt = GetFrameTime();

            // skill 
            UpdateSkills(skills, pl, enemies, dt);
            
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

            // =========================
            // Attack
            // =========================
            
            autoAttackTimer += dt;

    if (autoAttackTimer >= 0.3f && !enemies.empty())
{
    float minDist = 9999;
    int targetIndex = -1;

    // หา enemy ใกล้สุด
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

        Vector2 dir =
        Vector2Normalize(Vector2Subtract(targetPos, pl.pos));

        // ===== Gun =====
        if (currentWeapon == WEAPON_GUN)
        {
            ShootGun(gun, bullets, pl.pos, pl.size, dir, pl.mana);
        }

        // ===== Sword =====
        if (currentWeapon == WEAPON_SWORD && sword.pickedUp)
        {
            UseSword(sword, pl, dir, swordWaves, enemies);
        }

        // ===== Magic =====
        if (currentWeapon == WEAPON_MAGIC)
        {
            TryShootMagic(magic, magicProjectiles, pl.pos, targetPos);
        }
    }

            autoAttackTimer = 0;
}

            // อัปเดตกระสุน

            UpdateBullets(bullets, enemies, dt);

            UpdateSwordWaves(swordWaves, enemies, dt); // คลื่นดาบ
            for (auto it = enemies.begin(); it != enemies.end(); )
            {
                if (it->hp <= 0)
    {
            // Vampiric Blood
            pl.hp += 3;
            if (pl.hp > pl.hpMax)
            pl.hp = pl.hpMax;

            it = enemies.erase(it);
    }
            else
            ++it;
}
            // อัปเดตเวทมนตร์
            UpdateMagicProjectiles(magicProjectiles, enemies, dt);


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
            

            // ===== Heal Potion draw =====
            DrawHealPotion(healPotion);

            if (plInvincibleTimer <= 0
                || ((int)(plInvincibleTimer * 10) % 2 == 0))
            {
                DrawRectangleV(pl.pos, pl.size, pl.color);
            }
            DrawSkillEffects(skills, pl);

            for (const Enemy& e : enemies) DrawEnemy(e);

            // วาดอาวุธ
            if (!sword.pickedUp || currentWeapon == WEAPON_SWORD)
                DrawSword(sword, pl.pos, pl.size, dir);

            if (!gun.pickedUp || currentWeapon == WEAPON_GUN)
                DrawGun(gun, pl.pos, pl.size, dir);

            // วาด Magic 
            DrawMagicWeapon(magic, pl.pos, dir);
            DrawMagicProjectiles(magicProjectiles);

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