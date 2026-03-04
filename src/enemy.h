#ifndef ENEMY_H
#define ENEMY_H

// รวม header ไม่ใส่ใน .cpp เพราะจะได้ไม่ต้อง include ซ้ำหลายๆ ที่
#include "raylib.h"
#include "map.h"
#include "item.h"

//คร่าวๆ 
struct Enemy
{
    Vector2 pos;
    Vector2 size;
    float speed;
    int hp;
    int hpMax;
    int atk;
    float attackTimer;
    Color color;

    float burnTimer = 0.0f;

    float slowTimer = 0.0f;
    float freezeTimer = 0.0f;
    int iceStack = 0;
    float slowPower = 1.0f;

    float electrifiedTimer = 0.0f;
};

// ประกาศฟังก์ชัน
void InitEnemy(Enemy& e, Vector2 pos); // ค่าพื้นฐาน
void UpdateEnemy(Enemy& e, Vector2 playerPos); 
void DrawEnemy(const Enemy& e); 

#endif