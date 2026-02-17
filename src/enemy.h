#ifndef ENEMY_H 1
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
    int atk;
    float attackTimer;
    Color color;
};

// ประกาศฟังก์ชัน
void InitEnemy(Enemy& e, Vector2 pos); // ค่าพื้นฐาน
void UpdateEnemy(Enemy& e, Vector2 playerPos); 
void DrawEnemy(const Enemy& e); 

#endif