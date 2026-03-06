#include "loot_box.h"

// ===== สร้าง Loot Box =====
void InitLootBox(LootBox& box, Vector2 pos)
{
    box.pos = pos;
    box.size = { 26, 26 };
    box.active = true;   // สำคัญมาก ถ้าไม่มี = ชนแล้วไม่เกิดอะไร
}

// ===== กดEเปิด + สุ่มไอเทม =====
void UpdateLootBox(
    LootBox& box,
    Rectangle playerRec,
    LootType& outLoot,
    bool& opened
)
{
    opened = false;

    if (!box.active) return;

    Rectangle boxRec = {
        box.pos.x, box.pos.y,
        box.size.x, box.size.y
    };

    if (CheckCollisionRecs(playerRec, boxRec)
        && IsKeyPressed(KEY_E))
    {
        outLoot = (LootType)GetRandomValue(0, 4);
        opened = true;
        box.active = false;
    }
}

// ===== วาด Loot Box =====
void DrawLootBox(const LootBox& box)
{
    if (!box.active) return;

    DrawRectangleV(box.pos, box.size, GOLD);
    DrawRectangleLines(
        box.pos.x, box.pos.y,
        box.size.x, box.size.y,
        BLACK
    );
}