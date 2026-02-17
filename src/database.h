#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>
#include "raylib.h"
#include "player.h" 


struct MonsterTemplate {
    std::string name;
    int hp;
    int atk;
    Color color;
};


inline std::vector<MonsterTemplate> GetMonsterDb() {
    return {
        {"Normal Rat", 1, 1, GRAY},
        {"Warrior Rat", 10, 5, DARKBLUE},
        {"Muscular Rat", 100, 2, BROWN},
        {"Gunner Rat", 10, 10, PURPLE},
        {"Mickey Mouse", 500, 20, BLACK},
        {"Ninja Rat", 40, 25, DARKGREEN},
        {"Shield Rat", 200, 5, GOLD},
        {"Rat Alchemist", 60, 15, LIME},
        {"Undead Rat", 80, 12, DARKGRAY},
        {"Rat King", 1000, 50, ORANGE}
    };
}

// ฟังก์ชันสำหรับดึงสกิลเริ่มต้น
inline std::vector<Skill> GetDefaultSkills() {
    return {
        {"Fireball", 30, 15},
        {"Heavy Slash", 20, 5},
        {"Healing Light", -40, 20},
        {"Thunder Strike", 50, 25},
        {"Ice Spike", 25, 10},
        {"Ultimate Raid", 100, 45}
    };
}

#endif