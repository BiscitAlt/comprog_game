#include "player.h"
#include "raymath.h"

// ======================
// Update Player
// ======================
void plUpdate(player &pl, Map &map)
{
    // เก็บตำแหน่งปัจจุบันไว้ก่อน หากเดินชนกำแพงจะได้เด้งกลับมาจุดนี้
    Vector2 oldPos = pl.pos; 
    
    plMovement(pl);

    // เช็คไม่ให้หลุดขอบแผนที่
    plCollision(pl.pos, pl.size, pl.speed, map);

    // เช็คว่าตำแหน่งใหม่ชนกำแพงหรือไม่ (เช็คจาก 4 มุมของตัวละคร)
    bool hitWall = map.IsWall(pl.pos.x, pl.pos.y) || 
                   map.IsWall(pl.pos.x + pl.size.x, pl.pos.y) || 
                   map.IsWall(pl.pos.x, pl.pos.y + pl.size.y) || 
                   map.IsWall(pl.pos.x + pl.size.x, pl.pos.y + pl.size.y);

    // ถ้าตำแหน่งใหม่เป็นกำแพง ให้ดึงตัวละครกลับมาจุดก่อนเดิน
    if (hitWall) {
        pl.pos = oldPos;
    }
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
// Collision (พึ่งพา map.IsWall ใน plUpdate เพื่อกันชนสิ่งกีดขวางอย่างเดียว)
// ======================
void plCollision(Vector2 &plPos, Vector2 plSize, float plSpeed, Map &map)
{
    // คำนวณขนาดที่แท้จริงของแผนที่ (960 x 960)
//    float mapWidth = map.cols * map.tileSize;
//   float mapHeight = map.rows * map.tileSize;

    // กันหลุดขอบซ้าย-บน
//    if (plPos.x < 0) plPos.x = 0;
//    if (plPos.y < 0) plPos.y = 0;

    // กันหลุดขอบขวา-ล่าง (ใช้ขนาดแผนที่แทน GetScreenWidth/Height)
//    if (plPos.x > mapWidth - plSize.x)
//        plPos.x = mapWidth - plSize.x;

//    if (plPos.y > mapHeight - plSize.y)
//        plPos.y = mapHeight - plSize.y;
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