#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "player.h"

// โครงสร้างข้อมูลใหม่ (ใส่แทนที่ void เดิม)
struct PlayerInfo {
    int hp;
    int maxHp;
    float stamina;
    float speed;
    int floor;
    int gold;
    std::vector<Skill> skills; 

};

// เปลี่ยนชื่อฟังก์ชันใหม่ให้ครอบคลุม
void DrawRoguelikeHUD(PlayerInfo player, Vector2 mousePos);



#endif