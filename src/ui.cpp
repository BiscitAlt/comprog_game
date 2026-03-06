#include "ui.h"
#include <math.h>

/**
 * ฟังก์ชันวาดปุ่มกด (UI Button)
 * ใช้สำหรับตรวจจับเมาส์และวาดกราฟิกปุ่มพร้อมข้อความ
 */
bool DrawMenuButton(Rectangle rect, const char* text, Color baseColor) {
    Vector2 mousePos = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePos, rect);
    
    DrawRectangleRec(rect, Fade(BLACK, 0.6f)); 

    Color borderColor = isHovered ? GOLD : Fade(GOLD, 0.4f);
    DrawRectangleLinesEx(rect, 1, borderColor); 

    int fontSize = 20;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, rect.x + (rect.width/2 - textWidth/2), rect.y + (rect.height/2 - fontSize/2), fontSize, Fade(WHITE, 0.9f));
    
    return (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

// รูปแบบเมาส์ในเกม
void DrawFantasyCursor() {
    Vector2 mPos = GetMousePosition();
    float time = GetTime();

    // 1. วาดแสงเรืองๆ รอบเมาส์ (Outer Glow)
    DrawCircleGradient(mPos.x, mPos.y, 15, Fade(GOLD, 0.3f), BLANK);

    // 2. วาดวงแหวนเวทมนตร์หมุนๆ (Magic Circle)
    DrawCircleLinesV(mPos, 10 + sinf(time * 2) * 2, GOLD); // ขยับเข้าออกเบาๆ
    
    // 3. วาดกากบาทเล็ง (Crosshair)
    DrawLineEx({ mPos.x - 12, mPos.y }, { mPos.x + 12, mPos.y }, 2, Fade(RAYWHITE, 0.7f));
    DrawLineEx({ mPos.x, mPos.y - 12 }, { mPos.x, mPos.y + 12 }, 2, Fade(RAYWHITE, 0.7f));

    // 4. วาดจุดแกนกลางที่สว่างที่สุด
    DrawCircleV(mPos, 3, WHITE);
    DrawCircleLinesV(mPos, 3, BLACK); // ขอบดำให้ตัดกับพื้นหลัง
}

// ฟังก์ชันจัดการพื้นหลังเมนูและละอองไฟ
void UpdateAndDrawMenuBackground(Texture2D lobbygame, std::vector<fireeffect>& embers, int screenWidth, int screenHeight) {
    // แสดงผลเอฟเฟคไaดิว่ะ (Breathing Effect)
    float breathing = sinf(GetTime() * 0.5f) * 5.0f; 
    DrawTexturePro(lobbygame, 
        { 0, 0, (float)lobbygame.width, (float)lobbygame.height }, 
        { -breathing, -breathing, (float)screenWidth + (breathing * 2), (float)screenHeight + (breathing * 2) }, 
        { 0, 0 }, 0.0f, WHITE);

    // 2. เอฟเฟกต์ไฟกะพริบ (Torch Flicker) ทำให้สีจอส้มวูบวาบนิดๆ
    float flicker = sinf(GetTime() * 4.0f) * 0.1f + 0.9f;
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(ORANGE, 0.03f * (1.0f - flicker)));

    // 3. วาดละอองไฟ (Embers) ลอยขึ้น
    for (auto& p : embers) {
        p.pos.y -= p.speed; // ให้ลอยขึ้นข้างบน
        p.pos.x += sinf(GetTime() + p.pos.y) * 0.3f; // ส่ายนิดๆ
        if (p.pos.y < -10) p.pos.y = screenHeight + 10; // ถ้าหลุดจอให้กลับไปเริ่มข้างล่างใหม่

        DrawCircleV(p.pos, 1.5f, Fade(GOLD, p.alpha * flicker)); // วาดจุดไฟสีทองจางๆ
    }
}

void DrawRoguelikeHUD(PlayerInfo player) {
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

}