#include "map.h"

Map::Map() {
    // ไม่ต้องมี GenerateNewRoom แล้ว เริ่มมา map โล่งๆ ก่อน
}

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

    // 2. ถ้าสิ่งกีดขวางน้อยเกินไป ให้สุ่มเกิดใหม่รอบๆ ผู้เล่น
    if (obstacles.size() < 20) { // จำกัดจำนวนสูงสุดในฉาก
        Vector2 newPos;
        // สุ่มมุม (Angle) และระยะทาง (Distance) รอบๆ ตัว
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        float dist = GetRandomValue((int)spawnRadius - 200, (int)spawnRadius);
        
        // ทำให้ตำแหน่งลงล็อค Grid ขนาด tileSize (32)
        newPos.x = playerPos.x + (cos(angle) * dist);
        newPos.y = playerPos.y + (sin(angle) * dist);
        newPos.x = round(newPos.x / tileSize) * tileSize;
        newPos.y = round(newPos.y / tileSize) * tileSize;

        Obstacle obs; 
        obs.pos = newPos;
        obs.type = 23; // สุ่มไอดีสิ่งกีดขวางจาก Tileset ของคุณ
        
        obstacles.push_back(obs);
        if (obs.type == 23) for(int i = 0; i < 4; i++) {
            obs.pos.y = obs.pos.y - tileSize; // วางเรียงขึ้นไป
            obs.type = 24 + i; // สุ่มไอดีสิ่งกีดขวางจาก Tileset ของคุณ
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
            Rectangle crop = {0, 0, 16, 16}; // สมมติว่า Tile พื้นคือช่องซ้ายบนสุด
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