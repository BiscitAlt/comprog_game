#include "summon_staff.h"
#include "raymath.h"

void InitSummonStaff(SummonStaff& staff, Vector2 pos)
{
    staff.pos = pos;
    staff.size = { 16, 32 };
    staff.pickedUp = false;
    staff.cooldown = 1.2f;
    staff.timer = 0.0f;
}

void UpdateSummonStaff(SummonStaff& staff,
                       Vector2 plPos,
                       Vector2 plSize,
                       float dt)
{
    if (staff.timer > 0)
        staff.timer -= dt;

    if (staff.pickedUp) return;

    Rectangle staffRec = {
        staff.pos.x, staff.pos.y,
        staff.size.x, staff.size.y
    };

    Rectangle playerRec = {
        plPos.x, plPos.y,
        plSize.x, plSize.y
    };

    // เก็บคทา
    if (CheckCollisionRecs(staffRec, playerRec))
        staff.pickedUp = true;
}

void UseSummonStaff(SummonStaff& staff,
                    std::vector<Summon>& summons,
                    Vector2 plPos)
{
    if (staff.timer > 0) return;

    Summon s;
    InitSummon(s, plPos);
    summons.push_back(s);

    staff.timer = staff.cooldown;
}

void DrawSummonStaff(const SummonStaff& staff,
                     Vector2 plPos,
                     Vector2 dir)
{
    if (!staff.pickedUp)
    {
        // วาดคทาบนพื้น
        DrawRectangleV(staff.pos, staff.size, PURPLE);
    }
    else
    {
        // วาดคทาในมือ
        Vector2 handPos = Vector2Add(plPos, Vector2Scale(dir, 18));
        DrawRectangle(handPos.x, handPos.y, 6, 28, DARKPURPLE);
    }
}