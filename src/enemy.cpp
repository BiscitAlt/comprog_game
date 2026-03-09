#include "enemy.h"
#include <algorithm>
#include "raymath.h"

void InitEnemy(Enemy& e, Vector2 pos, EnemyType type)
{
    e.pos = pos;
   
    e.type = type;

    e.currentFrame = 0;
    e.frameTimer = 0;
    e.attackTimer = 0;
    e.bullets.clear();
    e.namemonster = "Monster";
    float scale = 1.0f;

    switch(type)
{
    case MELEE:

        e.texture = LoadTexture("assets/monster/slime.png");
        SetTextureFilter(e.texture, TEXTURE_FILTER_POINT);
        
        scale = 0.2f;   // slime

        e.hp = 100;
        e.speed = 1.4f;
        e.atk = 10;

        e.frameCount = 9;
        e.frameSpeed = 0.1f;

    break;

    case RANGED:

        e.texture = LoadTexture("assets/monster/goblin.png");
        SetTextureFilter(e.texture, TEXTURE_FILTER_POINT);

        scale = 0.18f;   // goblin

        e.hp = 50;
        e.speed = 0.8f;
        e.atk = 5;

        e.frameCount = 6;
        e.frameSpeed = 0.18f;

        e.shootCooldown = 1.8f;
        e.shootRange = 350;
        e.shootTimer = 0;

    break;

    case POISON:

        e.texture = LoadTexture("assets/monster/poison.png");
        SetTextureFilter(e.texture, TEXTURE_FILTER_POINT);

        scale = 0.2f;   // poison monster

        e.hp = 120;
        e.speed = 1.3f;
        e.atk = 1;

        e.frameCount = 7;
        e.frameSpeed = 0.1f;

        e.poisonRadius = 120;
        e.poisonInterval = 0.5f;
        e.poisonTimer = 0;

    break;

    case EXPLODER:

        e.texture = LoadTexture("assets/monster/bomb.png");
        SetTextureFilter(e.texture, TEXTURE_FILTER_POINT);

        scale = 0.12f;   // bomb

        e.hp = 20;
        e.speed = 1.8f;
        e.atk = 45;

        e.frameCount = 4;
        e.frameSpeed = 0.10f;

        e.explodeRadius = 110;

    break;
    }

    int frameWidth = e.texture.width / e.frameCount;

    e.size =
    {
    frameWidth * scale,
    e.texture.height * scale
    };

    e.frameRec =
    {
    0,
    0,
    (float)frameWidth,
    (float)e.texture.height
    };

    e.hpMax = e.hp;
}

void InitEnemy(Enemy& e, Vector2 pos, std::string name, int hp, int atk)
{
    e.pos = pos;
    e.size = {22,22};

    e.type = MELEE;

    e.namemonster = name;

    e.hp = hp;
    e.hpMax = e.hp;

    e.speed = 1.0f;
    e.color = BLUE;

    e.attackTimer = 0;
    e.bullets.clear();
}

void UpdateEnemy(Enemy& e, Vector2 playerPos)
{
    if(e.hp <= 0) return;

    float delta = GetFrameTime();

    Vector2 dir = Vector2Subtract(playerPos, e.pos);
    float dist = Vector2Length(dir);

    if(playerPos.x < e.pos.x)
    e.facingRight = false;
    else
    e.facingRight = true;

    e.frameTimer += GetFrameTime();

if (e.frameTimer >= e.frameSpeed)
{
    e.frameTimer = 0;
    e.currentFrame++;

    if (e.currentFrame >= e.frameCount)
{
    e.currentFrame = 0;
}

e.frameRec.x = e.currentFrame * e.frameRec.width;
}
    if(e.type == RANGED && dist < 200)
    {
        if(dist > 1)
        {
            e.pos = Vector2Subtract(
                e.pos,
                Vector2Scale(Vector2Normalize(dir), e.speed)
            );
        }
    }
    else if(dist > 2)
    {
        e.pos = Vector2Add(
            e.pos,
            Vector2Scale(Vector2Normalize(dir), e.speed)
        );
    }

    // =================
    // RANGED SHOOT
    // =================

    if(e.type == RANGED)
    {
        e.shootTimer += delta;

        if(dist < e.shootRange && e.shootTimer >= e.shootCooldown)
        {
            Bullet b;

            b.pos = e.pos;
            b.vel = Vector2Scale(Vector2Normalize(dir), 300.0f);
            b.radius = 5;
            b.active = true;
            b.type = BulletType::NORMAL;
            b.lifeTime = 3.0f;

            e.bullets.push_back(b);

            e.shootTimer = 0;
        }

        for(auto& b : e.bullets)
        {
            if(!b.active) continue;

            b.pos = Vector2Add(
                b.pos,
                Vector2Scale(b.vel, delta)
            );

            b.lifeTime -= delta;

            if(b.lifeTime <= 0)
                b.active = false;
        }

        e.bullets.erase(
            std::remove_if(
                e.bullets.begin(),
                e.bullets.end(),
                [](Bullet& b){ return !b.active; }
            ),
            e.bullets.end()
        );
    }

    // =================
    // POISON
    // =================

    if(e.type == POISON)
    {
        e.poisonActive = (dist < e.poisonRadius);

        if(e.poisonActive)
            e.poisonTimer += delta;
    } 
    
}


void DrawEnemy(const Enemy& e)
{
    if(e.hp <= 0) return;

    Color c = e.color;

    if(e.type == EXPLODER && ((int)(GetTime()*10)%2==0))
        c = RED;

    Rectangle dest =
{
    e.pos.x,
    e.pos.y,
    e.size.x,
    e.size.y
};

Rectangle src = e.frameRec;

if(!e.facingRight)
{
    src.width = -src.width;
}

DrawTexturePro(
    e.texture,
    src,
    dest,
    {0,0},
    0,
    WHITE
);

    DrawRectangle(e.pos.x, e.pos.y - 10, e.size.x, 5, RED);

    DrawRectangle(
        e.pos.x,
        e.pos.y - 10,
        e.size.x * ((float)e.hp / e.hpMax),
        5,
        LIME
    );

    if(e.type == RANGED)
    {
        for(const auto& b : e.bullets)
        {
            if(b.active)
                DrawCircleV(b.pos, b.radius, MAROON);
        }
    }

    if(e.type == POISON && e.poisonActive)
    {
        DrawCircleLinesV(
            Vector2Add(e.pos,{11,11}),
            e.poisonRadius,
            Fade(GREEN,0.5f)
        );
    }
}