#include "player.h"
#include "raymath.h"
#include "map.h"

// ======================
// Update Player
// ======================
void plUpdate(player &pl, Map &map)
{
    // เก็บตำแหน่งปัจจุบันไว้ก่อน หากเดินชนกำแพงจะได้เด้งกลับมาจุดนี้
    Vector2 oldPos = pl.pos; 
    
    plMovement(pl);

    // เช็คว่าตำแหน่งใหม่ชนกำแพงหรือไม่
    if (IsEntityColliding(pl.pos, pl.size, map)) pl.pos = oldPos;
}

// ======================
// Movement
// ======================
void plMovement(player &pl)
{
    bool moving = false;

    if (IsKeyDown(KEY_W))
    {
        pl.pos.y -= pl.speed;
        moving = true;
    }

    if (IsKeyDown(KEY_S))
    {
        pl.pos.y += pl.speed;
        moving = true;
    }

    if (IsKeyDown(KEY_A))
{
    pl.pos.x -= pl.speed;
    pl.faceRight = false;   // หันซ้าย
    moving = true;
}

if (IsKeyDown(KEY_D))
{
    pl.pos.x += pl.speed;
    pl.faceRight = true;    // หันขวา
    moving = true;
}

    if (moving)
    {
        pl.frameTime += GetFrameTime();

        if (pl.frameTime >= 0.15f)
        {
            pl.frame++;
            pl.frameTime = 0;

            if (pl.frame > 3)
                pl.frame = 0;
        }
    }
    else
    {
        pl.frame = 0;
    }
}

// ======================
// Draw Player
// ======================
void DrawPlayer(player &pl)
{
    int frameWidth = pl.texture.width / 4;
    int frameHeight = pl.texture.height;

    float scale = 0.125f;

    Rectangle source =
    {
        (float)pl.frame * frameWidth,
        0,
        (float)frameWidth,
        (float)frameHeight
    };

    if (!pl.faceRight)
        source.width = -frameWidth; // flip ซ้าย

    Rectangle dest =
    {
        pl.pos.x,
        pl.pos.y,
        frameWidth * scale,
        frameHeight * scale
    };

    Vector2 origin =
    {
        frameWidth * scale / 2.0f,
        frameHeight * scale / 2.0f
    };

    DrawTexturePro(pl.texture, source, dest, origin, 0.0f, WHITE);
}