#ifndef ENEMY_H
#define ENEMY_H

// รวม header ไม่ใส่ใน .cpp เพราะจะได้ไม่ต้อง include ซ้ำหลายๆ ที่
#include "raylib.h"
#include "map.h"
#include "item.h"
#include <string>
#include <vector>
//คร่าวๆ 
struct Enemy
{
    Vector2 pos; 
    Vector2 size;
    float speed;
    Color color;
    std::string namemonster;
    int maxHp;
    int attack;
    int hp;
};



// ประกาศฟังก์ชัน
void InitEnemy(Enemy& e, Vector2 pos ,std::string name, int hp, int atk); // ค่าพื้นฐาน
void UpdateEnemy(Enemy& e, Vector2 playerPos); 
void DrawEnemy(const Enemy& e); 

#endif