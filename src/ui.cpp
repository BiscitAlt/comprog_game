#include "raylib.h"

void textKey(float speed)
{
    DrawText("WASD or Arrow keys to move", 10, 700, 20, DARKGRAY);
    DrawText("Mouse Wheel to Zoom in-out", 10, 680, 20, DARKGRAY);
    DrawText("R to reset Zoom", 10, 660, 20, DARKGRAY);
    DrawText(TextFormat("Speed: %f", speed), 10, 10, 20, DARKGRAY);
}

void DrawRoguelikeHUD(PlayerInfo player, Vector2 mousePos) {
    // --- ตั้งค่าตำแหน่ง ---
    int x = 25, y = 25;
    int w = 260, h = 30;
    
    // 1. วาดเงาพื้นหลัง (Shadow)
    DrawRectangle(x + 5, y + 5, w, h, Fade(BLACK, 0.4f));

    // 2. วาดกรอบนอกสุด (Border) - สีทองหม่นๆ 
    DrawRectangleLinesEx({(float)x - 2, (float)y - 2, (float)w + 4, (float)h + 4}, 2, GetColor(0x8B4513FF)); 
    DrawRectangle(x, y, w, h, BLACK); // พื้นหลังดำสนิท

    // 3. คำนวณเปอร์เซ็นต์เลือด
    float hpPct = (player.maxHp > 0) ? (float)player.hp / player.maxHp : 0;
    if (hpPct < 0) hpPct = 0;

    // 4. เอฟเฟกต์ "เลือดน้อยขอบแดงกระพริบ"
    if (hpPct < 0.25f) {
        float blink = (sinf(GetTime() * 10.0f) + 1.0f) / 2.0f; // ทำไฟกระพริบ
        DrawRectangleLinesEx({(float)x, (float)y, (float)w, (float)h}, 2, Fade(RED, blink));
    }

    // 5. วาดเนื้อเลือด (HP Fill) - ใช้ Gradient ไล่สี
    // เราจะแบ่งเป็น 2 ชั้น: ชั้นล่างสีมืด ชั้นบนสีสว่าง
    int fillW = (int)((w - 6) * hpPct);
    if (fillW > 0) {
        // ไล่สีจาก แดงสด -> แดงเข้ม
        DrawRectangleGradientV(x + 3, y + 3, fillW, h - 6, RED, MAROON);
        
        // เพิ่ม "เส้นเงา" (Gloss) ด้านบนให้ดูมีมิติเหมือนแก้ว
        DrawRectangle(x + 3, y + 3, fillW, (h - 6) / 2, Fade(WHITE, 0.2f));
    }

    // 6. วาดข้อความ HP ตัวหนาๆ
    std::string txt = TextFormat("%i / %i", player.hp, player.maxHp);
    // วาดเงาตัวหนังสือ
    DrawText(txt.c_str(), x + 12, y + 7, 20, BLACK);
    // วาดตัวหนังสือจริง (ถ้าเลือดน้อยให้เปลี่ยนเป็นสีเหลือง)
    Color txtCol = (hpPct < 0.25f) ? YELLOW : RAYWHITE;
    DrawText(txt.c_str(), x + 10, y + 5, 20, txtCol);

    // 7. เป้าเล็ง Crosshair 
    DrawCircleLines(mousePos.x, mousePos.y, 10, Fade(RED, 0.5f)); // วงนอก
    DrawCircle(mousePos.x, mousePos.y, 2, RED); // จุดไข่ปลาตรงกลาง
    
    // ขีดเป้าเล็ง 4 ทิศ
    DrawLineEx({mousePos.x - 15, mousePos.y}, {mousePos.x - 5, mousePos.y}, 2, RED);
    DrawLineEx({mousePos.x + 15, mousePos.y}, {mousePos.x + 5, mousePos.y}, 2, RED);
    DrawLineEx({mousePos.x, mousePos.y - 15}, {mousePos.x, mousePos.y - 5}, 2, RED);
    DrawLineEx({mousePos.x, mousePos.y + 15}, {mousePos.x, mousePos.y + 5}, 2, RED);

    // โชว์พิกัดข้างเป้าเล็งแบบจางๆ
    DrawText(TextFormat("%i,%i", (int)mousePos.x, (int)mousePos.y), mousePos.x + 15, mousePos.y + 15, 10, Fade(LIME, 0.8f));
}