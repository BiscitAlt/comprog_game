#include "heal_potion.h"

// สร้างน้ำยา
void InitHealPotion(HealPotion& potion, Vector2 pos, int healAmount)
{
    potion.pos = pos;
    potion.radius = 8.0f;
    potion.healAmount = healAmount;
    potion.pickedUp = false;
}

// เช็คเก็บ + ฮีล
void UpdateHealPotion(HealPotion& potion, player& pl)
{
    if (potion.pickedUp) return;

    Rectangle playerRec = {
        pl.pos.x, pl.pos.y,
        pl.size.x, pl.size.y
    };

    if (CheckCollisionCircleRec(potion.pos, potion.radius, playerRec))
    {
        pl.hp += potion.healAmount;
        if (pl.hp > pl.hpMax)
            pl.hp = pl.hpMax;

        potion.pickedUp = true;
    }
}

// วาดน้ำยา
void DrawHealPotion(const HealPotion& potion)
{
    if (potion.pickedUp) return;

    DrawCircleV(potion.pos, potion.radius, RED);
    DrawCircleLines(potion.pos.x, potion.pos.y, potion.radius, RED);
}