#include "map.h"

void Map::LoadAssets() {
    tileset = LoadTexture("assets\\tileset.png");
    
    // ถ้าโหลดไม่สำเร็จ id ของ texture จะเป็น 0
    if (tileset.id == 0) tileset = LoadTexture("src\\assets\\tileset.png");
    if (tileset.id == 0) TraceLog(LOG_ERROR, "FAILED TO LOAD TILESET! Check path: assets/tileset.png");
    else TraceLog(LOG_INFO, "TILESET LOADED SUCCESSFULLY!");
}

void Map::UnloadAssets() {
    UnloadTexture(tileset);
}

void Map::UpdateEndless(Vector2 playerPos) {
    float spawnRadius = 800.0f; // ระยะที่จะเสกสิ่งกีดขวาง (นอกจอหน่อยๆ)
    float despawnRadius = 1200.0f; // ถ้ายืนห่างเกินระยะนี้ ลบสิ่งกีดขวางทิ้ง

    // 1. ลบสิ่งกีดขวางที่อยู่ไกลเกินไป (ลดภาระเครื่อง)
    for (int i = obstacles.size() - 1; i >= 0; i--) {
        float distX = playerPos.x - obstacles[i].pos.x;
        float distY = playerPos.y - obstacles[i].pos.y;
        float distSq = (distX * distX) + (distY * distY);
        
        if (distSq > despawnRadius * despawnRadius) {
            obstacles.erase(obstacles.begin() + i);
        }
    }

    // 2. สุ่มสร้างสิ่งกีดขวางใหม่ถ้าจำนวนยังไม่เยอะเกินไป
    if (obstacles.size() < 50) {
        Vector2 newPos;
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        float dist = GetRandomValue(600, 800); // ระยะ spawn
        
        newPos.x = round((playerPos.x + (cos(angle) * dist)) / tileSize) * tileSize;
        newPos.y = round((playerPos.y + (sin(angle) * dist)) / tileSize) * tileSize;

        // ==========================================
        // ระบบ Blueprint: สร้างแบบแปลนสิ่งก่อสร้าง
        // ==========================================
        
        // 1. แบบแปลน "เสาหิน" 5 ชั้น (เรียงขึ้นไปด้านบน เลยใช้ Y ติดลบ)
        std::vector<BlueprintPiece> pillarBlueprint = {
            {0,  0, 23}, // ฐานล่างสุด
            {0, -1, 24}, // ชั้น 2
            {0, -2, 25}, // ชั้น 3
            {0, -3, 26}, // ชั้น 4
            {0, -4, 27}  // ยอดเสาบนสุด
        };

        // 2. แบบแปลน "กำแพง 2x2" (ตัวอย่างของชิ้นใหญ่กว้างๆ)
        std::vector<BlueprintPiece> boxBlueprint = {
            {0, 0, 14}, {1, 0, 15}, // ซ้ายบน, ขวาบน
            {0, 1, 20}, {1, 1, 21}  // ซ้ายล่าง, ขวาล่าง
        };

        // สุ่มว่าจะสร้างสิ่งก่อสร้างแบบไหน
        int randStructure = GetRandomValue(0, 1);
        std::vector<BlueprintPiece> selectedBlueprint;
        
        switch (randStructure)
        {
        case 0:
            selectedBlueprint = pillarBlueprint;
            break;
        case 1:
            selectedBlueprint = boxBlueprint;
            break;
        }

        // วนลูปเสกชิ้นส่วนตามแบบแปลนที่สุ่มได้
        for (const auto& piece : selectedBlueprint) {
            Obstacle obs;
            obs.pos.x = newPos.x + (piece.offsetX * tileSize);
            obs.pos.y = newPos.y + (piece.offsetY * tileSize);
            obs.type = piece.type;
            obstacles.push_back(obs);
        }
    }
}

void Map::Draw(Vector2 playerPos, int screenWidth, int screenHeight) {
    
    // --- 1. วาดพื้น (วาดเฉพาะบริเวณหน้าจอผู้เล่น) ---
    // หาพิกัดมุมซ้ายบนของกล้อง (สมมติกล้องอยู่กึ่งกลางผู้เล่น)
    float startX = playerPos.x - (screenWidth / 2.0f);
    float startY = playerPos.y - (screenHeight / 2.0f);
    
    // ปัดเศษให้ลงล็อค tileSize เพื่อไม่ให้พื้นกระตุกเวลาเดิน
    int firstCol = (int)floor(startX / tileSize);
    int firstRow = (int)floor(startY / tileSize);
    
    int colsToDraw = (screenWidth / tileSize) + 2;
    int rowsToDraw = (screenHeight / tileSize) + 2;

    for (int y = firstRow; y < firstRow + rowsToDraw; y++) {
        for (int x = firstCol; x < firstCol + colsToDraw; x++) {
            Rectangle crop = {0, 0, 16, 16};
            
            // ใช้สมการ Hash (เอาพิกัดมาคูณเลข Prime) เพื่อให้ค่า x, y เดิม ได้เลขเดิมเสมอ
            int noise = (x * 73856093) ^ (y * 19349663);
            int tileIndex = abs(noise) % 4; // สมมติว่ามีพื้น 4 ลาย (0 ถึง 3)
            crop.x = tileIndex * 16;
            
            Rectangle dest = {(float)(x * tileSize), (float)(y * tileSize), (float)tileSize, (float)tileSize};
            DrawTexturePro(tileset, crop, dest, {0, 0}, 0.0f, WHITE);
        }
    }

    // --- 2. วาดสิ่งกีดขวาง ---
    for (const auto& obs : obstacles) {
        Rectangle crop = {0, 0, 16, 16};
        // คำนวณ crop ตาม type ที่สุ่มมา (ใช้โค้ดเดิมของคุณประยุกต์ได้เลย)
        if (obs.type >= 23 && obs.type <= 27) {
            crop.y = 112;
            crop.x = (obs.type - 23) * 16;
        }
        if (obs.type >= 14 && obs.type <= 21) {
            crop.y = 16;
            crop.x = (obs.type - 14) * 16;
        }
        
        Rectangle dest = {obs.pos.x, obs.pos.y, (float)tileSize, (float)tileSize};
        DrawTexturePro(tileset, crop, dest, {0, 0}, 0.0f, WHITE);
    }
}

bool Map::IsWall(float x, float y) {
    // เช็คว่าพิกัดที่เดินไป ชนกับสิ่งกีดขวางใน Vector หรือไม่
    for (const auto& obs : obstacles) {
        Rectangle obsRec = { obs.pos.x, obs.pos.y, (float)tileSize, (float)tileSize };
        if (CheckCollisionPointRec({x, y}, obsRec)) {
            return true;
        }
    }
    return false;
}