#ifndef EXP_ORB_H
#define EXP_ORB_H

#include "raylib.h"

struct ExpOrb
{
    Vector2 pos;
    Vector2 size;
    int value;
    bool active;
};

void SpawnExpOrb(ExpOrb& orb, Vector2 pos, int value);
void UpdateExpOrb(ExpOrb& orb, Rectangle playerRec, int& playerExp);
void DrawExpOrb(const ExpOrb& orb);

#endif