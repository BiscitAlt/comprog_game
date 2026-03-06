#include "exp_orb.h"

void SpawnExpOrb(ExpOrb& orb, Vector2 pos, int value)
{
    orb.pos = pos;
    orb.size = {10, 10};
    orb.value = value;
    orb.active = true;
}

void UpdateExpOrb(ExpOrb& orb, Rectangle playerRec, int& playerExp)
{
    if (!orb.active) return;

    Rectangle orbRec = {
        orb.pos.x,
        orb.pos.y,
        orb.size.x,
        orb.size.y
    };

    if (CheckCollisionRecs(playerRec, orbRec))
    {
        playerExp += orb.value;
        orb.active = false;
    }
}

void DrawExpOrb(const ExpOrb& orb)
{
    if (orb.active)
        DrawCircleV(orb.pos, 5, GREEN);
}