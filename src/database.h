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
    float speed;        
    float attackRange;  
    int defense;        
};

inline std::vector<MonsterTemplate> GetMonsterDb() {
    return {
        
        {"Normal Rat", 10, 1, GRAY, 1.0f, 20.0f, 0},
        {"Warrior Rat", 25, 5, DARKBLUE, 1.2f, 25.0f, 2},
        {"Muscular Rat", 100, 2, BROWN, 0.6f, 30.0f, 5},     
        {"Gunner Rat", 15, 10, PURPLE, 1.0f, 150.0f, 0},     
        {"Mickey Mouse", 500, 20, BLACK, 1.5f, 40.0f, 10},
        {"Ninja Rat", 40, 25, DARKGREEN, 2.5f, 20.0f, 1},    
        {"Shield Rat", 200, 5, GOLD, 0.4f, 20.0f, 20},       
        {"Rat Alchemist", 60, 15, LIME, 0.8f, 120.0f, 2},    
        {"Undead Rat", 80, 12, DARKGRAY, 0.5f, 25.0f, 0},    
        {"Rat King", 1000, 50, ORANGE, 0.9f, 60.0f, 15}
    };
}



#endif