#ifndef LOOT_BOX_H
#define LOOT_BOX_H

#include "raylib.h"

// ===== ประเภทของที่สุ่มได้ =====
enum LootType {
    LOOT_HEAL,
    LOOT_SWORD,
    LOOT_GUN,
    LOOT_SUMMON,
    LOOT_MAGIC
};

// ===== โครงสร้าง Loot Box =====
struct LootBox {
    Vector2 pos;
    Vector2 size;
    bool active;   // ยังไม่ถูกเปิด
};

// ===== ฟังก์ชัน =====
void InitLootBox(LootBox& box, Vector2 pos);
void UpdateLootBox(
    LootBox& box,
    Rectangle playerRec,
    LootType& outLoot,
    bool& opened
);
void DrawLootBox(const LootBox& box);

#endif