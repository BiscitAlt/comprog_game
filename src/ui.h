#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "player.h"
#include <vector>
#include <string>

// --- โครงสร้างข้อมูลสำหรับละอองไฟ ---
struct fireeffect {
    Vector2 pos;
    float speed;
    float alpha;
};

// --- โครงสร้างข้อมูล Player สำหรับ HUD ---
struct PlayerInfo {
    int hp;
    int maxHp;
    float stamina;
    float mp; 
    float speed;
    int floor;
    int gold;
    std::vector<Skill> skillList; 
};

// --- ประกาศฟังก์ชัน UI เพื่อให้ main.cpp มองเห็น ---
void DrawRoguelikeHUD(PlayerInfo player);
bool DrawMenuButton(Rectangle rect, const char* text, Color baseColor);
void DrawFantasyCursor();
void UpdateAndDrawMenuBackground(Texture2D lobbygame, std::vector<fireeffect>& embers, int screenWidth, int screenHeight);
void DrawPauseMenu(int screenWidth, int screenHeight, bool* isResumed, bool* isQuit);
#endif