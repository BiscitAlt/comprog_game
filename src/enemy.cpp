#include "enemy.h"
#include <algorithm>

void InitEnemy(Enemy& e, Vector2 pos, EnemyType type) {
    e.pos = pos;
    e.size = { 22, 22 };
    e.type = type;
    e.attackTimer = 0;
    e.bullets.clear();
    e.namemonster = "Monster";

    switch(type) {
        case MELEE: 
            e.hp = 6; e.speed = 1.3f; e.color = BLUE; e.atk = 10; 
            break;
        case RANGED: 
            e.hp = 4; e.speed = 0.8f; e.color = ORANGE; e.atk = 5; 
            e.shootCooldown = 1.8f; e.shootRange = 350; e.shootTimer = 0;
            break;
        case POISON: 
            e.hp = 8; e.speed = 1.1f; e.color = GREEN; e.atk = 2; 
            e.poisonRadius = 80; e.poisonInterval = 0.5f; e.poisonTimer = 0;
            break;
        case EXPLODER: 
            e.hp = 2; e.speed = 1.8f; e.color = YELLOW; e.atk = 45; 
            e.explodeRadius = 110; 
            break;
    }
    e.hpMax = e.hp;
    e.maxHp = e.hp;
    e.attack = e.atk;
}

void InitEnemy(Enemy& e, Vector2 pos, std::string name, int hp, int atk) {
    e.pos = pos;
    e.size = { 22, 22 };
    e.type = MELEE;
    e.namemonster = name;
    e.hp = hp;
    e.hpMax = hp;
    e.maxHp = hp;
    e.atk = atk;
    e.attack = atk;
    e.speed = 1.0f;
    e.color = BLUE;
    e.attackTimer = 0;
    e.bullets.clear();
}

void UpdateEnemy(Enemy& e, Vector2 playerPos) {
    if (e.hp <= 0) return;

    float delta = GetFrameTime();
    Vector2 dir = Vector2Subtract(playerPos, e.pos);
    float dist = Vector2Length(dir);

    if (e.type == RANGED && dist < 200) { 
        if (dist > 1) e.pos = Vector2Subtract(e.pos, Vector2Scale(Vector2Normalize(dir), e.speed));
    } else if (dist > 2) {
        e.pos = Vector2Add(e.pos, Vector2Scale(Vector2Normalize(dir), e.speed));
    }

    if (e.type == RANGED) {
        e.shootTimer += delta;
        if (dist < e.shootRange && e.shootTimer >= e.shootCooldown) {
            e.bullets.push_back({e.pos, Vector2Scale(Vector2Normalize(dir), 300.0f), 5, true});
            e.shootTimer = 0;
        }
        for (auto& b : e.bullets) b.pos = Vector2Add(b.pos, Vector2Scale(b.velocity, delta));
        
        e.bullets.erase(std::remove_if(e.bullets.begin(), e.bullets.end(), 
            [&e](Bullet& b){ return Vector2Distance(b.pos, e.pos) > 1000; }), e.bullets.end());
    }

    if (e.type == POISON) {
        e.poisonActive = (dist < e.poisonRadius);
        if (e.poisonActive) e.poisonTimer += delta;
    }
}

void DrawEnemy(const Enemy& e) {
    if (e.hp <= 0) return;

    Color c = e.color;
    if (e.type == EXPLODER && ((int)(GetTime()*10)%2==0)) c = RED;
    DrawRectangleV(e.pos, e.size, c);

    DrawRectangle(e.pos.x, e.pos.y - 10, e.size.x, 5, RED);
    DrawRectangle(e.pos.x, e.pos.y - 10, e.size.x * ((float)e.hp/e.hpMax), 5, LIME);

    if (e.type == RANGED) {
        for (const auto& b : e.bullets) DrawCircleV(b.pos, b.radius, MAROON);
    }
    if (e.type == POISON && e.poisonActive) {
        DrawCircleLinesV(Vector2Add(e.pos, {11,11}), e.poisonRadius, Fade(GREEN, 0.5f));
    }
}