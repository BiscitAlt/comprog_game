#ifndef HEAL_POTION_H
#define HEAL_POTION_H

#include "raylib.h"
#include "player.h"

struct HealPotion
{
    Vector2 pos;
    float radius;
    int healAmount;
    bool pickedUp;
};

// สร้างน้ำยา
void InitHealPotion(HealPotion& potion, Vector2 pos, int healAmount);

// เช็คเก็บ
void UpdateHealPotion(HealPotion& potion, player& pl);

// วาด
void DrawHealPotion(const HealPotion& potion);

#endif